#!/usr/bin/env python3
# Wertet Messsätze von hw_suite.sh aus: rendert die Testprogramme mit einem oder mehreren Emu64-Ständen und
# vergleicht sie, zusammen mit den Referenzbildern der VICE-Testbench, gegen die Aufnahmen jedes Geräts.
#
#   hw_suite_eval.py <gerät> [<gerät> ...]                     aktueller Stand (EMU64_BIN) und Referenz
#   hw_suite_eval.py --emu alt=/pfad/a/emu64 --emu neu=/pfad/b/emu64 <gerät> ...
#   hw_suite_eval.py --all                                     alle gemessenen Geräte
#   hw_suite_eval.py --identify <gerät>                        nur VIC-Typ erkennen (ruft hw_suite.sh auf)
#
# Ausgabe je Gerät: RMSE (kleiner = näher an der Hardware) je Test und strittiger Stelle (suite_regions.txt),
# der beste Wert ist mit * markiert (innerhalb von 5 % gelten Werte als gleich gut). Am Ende eine Übersicht:
# wie oft war welcher Stand der beste, getrennt nach VIC-Typ.
#
# Gerenderte Bilder werden unter $CAPTURE_DIR/_render/<name>/ zwischengespeichert und neu erzeugt, wenn die
# Binary neuer ist. Für Varianten derselben Binary per Umgebungsvariable verschiedene Namen verwenden und
# --rerender angeben.

import argparse
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

HW_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HW_DIR)
import hwcompare  # noqa: E402

MODEL_NAMES = {0: '6569', 1: '8565'}
TIE = 1.05
RENDER_JOBS = 6     # mehr gleichzeitige Emu64-Instanzen führten zu Abstürzen beim Start


def shell_config():
    """Einstellungen aus hw_config.sh (inklusive config.local.sh) holen."""
    out = subprocess.run(['bash', '-c', 'source "%s/hw_config.sh"; echo "$CAPTURE_DIR"; echo "$VICE_TESTBENCH"; '
                          'echo "$EMU64_BIN"' % HW_DIR], capture_output=True, text=True, check=True).stdout.split('\n')
    return out[0], os.path.normpath(os.path.join(out[1], '..')), out[2]


def read_tests():
    tests = []
    for line in open(os.path.join(HW_DIR, 'suite_tests.txt')):
        line = line.split('#')[0].split()
        if line:
            tests.append((line[0], int(line[1])))
    return tests


def read_regions():
    regions = {}
    for line in open(os.path.join(HW_DIR, 'suite_regions.txt')):
        line = line.split('#')[0].split()
        if line:
            regions.setdefault(line[0], []).append((line[1], line[2]))
    return regions


def test_name(path):
    return os.path.splitext(os.path.basename(path))[0]


def reference(testprogs, path, model):
    base = os.path.join(testprogs, os.path.dirname(path), 'references', os.path.basename(path))
    ref = base + ('-8565.png' if model == 1 else '.png')
    return ref if os.path.exists(ref) else None


def render_all(jobs, rerender):
    """jobs: [(binary, prg, png, model, cycles)] -> fehlende/veraltete Bilder parallel rendern."""
    todo = []
    for binary, prg, png, model, cycles in jobs:
        if rerender or not os.path.exists(png) or os.path.getmtime(png) < os.path.getmtime(binary):
            todo.append((binary, prg, png, model, cycles))

    def run(job):
        binary, prg, png, model, cycles = job
        os.makedirs(os.path.dirname(png), exist_ok=True)
        env = dict(os.environ, EMU64_BIN=binary)
        r = subprocess.run([os.path.join(HW_DIR, 'emu_screenshot.sh'), prg, png, str(model), str(cycles)], env=env)
        return r.returncode == 0

    if todo:
        print('Rendere %d Bilder ...' % len(todo), flush=True)
        with ThreadPoolExecutor(RENDER_JOBS) as ex:
            ok = list(ex.map(run, todo))
        if not all(ok):
            sys.exit('Rendern fehlgeschlagen, Abbruch')


def identify(device, capdir_root, testprogs, emu_bin, render_root, tests):
    path, cycles = tests[0]
    name = test_name(path)
    cap = os.path.join(capdir_root, device, name)
    pngs = {m: os.path.join(render_root, 'standard', '%s-m%d.png' % (name, m)) for m in (0, 1)}
    render_all([(emu_bin, os.path.join(testprogs, path), pngs[m], m, cycles) for m in (0, 1)], False)
    hwcompare.fit_geometry(cap, [pngs[0], pngs[1]], verbose=False)
    rm = {m: hwcompare.region_rmse(cap, pngs[m], [('Modusblock', '160:198,60:140')])['Modusblock'] for m in (0, 1)}
    model = 0 if rm[0] < rm[1] else 1
    sure = max(rm.values()) / max(min(rm.values()), 0.01) > 1.3
    with open(os.path.join(capdir_root, device, 'model.txt'), 'w') as f:
        f.write('%d\n' % model)
    print('%s: VIC %s erkannt (Abweichung 6569 %.1f, 8565 %.1f)%s' % (
        device, MODEL_NAMES[model], rm[0], rm[1], '' if sure else '  ACHTUNG: nicht eindeutig'))
    return model


