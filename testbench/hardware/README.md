# Messungen am echten C64

[Deutsch](#deutsch) | [English](#english)

---

## Deutsch

Mit diesen Skripten wird ein Testprogramm auf einem echten C64 gestartet, das Bild mit einem Video-Grabber aufgenommen und auf C64-Pixelebene mit Emu64 verglichen. So lässt sich klären, was die Hardware wirklich macht, wenn Referenzbild und VICE sich widersprechen oder beide unsicher sind (in der Testliste oft als `unsafe reference` vermerkt).

Entstanden bei #446 (VICII/vicii_timing): Gemessen an echten 6569 und 8565 lagen dort weder die Referenzbilder noch VICE ganz richtig. Die Ergebnisse aller Messungen stehen in [`MESSUNGEN.md`](MESSUNGEN.md).

### Aufbau

- C64 mit 1541 Ultimate II+ im Netzwerk (REST-API, ohne Passwort). Das Programm wird per `runners:run_prg` gestartet, an der Ultimate wird nichts umkonfiguriert.
- Video-Grabber am C64 (getestet: stk1160 / EasyCAP, PAL 720x576, Composite). S-Video ist schärfer, falls vorhanden.
- `python3` mit `numpy` und `Pillow`, `ffmpeg`, `curl`.
- Emu64 gebaut und installiert (`EMU64_BIN`, wie bei der Testbench).

In `testbench/config.local.sh` (wird nicht eingecheckt) eintragen:

```bash
U2_HOST="192.168.1.64"    # Adresse der Ultimate
```

Der Grabber wird automatisch über seinen Namen gefunden (Standard `stk1160`), die Gerätenummer `/dev/videoN` ändert sich beim Umstecken. Für einen anderen Grabber `VIDEO_NAME="<name>"` eintragen (die Namen stehen in `/sys/class/video4linux/video*/name`) oder das Gerät fest mit `VIDEO_DEV="/dev/videoN"` vorgeben. Wird nichts gefunden, brechen die Skripte ab und listen die vorhandenen Video-Geräte auf. Alle Skripte stellen den Grabber auf die Fernsehnorm PAL (`VIDEO_STANDARD`, Standard `PAL`; mögliche Namen mit `./grabber_ctrl.py /dev/videoN std`). Nach einem Neustart steht der stk1160 auf NTSC, ohne das gäbe es ein graues Bild in falscher Größe.

### Skripte

| Skript | Zweck |
|---|---|
| `hw_run.sh <prg> <name>` | Programm auf dem C64 starten, warten (`SETTLE`, 7 s), `FRAMES` (20) Bilder aufnehmen nach `captures/<name>/` |
| `hw_mount.sh <image>` | Disk-Image vom PC in die Ultimate einlegen (`-d a\|b` Laufwerk, Standard A; `-m ro\|rw\|ul`, Standard schreibgeschützt; `-r` Laufwerk danach zurücksetzen), `--eject` auswerfen, `--status` Laufwerke anzeigen. Typ (d64, g64, d71, g71, d81) aus der Dateiendung oder per `-t`. Ohne `-a` wird der C64 nicht zurückgesetzt. `-a` startet danach automatisch mit `LOAD"*",8,1` und `RUN` (`-f name` für eine bestimmte Datei, Gerätenummer kommt von der Ultimate). Das geht auch, wenn ein Cartridge-Menü eingestellt ist |
| `emu_screenshot.sh <prg> <png> <0\|1>` | Gleiches Programm in Emu64 rendern (0 = 6569, 1 = 8565), Einstellungen wie die VICE-Testbench, eigenes HOME je Aufruf |
| `patch_prg.py` | Variante eines Testprogramms mit einem geänderten Byte erzeugen (z.B. Bitmap-Füllmuster) |
| `pattern_series.sh` | Messreihe: Varianten erzeugen, auf der Hardware aufnehmen, mit Emu64 rendern |
| `hw_suite.sh <gerät>` | Fester Messsatz pro Gerät (`suite_tests.txt`, ca. 2,5 Minuten), erkennt den VIC-Typ selbst. Ablage unter `captures/suite/<gerät>/` |
| `hw_suite_eval.py` | Wertet die Messsätze aller Geräte aus: rendert die Tests mit einem oder mehreren Emu64-Ständen (`--emu name=binary`) und vergleicht sie mit Referenz und Hardware an den strittigen Stellen aus `suite_regions.txt` |
| `hwcompare.py` | Auswertung: `fit`, `compare`, `profile`, `view` |
| `grabber_ctrl.py` | Helligkeit/Kontrast des Grabbers lesen und setzen |
| `hw_view.sh` | Grabber-Bild live in einem Fenster zeigen (ffplay). Standard: nur das sichtbare C64-Bild wie in den Testbench-Screenshots (384x272 mit Rahmen), `--full` ganzes Grabberbild, `-z 2` doppelte Größe, `-f` Vollbild, `--pixel` ohne Glättung, Deinterlacer bwdif mit 50 Bildern/s (`-d yadif\|field\|none`). Solange es offen ist, ist der Grabber für Aufnahmen belegt. |

### Geräte messen

Jeden C64 vor der Messung mindestens 15 Minuten warmlaufen lassen: Kalt liegen Kanten beim Umschalten des Videomodus um etwa ein Pixel anders (gemessen bei #446 an einem 8565R2, ab 15 Minuten stabil). Den Gerätenamen aus Nummer und VIC bilden, die Bestückung steht danach in `info.txt` im Messsatz.

```bash
./hw_suite.sh C64-06_6569R3                         # ca. 2,5 Minuten, VIC-Typ wird erkannt
./hw_suite_eval.py --all                            # alle Geräte mit dem aktuellen Stand (EMU64_BIN)
./hw_suite_eval.py --emu alt=/pfad/a/emu64 --emu neu=/pfad/b/emu64 C64-06_6569R3 C64-02_8565R2_t45
```

Neue strittige Stellen kommen in `suite_regions.txt`, neue Testprogramme in `suite_tests.txt`. Bereits gemessene Geräte werden bei der nächsten Auswertung automatisch mit verglichen (für neue Testprogramme muss das Gerät erneut gemessen werden).

### Wie verglichen wird

Der Grabber tastet mit 13,5 MHz ab, ein C64-Pixel sind etwa 1,71 Abtastwerte, und jede Rasterzeile erscheint zweimal. Einzelne Pixel sind dadurch unscharf. `hwcompare.py` schärft die Aufnahme deshalb nicht nach, sondern macht umgekehrt jedes Emu64-Bild (eine Hypothese) so unscharf wie den Grabber und passt Helligkeit und Kontrast an. Danach wird die Abweichung (RMSE der Helligkeit) in einem Bildbereich berechnet. Die Hypothese mit der kleineren Abweichung entspricht der Hardware.

- Unterscheiden sich zwei Hypothesen nur in einem Pixel, entscheidet das zuverlässig, solange die beiden Farben verschieden hell sind. Weiß neben Weiß oder zwei gleich helle Farben bleiben unentschieden.
- Mehrere Bitmuster messen statt nur eines: Mit Varianten, die das letzte Pixel der alten Zelle sowie Pixel 0 und 1 der neuen Zelle unabhängig ändern, trennt man Regeln, die bei einem einzelnen Muster gleich aussehen.
- Beim Kontrast-Standardwert 64 übersteuert Weiß. `hw_run.sh` setzt ihn für die Aufnahme auf `MEASURE_CONTRAST` (48) und stellt ihn danach zurück.
- Die Abbildung Aufnahme → C64-Pixel ermittelt `hwcompare.py fit` (stk1160: `x = 1,714 * x_c64 + 31`, `y = 2 * y_c64 + 4 oder 5`) und legt sie als `geometry.txt` ab. Ohne die Datei gilt dieser Standard und das `y` wird gesucht.
- Koordinaten sind die der Testbench-Screenshots (384x272, Textfenster beginnt bei x=32, y=35).

### Ablauf an einem Beispiel (#446)

```bash
cd testbench/hardware
T=<vice>/testprogs/VICII/vicii_timing

# Welcher VIC steckt im C64? Aufnehmen, Abbildung ermitteln und im Bereich der Moduswechsel gegen beide
# Modelle vergleichen. Dort liegen 6569 und 8565 eine Zelle auseinander (gemessen: 8,6 gegen 17,6).
./hw_run.sh $T/vicii_reg_timing.prg id
./emu_screenshot.sh $T/vicii_reg_timing.prg /tmp/m0.png 0
./emu_screenshot.sh $T/vicii_reg_timing.prg /tmp/m1.png 1
./hwcompare.py fit captures/id /tmp/m0.png /tmp/m1.png
./hwcompare.py compare --region Modusblock=160:198,60:140 captures/id:6569=/tmp/m0.png,8565=/tmp/m1.png

# Messreihe: Bitmap-Füllmuster (Byte 8565, Original $5a) variieren, aufnehmen und rendern
./pattern_series.sh -p $T/vicii_reg_timing.prg -o 8565 -e 5a -v "00 40 80 c0 5a a5 ff" -n vt6569 -m 0 -l neu

# Zweite Hypothese rendern (andere Binary oder per Umgebungsvariable geschaltete Variante)
EMU64_BIN=/pfad/alte/emu64 ./pattern_series.sh -p $T/vicii_reg_timing.prg -o 8565 -e 5a \
    -v "00 40 80 c0 5a a5 ff" -n vt6569 -m 0 -l alt --no-hw

# Vergleichen, je Zeile der Umschaltkante
C=captures/vt6569
./hwcompare.py compare --region BMM=172:179,100:124 --region ECM=180:187,100:124 --region E+B=188:195,100:124 \
    $(for v in 00 40 80 c0 5a a5 ff; do echo "$C/$v:alt=$C/$v/alt-m0.png,neu=$C/$v/neu-m0.png"; done)

# Einzelne Zeile ansehen
./hwcompare.py profile $C/a5 $C/a5/neu-m0.png --row 188 --x 100:124
./hwcompare.py view $C/a5 $C/a5/alt-m0.png $C/a5/neu-m0.png --region 160:198,60:140 --out /tmp/a5.png
```

Für Varianten im Emu64-Code hat sich bewährt, sie vorübergehend per `getenv()` schaltbar zu machen, eine Binary zu bauen und jede Variante unter eigenem Namen (`-l`) zu rendern. Die Schalter danach wieder entfernen.

### Grenzen

- Gemessen werden einzelne Geräte. Chips streuen (gpz führt z.B. eine eigene Variante „8565early“), das Ergebnis gilt zuerst für das gemessene Exemplar.
- Farbe ist über Composite schlechter aufgelöst als Helligkeit. Verglichen wird deshalb nur die Helligkeit.
- Die Ultimate emuliert die 1541 nicht zyklusgenau. Für Floppy-Timing ist eine echte 1541 die Referenz.

---

## English

These scripts run a test program on a real C64, capture the picture with a video grabber and compare it with Emu64 at C64 pixel level. They help to decide what the hardware really does when reference screenshot and VICE disagree or are uncertain (often marked `unsafe reference` in the test list).

Setup: C64 with 1541 Ultimate II+ (REST API, `runners:run_prg`), a video grabber (tested: stk1160, PAL 720x576), `python3` with `numpy` and `Pillow`, `ffmpeg`, `curl`. Put `U2_HOST` into `testbench/config.local.sh`. The grabber is found by its name (default `stk1160`, override with `VIDEO_NAME` or `VIDEO_DEV`).

`hwcompare.py` does not sharpen the capture. Instead, every Emu64 screenshot (a hypothesis) is blurred like the grabber and adjusted in brightness and contrast, then the luma RMSE in a region is computed. The hypothesis with the smaller error matches the hardware. A single-pixel difference is decided reliably as long as the two colours differ in brightness. Measure several bitmap patterns instead of one to separate rules that look identical for a single pattern. See the German section for a complete example.
