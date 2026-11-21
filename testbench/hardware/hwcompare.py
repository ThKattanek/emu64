#!/usr/bin/env python3
# Vergleicht Grabber-Aufnahmen des echten C64 mit Emu64-Screenshots auf C64-Pixelebene.
#
# Idee: Der Grabber (stk1160, PAL 720x576) tastet mit 13,5 MHz ab, ein C64-Pixel sind etwa 1,71 Abtastwerte,
# jede C64-Rasterzeile erscheint zweimal. Einzelne Pixel sind dadurch unscharf und nicht direkt ablesbar.
# Statt die Aufnahme zu "schärfen", wird jedes Emu64-Bild (eine Hypothese) genauso unscharf gemacht wie der
# Grabber (Gauß-Unschärfe, Helligkeit/Kontrast linear angepasst) und mit der Aufnahme verglichen. Die Hypothese
# mit der kleineren Abweichung (RMSE der Helligkeit) gewinnt. Unterscheiden sich zwei Hypothesen nur in einem
# Pixel, entscheidet das zuverlässig, solange die beiden Farben verschieden hell sind.
#
# Koordinaten sind immer die der Emu64/VICE-Testbench-Screenshots (384x272, Textfenster ab x=32, y=35).
#
# Befehle:
#   fit     <aufnahme> <emu.png> [...]           Abbildung Aufnahme -> C64-Pixel ermitteln (schreibt geometry.txt)
#   compare [--region N=y1:y2,x1:x2] FALL [...]  Hypothesen vergleichen, FALL = <aufnahme>:<name>=<png>[,<name>=<png>...]
#   profile <aufnahme> <emu.png> --row Y --x X1:X2   Helligkeitsverlauf einer Zeile: Aufnahme gegen Modell
#   view    <aufnahme> <emu.png> [...] --region y1:y2,x1:x2 --out bild.png   Ausschnitt zum Ansehen
#
# <aufnahme> ist ein Verzeichnis von hw_run.sh (frame_*.png). Alle Bilder darin werden gemittelt.

import argparse
import glob
import os
import sys

import numpy as np
from PIL import Image

SUB = 4                      # Unterabtastung pro C64-Pixel beim Vergleich
DEFAULT_GEOM = (1.714, 31.0)  # x_aufnahme = ax * x_c64 + bx (stk1160, PAL)
FIT_REGION = (slice(20, 250), slice(20, 360))   # für Unschärfe und Helligkeit: fast das ganze Bild


def luma(rgb):
    return rgb[..., 0] * 0.299 + rgb[..., 1] * 0.587 + rgb[..., 2] * 0.114


def load_rgb(path):
    return np.asarray(Image.open(path).convert('RGB'), dtype=np.float64)


def load_capture(capdir):
    files = sorted(glob.glob(os.path.join(capdir, 'frame_*.png')))
    if not files:
        sys.exit('Keine Aufnahmen (frame_*.png) in %s' % capdir)
    return np.mean([luma(load_rgb(f)) for f in files], axis=0)


def read_geometry(capdir):
    p = os.path.join(capdir, 'geometry.txt')
    if os.path.exists(p):
        ax, bx, cy = open(p).read().split()
        return float(ax), float(bx), int(cy)
    return DEFAULT_GEOM[0], DEFAULT_GEOM[1], None


def resample(cap, ax, bx, cy, sub=1):
    """Aufnahme auf das C64-Raster (272 Zeilen, 384*sub Spalten) umrechnen."""
    h, w = cap.shape
    xs = ax * (np.arange(384 * sub) / sub) + bx
    xs = np.clip(xs, 0, w - 1.001)
    x0 = np.floor(xs).astype(int)
    fx = xs - x0
    ys = np.clip(2 * np.arange(272) + cy, 0, h - 1)
    rows = cap[ys]
    return rows[:, x0] * (1 - fx) + rows[:, x0 + 1] * fx


def blur(img, sigma):
    up = np.repeat(img, SUB, axis=1)
    k = np.arange(-12, 13)
    k = np.exp(-0.5 * (k / (sigma * SUB)) ** 2)
    k /= k.sum()
    return np.array([np.convolve(r, k, mode='same') for r in up])


def corr(a, b):
    a = a - a.mean()
    b = b - b.mean()
    return (a * b).sum() / np.sqrt((a * a).sum() * (b * b).sum())


