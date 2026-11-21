#!/usr/bin/env python3
# Liest und setzt Regler eines Video4Linux-Grabbers (Helligkeit, Kontrast, Sättigung, Farbton) per ioctl,
# ohne v4l2-ctl. Ausgabe von "get" ist nur der Zahlenwert, damit Skripte ihn sich merken können.
#
#   grabber_ctrl.py /dev/video4 get contrast
#   grabber_ctrl.py /dev/video4 set contrast 48
#   grabber_ctrl.py /dev/video4 list

import fcntl
import os
import struct
import sys

VIDIOC_G_CTRL = 0xC008561B
VIDIOC_S_CTRL = 0xC008561C

CONTROLS = {
    'brightness': 0x00980900,
    'contrast':   0x00980901,
    'saturation': 0x00980902,
    'hue':        0x00980903,
}


def get_ctrl(fd, cid):
    data = fcntl.ioctl(fd, VIDIOC_G_CTRL, struct.pack('Ii', cid, 0))
    return struct.unpack('Ii', data)[1]


def set_ctrl(fd, cid, value):
    fcntl.ioctl(fd, VIDIOC_S_CTRL, struct.pack('Ii', cid, value))


def main():
    if len(sys.argv) < 3 or sys.argv[2] not in ('get', 'set', 'list'):
        print('Aufruf: grabber_ctrl.py <gerät> get <regler> | set <regler> <wert> | list', file=sys.stderr)
        sys.exit(2)
    dev, cmd = sys.argv[1], sys.argv[2]
    fd = os.open(dev, os.O_RDWR)
    try:
        if cmd == 'list':
            for name, cid in CONTROLS.items():
                try:
                    print('%-10s %d' % (name, get_ctrl(fd, cid)))
                except OSError:
                    print('%-10s nicht vorhanden' % name)
            return
        name = sys.argv[3]
        if name not in CONTROLS:
            sys.exit('Unbekannter Regler: %s (bekannt: %s)' % (name, ', '.join(CONTROLS)))
        if cmd == 'get':
            print(get_ctrl(fd, CONTROLS[name]))
        else:
            set_ctrl(fd, CONTROLS[name], int(sys.argv[4]))
            got = get_ctrl(fd, CONTROLS[name])
            if got != int(sys.argv[4]):
                sys.exit('%s: gesetzt %s, Gerät meldet %d' % (name, sys.argv[4], got))
    finally:
        os.close(fd)


if __name__ == '__main__':
    main()
