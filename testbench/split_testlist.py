#!/usr/bin/env python3
"""Teilt die Testliste der VICE Testbench in Pakete auf, die die Worker nacheinander abholen.

Aufruf: split_testlist.py <testliste> <worker> <ausgabeverzeichnis> [gruppe ...]

- Tests, die dieselbe Image Datei einbinden (Optionen "mountd64:", "mountg64:", "mountcrt:" ...), dürfen nicht
  parallel laufen, weil Emu64 in das eingebundene Image zurückschreibt. Sie bleiben in einem Paket (in der
  Reihenfolge der Testliste). Bindet ein Test mehrere Images ein, werden deren Gruppen zusammengelegt.
- Alle anderen Tests (reine Programme, Exitcode oder Screenshot) sind unabhängig. Sie werden je Verzeichnis
  in Pakete zerlegt (Reihenfolge der Testliste bleibt erhalten), damit große Verzeichnisse wie
  Lorenz-2.15 oder irqdma auf mehrere Worker verteilt werden.
- Kosten eines Tests = sein Zyklenlimit (Spalte 4) plus ein fester Aufschlag für den Programmstart.
  Ein Paket wird bis etwa Gesamtkosten / (Worker * PACKAGES_PER_WORKER) gefüllt.
- Die Pakete werden nach Kosten absteigend nummeriert (pkg-0001.txt ist das teuerste). Die Worker nehmen
  sich der Reihe nach das nächste freie Paket, die teuren laufen so zuerst und die kleinen füllen am Ende auf.
- Mit "gruppe" werden nur Tests genommen, deren Pfad die Zeichenkette enthält (z.B. "drive/scanner").
- Gibt je Paket eine Zeile "Paket Tests Kosten" aus.
"""
import os
import sys

STARTUP_COST = 3_000_000
PACKAGES_PER_WORKER = 6
IMAGE_EXTENSIONS = (".d64", ".d71", ".d81", ".g64", ".crt", ".tap", ".t64")


def main():
    if len(sys.argv) < 4:
        print(__doc__)
        return 2

    testlist, workers, outdir = sys.argv[1], max(1, int(sys.argv[2])), sys.argv[3]
    groups = sys.argv[4:]

    by_path = {}
    order = []
    with open(testlist, encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")
            if not line.strip() or line.startswith("#"):
                continue
            fields = line.split(",")
            if len(fields) < 4:
                print("Fehler: Zeile mit zu wenigen Feldern: " + line, file=sys.stderr)
                return 1
            path = fields[0]
            if groups and not any(g in path for g in groups):
                continue
            try:
                cost = int(fields[3])
            except ValueError:
                cost = 0
            # Eingebundene Images als normalisierter Pfad (relativ zum testbench Verzeichnis)
            images = [os.path.normpath(os.path.join(path, opt.split(":", 1)[1]))
                      for opt in (o.strip() for o in fields[4:]) if opt.startswith("mount") and ":" in opt]
            # Ein per Autostart gestartetes Image wird ebenfalls eingebunden
            if fields[1].lower().endswith(IMAGE_EXTENSIONS):
                images.append(os.path.normpath(os.path.join(path, fields[1])))
            resources = list(images)
            # Screenshot Tests schreiben <pfad>/.testbench/<name>-emu64.png (name = Programm, sonst das Steckmodul,
            # siehe emu64_run_screenshot). Tests mit gleichem Namen dürfen nicht parallel laufen.
            if fields[2] == "screenshot":
                shot = fields[1]
                if not shot:
                    crts = [o.strip().split(":", 1)[1] for o in fields[4:] if o.strip().startswith("mountcrt:")]
                    shot = crts[0] if crts else ""
                resources.append("screenshot:" + os.path.normpath(os.path.join(path, ".testbench", shot)))
            if path not in by_path:
                by_path[path] = []
                order.append(path)
            by_path[path].append((line, cost + STARTUP_COST, resources, bool(images)))

    if not by_path:
        print("Fehler: Keine Tests gefunden (Filter: %s)" % " ".join(groups), file=sys.stderr)
        return 1

    total = sum(t[1] for tests in by_path.values() for t in tests)
    target = max(1, total // (workers * PACKAGES_PER_WORKER))

    # Tests mit gemeinsamen Dateien (Images, Screenshots): Gruppen bilden (Union-Find, ein Test mit mehreren
    # Dateien verbindet deren Gruppen)
    parent = {}

    def find(x):
        while parent.setdefault(x, x) != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    for path in order:
        for _, _, res, _ in by_path[path]:
            for r in res[1:]:
                parent[find(r)] = find(res[0])

    groups = {}
    for path in order:
        for line, cost, res, has_image in by_path[path]:
            if res:
                g = groups.setdefault(find(res[0]), {"tests": [], "image": False})
                g["tests"].append((line, cost))
                g["image"] = g["image"] or has_image

    # Gruppen mit Image oder mit mehreren Tests werden je ein Paket, ein einzelner Test ohne Image ist frei
    packages = []
    single = set()
    for g in groups.values():
        if g["image"] or len(g["tests"]) > 1:
            packages.append(g["tests"])
        else:
            single.add(g["tests"][0][0])

    # Alle freien Tests je Verzeichnis in Pakete bis zur Zielgröße zerlegen
    for path in order:
        tests = [(line, cost) for line, cost, res, _ in by_path[path] if not res or line in single]
        current, current_cost = [], 0
        for line, cost in tests:
            if current and current_cost + cost > target:
                packages.append(current)
                current, current_cost = [], 0
            current.append((line, cost))
            current_cost += cost
        if current:
            packages.append(current)

    # Teuerste Pakete zuerst (bei gleichen Kosten bleibt die Reihenfolge der Testliste)
    packages.sort(key=lambda p: -sum(c for _, c in p))

    os.makedirs(outdir, exist_ok=True)
    for i, pkg in enumerate(packages, 1):
        with open(os.path.join(outdir, "pkg-%04d.txt" % i), "w", encoding="utf-8") as f:
            f.write("# generated by split_testlist.py, do not edit.\n")
            for line, _ in pkg:
                f.write(line + "\n")
        print("%d %d %d" % (i, len(pkg), sum(c for _, c in pkg)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
