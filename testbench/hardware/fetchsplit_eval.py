#!/usr/bin/env python3
# Zeichenweise Auswertung von fetchsplit-Aufnahmen (VICII/split-tests/fetchsplit, #456).
#
#   ./fetchsplit_eval.py <aufnahme> <emu64.png>
#
# Die Glyphen 0-3 werden aus dem Emu64-Bild gelernt (der Test benutzt einen eigenen, gestauchten Zeichensatz;
# die Emu64-Ausgabe entspricht dem "langweiligen" Muster aus der readme des Tests). Für jede Ziffernzelle werden
# alle vier Glyphen in den Farben der Zelle als Hypothesen gegen die Aufnahme gerechnet (Unschärfe und Helligkeit
# wie hwcompare.py). Ausgewertet werden die Zeilen 1-6 jeder Zelle, Zeile 0 und 7 überdecken Rasterlinien des Tests.
# Ausgabe: alle Zellen, an denen die Hardware von Emu64 abweicht, und unsichere Zellen (Abstand < 4).
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hwcompare as hc  # noqa: E402

BLOCK = ["BBBBAAAAAABBBBBBAAAAAABBBBBBAAAAAABBBBBB",
         "0000111111000000222222000000333333000000",
         "0000111111000000222222000000333333000000",
         "1111222222111111333333111111000000111111",
         "1111222222111111333333111111000000111111",
         "2222333333222222000000222222111111222222",
         "2222333333222222000000222222111111222222",
         "0000000000000000000000000000000000000000"]
TEXT = {1 + 8 * b + i: BLOCK[i] for b in range(3) for i in range(8)}


def cell(img, r, c):
    y, x = 35 + 8 * r, 32 + 8 * c
    return img[y + 1:y + 7, x:x + 8]


def two_colors(z):
    vals, cnt = np.unique(np.round(z, 3), return_counts=True)
    if len(vals) != 2:
        return None, None
    return vals[np.argmin(cnt)], vals[np.argmax(cnt)]


def main():
    if len(sys.argv) != 3:
        sys.exit('Aufruf: %s <aufnahme> <emu64.png>' % sys.argv[0])
    capdir, emu = sys.argv[1], sys.argv[2]
    base = hc.luma(hc.load_rgb(emu))

    gly = {}
    for r, t in TEXT.items():
        for c, ch in enumerate(t):
            if ch in '0123' and ch not in gly:
                fg, _ = two_colors(cell(base, r, c))
                if fg is not None:
                    gly[ch] = (np.abs(cell(base, r, c) - fg) < 1e-3).astype(int)
    if len(gly) != 4:
        sys.exit('Glyphen nicht vollständig gefunden: %s' % sorted(gly))

    cap = hc.load_capture(capdir)
    ax, bx, cy = hc.read_geometry(capdir)
    cys = range(cy - 1, cy + 2) if cy is not None else range(3, 7)
    _, _, g, p = hc.fit_model(cap, base, ax, bx, cys)
    S = hc.SUB
    mb = np.roll(hc.blur(base, p['sigma']), p['shift'], axis=1)
    fy, fx = hc.FIT_REGION
    fit = (fy, slice(fx.start * S, fx.stop * S))
    a = np.vstack([mb[fit].ravel(), np.ones(mb[fit].size)]).T
    k0, k1 = np.linalg.lstsq(a, g[fit].ravel(), rcond=None)[0]

    diffs, weak = [], []
    for r, t in sorted(TEXT.items()):
        for c, ch in enumerate(t):
            if ch not in '0123':
                continue
            fg, bg = two_colors(cell(base, r, c))
            if fg is None:
                continue
            y, x = 35 + 8 * r, 32 + 8 * c
            errs = {}
            for h, m in gly.items():
                var = base[y + 1:y + 7].copy()
                var[:, x:x + 8] = np.where(m == 1, fg, bg)
                mod = np.roll(hc.blur(var, p['sigma']), p['shift'], axis=1) * k0 + k1
                w = (slice(None), slice(x * S, (x + 8) * S))
                errs[h] = float(np.sqrt(((mod[w] - g[y + 1:y + 7][w]) ** 2).mean()))
            best = min(errs, key=errs.get)
            margin = sorted(errs.values())[1] - errs[best]
            if best != ch:
                diffs.append((r, c, best))
            if margin < 4.0:
                weak.append((r, c, best, round(margin, 1)))

    rows = {}
    for r, c, h in diffs:
        rows.setdefault(r, []).append('S%d=%s' % (c, h))
    for r in sorted(rows):
        print('  Zeile %2d: %s' % (r, ' '.join(rows[r])))
    if not rows:
        print('  keine Abweichung zu Emu64')
    print('  unsichere Zellen:', weak if weak else 'keine')


if __name__ == '__main__':
    main()