def fit_model(cap, ref_luma, ax, bx, cys):
    """Bestes cy, Unschärfe, Verschiebung und Helligkeit für ein Modellbild. Gibt (rmse, modell, aufnahme) zurück."""
    fy, fx = FIT_REGION
    fit = (fy, slice(fx.start * SUB, fx.stop * SUB))
    best = None
    for cy in cys:
        g = resample(cap, ax, bx, cy, SUB)
        for sigma in (0.4, 0.5, 0.6, 0.7, 0.8, 0.9):
            for shift in (-2, -1, 0, 1):
                m = np.roll(blur(ref_luma, sigma), shift, axis=1)
                a = np.vstack([m[fit].ravel(), np.ones(m[fit].size)]).T
                coef = np.linalg.lstsq(a, g[fit].ravel(), rcond=None)[0]
                e = ((a @ coef - g[fit].ravel()) ** 2).mean()
                if best is None or e < best[0]:
                    best = (e, m * coef[0] + coef[1], g, dict(cy=cy, sigma=sigma, shift=shift))
    return best


def parse_region(s):
    ys, xs = s.split(',')
    y1, y2 = (int(v) for v in ys.split(':'))
    x1, x2 = (int(v) for v in xs.split(':'))
    return y1, y2, x1, x2


def fit_geometry(capdir, images, verbose=True):
    """Abbildung Aufnahme -> C64-Pixel suchen, als geometry.txt ablegen. Gibt (korrelation, ax, bx, cy) zurück."""
    cap = load_capture(capdir)
    best_all = None
    for path in images:
        ref = luma(load_rgb(path))
        best = None
        for cy in range(-20, 30):
            for bx in np.arange(0, 70, 1.0):
                c = corr(resample(cap, DEFAULT_GEOM[0], bx, cy)[30:250, 20:360], ref[30:250, 20:360])
                if best is None or c > best[0]:
                    best = (c, DEFAULT_GEOM[0], bx, cy)
        _, ax0, bx0, cy0 = best
        for ax in np.arange(1.690, 1.740, 0.002):
            for bx in np.arange(bx0 - 3, bx0 + 3, 0.25):
                for cy in (cy0 - 1, cy0, cy0 + 1):
                    c = corr(resample(cap, ax, bx, cy)[30:250, 20:360], ref[30:250, 20:360])
                    if c > best[0]:
                        best = (c, ax, bx, cy)
        if verbose:
            print('%s: Korrelation %.4f  ax=%.4f bx=%.2f cy=%d' % (path, *best))
        if best_all is None or best[0] > best_all[0]:
            best_all = best
    with open(os.path.join(capdir, 'geometry.txt'), 'w') as f:
        f.write('%.4f %.2f %d\n' % best_all[1:])
    return best_all


def region_rmse(capdir, image, regions):
    """RMSE einer Hypothese (Emu64-Bild) gegen eine Aufnahme je Bereich. regions: [(name, 'y1:y2,x1:x2')]"""
    cap = load_capture(capdir)
    ax, bx, cy = read_geometry(capdir)
    cys = range(cy - 1, cy + 2) if cy is not None else range(3, 7)
    _, m, g, _ = fit_model(cap, luma(load_rgb(image)), ax, bx, cys)
    out = {}
    for name, spec in regions:
        y1, y2, x1, x2 = parse_region(spec)
        w = (slice(y1, y2), slice(x1 * SUB, x2 * SUB))
        out[name] = float(np.sqrt(((m[w] - g[w]) ** 2).mean()))
    return out


def cmd_fit(args):
    best = fit_geometry(args.capture, args.images)
    print('geometry.txt geschrieben (beste Korrelation %.4f)' % best[0])