def main():
    ap = argparse.ArgumentParser(description='Messsätze von hw_suite.sh auswerten')
    ap.add_argument('devices', nargs='*')
    ap.add_argument('--all', action='store_true', help='alle gemessenen Geräte')
    ap.add_argument('--emu', action='append', default=[], help='<name>=<emu64-binary>, mehrfach möglich')
    ap.add_argument('--no-ref', action='store_true', help='Referenzbilder der VICE-Testbench nicht vergleichen')
    ap.add_argument('--test', action='append', help='nur diese Tests (Dateiname ohne .prg)')
    ap.add_argument('--rerender', action='store_true', help='Emu64-Bilder neu erzeugen')
    ap.add_argument('--identify', action='store_true', help='nur VIC-Typ erkennen')
    a = ap.parse_args()

    capture_dir, testprogs, emu_bin = shell_config()
    capdir_root = os.path.join(capture_dir, 'suite')
    render_root = os.path.join(capture_dir, '_render')
    tests = read_tests()
    regions = read_regions()

    devices = a.devices
    if a.all:
        devices = sorted(d for d in os.listdir(capdir_root) if os.path.isdir(os.path.join(capdir_root, d)))
    if not devices:
        ap.error('kein Gerät angegeben')

    if a.identify:
        for d in devices:
            identify(d, capdir_root, testprogs, emu_bin, render_root, tests)
        return

    emus = []
    for e in a.emu or ['aktuell=%s' % emu_bin]:
        label, binary = e.split('=', 1)
        if not os.access(binary, os.X_OK):
            sys.exit('Binary nicht gefunden: %s' % binary)
        emus.append((label, os.path.abspath(binary)))
    if a.test:
        tests = [t for t in tests if test_name(t[0]) in a.test]

    models = {}
    for d in devices:
        mf = os.path.join(capdir_root, d, 'model.txt')
        models[d] = int(open(mf).read()) if os.path.exists(mf) else \
            identify(d, capdir_root, testprogs, emu_bin, render_root, read_tests())

    jobs = []
    for label, binary in emus:
        for path, cycles in tests:
            for m in sorted(set(models.values())):
                jobs.append((binary, os.path.join(testprogs, path),
                             os.path.join(render_root, label, '%s-m%d.png' % (test_name(path), m)), m, cycles))
    render_all(jobs, a.rerender)

    wins = {}
    for d in devices:
        m = models[d]
        print('\n=== %s (VIC %s)   RMSE, * = am nächsten an der Hardware' % (d, MODEL_NAMES[m]))
        labels = [e[0] for e in emus] + ([] if a.no_ref else ['Referenz'])
        print('%-22s %-12s %s' % ('Test', 'Stelle', ' '.join('%10s' % l for l in labels)))
        for path, _ in tests:
            name = test_name(path)
            cap = os.path.join(capdir_root, d, name)
            if not os.path.isdir(cap):
                print('%-22s (keine Aufnahme)' % name)
                continue
            hyps = [(label, os.path.join(render_root, label, '%s-m%d.png' % (name, m))) for label, _ in emus]
            ref = None if a.no_ref else reference(testprogs, path, m)
            if not a.no_ref:
                hyps.append(('Referenz', ref))
            if not os.path.exists(os.path.join(cap, 'geometry.txt')):
                hwcompare.fit_geometry(cap, [hyps[0][1]], verbose=False)
            regs = [('Gesamt', '20:250,20:360')] + regions.get(name, [])
            res = {label: (hwcompare.region_rmse(cap, png, regs) if png else None) for label, png in hyps}
            for rname, _ in regs:
                vals = {l: r[rname] for l, r in res.items() if r is not None}
                best = min(vals.values())
                cells = []
                for l in labels:
                    if l not in vals:
                        cells.append('%10s' % '-')
                        continue
                    star = vals[l] <= best * TIE
                    cells.append('%9.2f%s' % (vals[l], '*' if star else ' '))
                    if star and rname != 'Gesamt' and len(vals) > 1 and max(vals.values()) > best * TIE:
                        wins.setdefault(MODEL_NAMES[m], {}).setdefault(l, 0)
                        wins[MODEL_NAMES[m]][l] += 1
                print('%-22s %-12s %s' % (name, rname, ' '.join(cells)))

    print('\n=== Übersicht: wie oft am nächsten an der Hardware (nur strittige Stellen mit Unterschied)')
    for model, per in sorted(wins.items()):
        print('VIC %s: %s' % (model, ', '.join('%s %d' % (l, n) for l, n in sorted(per.items(), key=lambda x: -x[1]))))


if __name__ == '__main__':
    main()
