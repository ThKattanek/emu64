#!/usr/bin/env python3
# Liest und setzt Regler eines Video4Linux-Grabbers (Helligkeit, Kontrast, Sättigung, Farbton) und die
# Fernsehnorm per ioctl, ohne v4l2-ctl. Ausgabe von "get" ist nur der Zahlenwert, damit Skripte ihn sich merken
# können.
#
#   grabber_ctrl.py /dev/video4 get contrast
#   grabber_ctrl.py /dev/video4 set contrast 48
#   grabber_ctrl.py /dev/video4 list
#   grabber_ctrl.py /dev/video4 std PAL     Fernsehnorm setzen (Namen wie bei "std" ohne Namen aufgelistet)
#
# Die Norm muss gesetzt sein, bevor ffmpeg/ffplay das Gerät öffnet: Nach einem Neustart steht z.B. der stk1160
# auf NTSC (720x480). ffmpegs eigene Option -standard stellt die Norm erst nach dem Lesen der Bildgröße um, die
# Aufnahme passt dann nicht (graues Bild, falsche Größe oder ffmpeg hängt).

import fcntl
import os
import struct
import sys

VIDIOC_G_CTRL = 0xC008561B
VIDIOC_S_CTRL = 0xC008561C
VIDIOC_G_STD = 0x80085617
VIDIOC_S_STD = 0x40085618
VIDIOC_ENUMSTD = 0xC0485619

# struct v4l2_standard: index, id (64 Bit), name[24], frameperiod, framelines, reserved[4] (72 Byte)
V4L2_STANDARD = '=I4xQ24sIII16x4x'

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


def enum_std(fd):
    result = []
    index = 0
    while True:
        try:
            data = fcntl.ioctl(fd, VIDIOC_ENUMSTD, struct.pack(V4L2_STANDARD, index, 0, b'', 0, 0, 0))
        except OSError:
            return result
        _, std_id, name, _, _, _ = struct.unpack(V4L2_STANDARD, data)
        result.append((name.split(b'\0')[0].decode(), std_id))
        index += 1


def set_std(fd, name):
    standards = enum_std(fd)
    ids = dict((n.upper(), i) for n, i in standards)
    if name.upper() not in ids:
        sys.exit('Unbekannte Fernsehnorm: %s (bekannt: %s)' % (name, ', '.join(n for n, _ in standards)))
    try:
        fcntl.ioctl(fd, VIDIOC_S_STD, struct.pack('Q', ids[name.upper()]))
    except OSError as e:
        sys.exit('Fernsehnorm %s nicht setzbar: %s (Grabber von einem anderen Programm belegt?)' % (name, e.strerror))
    got = struct.unpack('Q', fcntl.ioctl(fd, VIDIOC_G_STD, struct.pack('Q', 0)))[0]
    if got & ids[name.upper()] != got:
        sys.exit('Fernsehnorm: gesetzt %s, Gerät meldet 0x%x' % (name, got))


def main():
    if len(sys.argv) < 3 or sys.argv[2] not in ('get', 'set', 'list', 'std'):
        print('Aufruf: grabber_ctrl.py <gerät> get <regler> | set <regler> <wert> | list | std [<norm>]',
              file=sys.stderr)
        sys.exit(2)
    dev, cmd = sys.argv[1], sys.argv[2]
    fd = os.open(dev, os.O_RDWR)
    try:
        if cmd == 'std':
            if len(sys.argv) > 3:
                set_std(fd, sys.argv[3])
            else:
                for name, std_id in enum_std(fd):
                    print('%-10s 0x%x' % (name, std_id))
            return
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
