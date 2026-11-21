#!/usr/bin/env python3
# Erzeugt eine Variante eines Testprogramms, in der ein Byte ersetzt ist (z.B. das Füllmuster der Bitmap).
# Der Offset zählt ab Dateianfang (inklusive der 2 Bytes Ladeadresse). Mit --expect wird geprüft, dass an der
# Stelle der erwartete alte Wert steht, damit man nicht versehentlich Code überschreibt.
#
#   patch_prg.py vicii_reg_timing.prg bm_80.prg 8565 0x80 --expect 0x5a
#
# Den Offset findet man am einfachsten mit "cmp -l a.prg b.prg" zwischen zwei Varianten desselben Tests
# (cmp zählt ab 1, hier wird ab 0 gezählt).

import argparse


def num(s):
    return int(s, 0)


def main():
    ap = argparse.ArgumentParser(description='Ein Byte in einem PRG ersetzen')
    ap.add_argument('src')
    ap.add_argument('dst')
    ap.add_argument('offset', type=num)
    ap.add_argument('value', type=num)
    ap.add_argument('--expect', type=num, help='erwarteter alter Wert an der Stelle')
    a = ap.parse_args()

    data = bytearray(open(a.src, 'rb').read())
    if a.offset >= len(data):
        raise SystemExit('Offset %d liegt hinter dem Dateiende (%d Bytes)' % (a.offset, len(data)))
    if a.expect is not None and data[a.offset] != a.expect:
        raise SystemExit('An Offset %d steht $%02x, erwartet $%02x' % (a.offset, data[a.offset], a.expect))
    data[a.offset] = a.value & 0xFF
    open(a.dst, 'wb').write(data)


if __name__ == '__main__':
    main()