def cmd_compare(args):
    regions = []
    for r in args.region or []:
        name, spec = r.split('=', 1)
        y1, y2, x1, x2 = parse_region(spec)
        regions.append((name, (slice(y1, y2), slice(x1 * SUB, x2 * SUB))))
    if not regions:
        regions = [('Gesamtbild', (slice(20, 250), slice(20 * SUB, 360 * SUB)))]

    totals = {}
    for case in args.cases:
        capdir, hyps = case.split(':', 1)
        cap = load_capture(capdir)
        ax, bx, cy = read_geometry(capdir)
        cys = range(cy - 1, cy + 2) if cy is not None else range(3, 7)
        line = [os.path.basename(os.path.normpath(capdir))]
        for h in hyps.split(','):
            label, path = h.split('=', 1)
            _, m, g, _ = fit_model(cap, luma(load_rgb(path)), ax, bx, cys)
            vals = []
            for rname, w in regions:
                e = np.sqrt(((m[w] - g[w]) ** 2).mean())
                totals.setdefault(label, {}).setdefault(rname, []).append(e)
                vals.append('%.1f' % e)
            line.append('%s: %s' % (label, ' '.join(vals)))
        print('  '.join(line))
    print('--- RMSE je Bereich (%s), über alle Fälle gemittelt, kleiner = näher an der Hardware' %
          ', '.join(r[0] for r in regions))
    for label, per in totals.items():
        print('%-20s %s' % (label, '  '.join('%s=%.2f' % (r, np.sqrt(np.mean(np.square(v)))) for r, v in per.items())))


def cmd_profile(args):
    cap = load_capture(args.capture)
    ax, bx, cy = read_geometry(args.capture)
    img = load_rgb(args.image)
    _, m, g, p = fit_model(cap, luma(img), ax, bx, range(cy - 1, cy + 2) if cy is not None else range(3, 7))
    x1, x2 = (int(v) for v in args.x.split(':'))
    y = args.row
    xs = range(x1, x2)
    print('Unschärfe sigma=%.2f px, cy=%d' % (p['sigma'], p['cy']))
    print('x        ' + ' '.join('%4d' % x for x in xs))
    print('Aufnahme ' + ' '.join('%4.0f' % g[y, x * SUB + SUB // 2] for x in xs))
    print('Modell   ' + ' '.join('%4.0f' % m[y, x * SUB + SUB // 2] for x in xs))
    print('Emu64 R  ' + ' '.join('%4d' % img[y, x, 0] for x in xs))


def cmd_view(args):
    cap = load_capture(args.capture)
    ax, bx, cy = read_geometry(args.capture)
    y1, y2, x1, x2 = parse_region(args.region)
    if cy is None:
        cy = 5
    g = resample(cap, ax, bx, cy)
    g = np.clip((g - g.min()) / (np.percentile(g, 99.5) - g.min()), 0, 1) * 255
    tiles = [Image.fromarray(g[y1:y2, x1:x2].astype(np.uint8)).convert('RGB')]
    tiles += [Image.fromarray(load_rgb(p)[y1:y2, x1:x2].astype(np.uint8)) for p in args.images]
    w, h = x2 - x1, y2 - y1
    out = Image.new('RGB', (w, (h + 2) * len(tiles)), (255, 0, 255))
    for i, t in enumerate(tiles):
        out.paste(t, (0, i * (h + 2)))
    out.resize((w * args.zoom, out.height * args.zoom), Image.NEAREST).save(args.out)
    print('%s: oben Aufnahme, darunter %s' % (args.out, ', '.join(args.images)))


def main():
    ap = argparse.ArgumentParser(description='Grabber-Aufnahmen des echten C64 mit Emu64-Screenshots vergleichen')
    sp = ap.add_subparsers(dest='cmd', required=True)

    p = sp.add_parser('fit', help='Abbildung Aufnahme -> C64-Pixel ermitteln')
    p.add_argument('capture')
    p.add_argument('images', nargs='+')
    p.set_defaults(func=cmd_fit)

    p = sp.add_parser('compare', help='Hypothesen gegen Aufnahmen vergleichen')
    p.add_argument('--region', action='append', help='NAME=y1:y2,x1:x2 (mehrfach möglich)')
    p.add_argument('cases', nargs='+', help='<aufnahme>:<name>=<png>[,<name>=<png>...]')
    p.set_defaults(func=cmd_compare)

    p = sp.add_parser('profile', help='Helligkeitsverlauf einer Zeile')
    p.add_argument('capture')
    p.add_argument('image')
    p.add_argument('--row', type=int, required=True)
    p.add_argument('--x', required=True, help='x1:x2')
    p.set_defaults(func=cmd_profile)

    p = sp.add_parser('view', help='Ausschnitt Aufnahme und Modelle als Bild')
    p.add_argument('capture')
    p.add_argument('images', nargs='+')
    p.add_argument('--region', required=True, help='y1:y2,x1:x2')
    p.add_argument('--out', required=True)
    p.add_argument('--zoom', type=int, default=10)
    p.set_defaults(func=cmd_view)

    args = ap.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
