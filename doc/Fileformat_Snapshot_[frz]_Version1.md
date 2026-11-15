# Quick C64 Snapshot ("Freezen") – Dokumentation

Stand: Format-Version 1 (Issue #200)

Mit dem Quick C64 Snapshot wird der **komplette Zustand des emulierten C64** zu einem beliebigen Zeitpunkt in eine Datei (Endung `.frz`) gespeichert und später wieder geladen. Nach dem Laden läuft die Emulation zyklusgenau an exakt derselben Stelle weiter (gleiche CPU-Position, gleiche Rasterposition, gleiche Timerstände, Floppy mitten im Lesevorgang usw.).

Inhalt:

1. [Bedienung](#1-bedienung)
2. [Was wird gespeichert](#2-was-wird-gespeichert)
3. [Ablauf beim Speichern und Laden](#3-ablauf-beim-speichern-und-laden)
4. [Aufbau der Datei](#4-aufbau-der-datei)
5. [Die Chunks im Detail](#5-die-chunks-im-detail)
6. [Einschränkungen](#6-einschränkungen)
7. [Versionierung und Kompatibilität](#7-versionierung-und-kompatibilität)
8. [Hinweise für Entwickler](#8-hinweise-für-entwickler)
9. [Testen](#9-testen)

---

## 1. Bedienung

### Menü

*Tools / Quick C64 Snapshot*

| Eintrag | Funktion |
|---|---|
| Speichern … | Zustand in eine frei gewählte `.frz`-Datei speichern |
| Laden … | Zustand aus einer `.frz`-Datei laden |
| In Slot speichern → Slot 1 … 10 | Zustand in einen von 10 festen Slots speichern (belegte Slots zeigen Datum und Uhrzeit, so sieht man, was überschrieben wird) |
| Aus Slot laden → Slot 1 … 10 | Slot laden (leere Slots sind ausgegraut, belegte zeigen Datum und Uhrzeit) |

In beiden Slot-Menüs erscheint beim Überfahren eines belegten Eintrags mit der Maus das Vorschaubild des Slots (siehe „Vorschaubild“) als Tooltip.

### Hotkeys im C64-Fenster

| Taste | Funktion |
|---|---|
| `ALT+1` … `ALT+9`, `ALT+0` | Slot 1 … 9 bzw. Slot 10 laden (ein leerer Slot meldet „Slot n ist leer.“ in der Statuszeile) |
| `ALT+SHIFT+1` … `ALT+SHIFT+9`, `ALT+SHIFT+0` | Slot 1 … 9 bzw. Slot 10 speichern |

Speichern braucht bewusst zusätzlich Shift, damit ein Slot nicht versehentlich überschrieben wird.

(F5/F9 sind bewusst nicht belegt, da es echte C64-Tasten sind.)

### Speicherort

Die Slots liegen in `~/.config/emu64/snapshots/` (Windows: entsprechend im Emu64-Konfigurationsverzeichnis):

```
slot1.frz … slot10.frz
```

Dateien mit der Endung `.frz` können auch über *Datei öffnen* / `C64 Programme direkt laden`, per Drag&Drop oder per Kommandozeile geöffnet werden. Das führt zum Laden des Snapshots (kein Reset, kein Autostart).

### Vorschaubild

Zu jedem gespeicherten Snapshot wird ein gleichnamiges Bild im PNG-Format angelegt (`slot1.frz` → `slot1.png`, `mein_spiel.frz` → `mein_spiel.png`), im selben Verzeichnis. Es zeigt das aktuelle C64-Bild (wie ein normaler Screenshot, inklusive CRT-Einstellungen) im Moment des Speicherns. Das PNG ist nicht Teil der `.frz`-Datei und wird beim Laden nicht benötigt. Beim Verschieben oder Umbenennen eines Snapshots sollte das PNG mitgenommen werden. Im Slot-Menü wird es als Vorschau angezeigt. Schlägt das Speichern des PNG fehl, ist der Snapshot trotzdem gespeichert (Eintrag im Log).

### Fehlermeldungen

| Meldung | Ursache |
|---|---|
| Die Datei ist kein gültiger Emu64 Snapshot oder sie ist beschädigt. | Magic, Prüfsumme oder Chunk-Struktur falsch |
| Der Snapshot wurde mit einer neueren Emu64 Version erstellt. | Format-Version größer als die der laufenden Emu64 |
| Der Snapshot passt nicht zum aktuell eingestellten C64 Typ (PAL/NTSC). | VIC-Typ des Snapshots ≠ aktuell eingestellter VIC-Typ |
| Während einer Bandaufnahme kann kein Snapshot gespeichert werden. | Datasette steht auf Record |
| Der Snapshot konnte nicht vollständig übernommen werden. Der C64 wurde zurückgesetzt. | Ein Chunk war inhaltlich ungültig (siehe 3.2) |
| Die Datei konnte nicht gelesen oder geschrieben werden. | Dateisystemfehler |

---

## 2. Was wird gespeichert

| Komponente | Inhalt |
|---|---|
| C64 allgemein | Zykluszähler, Reset-Zustand, alle Leitungen (GAME, EXROM, RDY/BA, RAM-Leitungen), IEC-Bus, Takt-Akkumulator der Floppy, aktive SID-Emulation, Erweiterungsquelle (CRT/REU/GeoRAM) |
| CPU 6510 | Alle Register, Position im Microcode, Interruptzustand (IRQ/NMI inkl. Pipeline), JAM-Flag, CPU-Port inkl. Datasetten-Motor und Sense-Zustand |
| MMU | 64 KB RAM, 1 KB Farb-RAM. Die Speicherkonfiguration (Banking) wird nach dem Laden aus CPU-Port und Leitungen neu berechnet |
| CIA 1 + 2 | Alle Register, beide Timer inkl. interner Pipeline, Interruptlogik, TOD, Schieberegister, Portzustände |
| VIC-II | Alle Register, Raster-/Zyklusposition, Sprite- und Grafik-Pipeline, Border-Zustand, Farbregister-Historie, **beide Videopuffer** |
| SID | Eigene Engine **und** reSID jeweils für SID 1 und SID 2 (Register, Oszillator-, Hüllkurven- und Filterzustand) |
| Floppy 1541 (bis zu 4) | 2 KB RAM, 6502-CPU, beide VIAs, Rotation/Lese-/Schreibzustand, Kopfposition, Motor/Stepper, **kompletter GCR-Disk-Inhalt** |
| Datasette | Bandposition, Tastenzustand, Motorzustand, **kompletter TAP-Inhalt** |
| Cartridge | Typ, alle Register, ROM-Bänke (bei EasyFlash inkl. geschriebenem Flash-Inhalt), Flash-Zustandsautomat, EasyFlash-RAM, Action-Replay-RAM, LEDs |
| REU | Register, Größe, Transferzustand, **RAM-Inhalt** |
| GeoRAM | Register, Größe, **RAM-Inhalt** |

Der Disk-/Band-/Modul-Inhalt wird **eingebettet**. Ein Snapshot ist also unabhängig davon, ob das ursprüngliche D64/TAP/CRT noch existiert.

### Nicht gespeichert

* ROM-Inhalte (Kernal, Basic, Zeichensatz, 1541-DOS): kommen aus dem aktiven ROM-Set.
* Konfiguration (Setup-Einstellungen, Tastenbelegung, Joystick-Mapping, Audio-/Videoeinstellungen, Emulationsgeschwindigkeit, C64-Takt).
* Eingabegeräte: gedrückte Tasten, Joystick- und Mausstellung. Nach dem Laden gilt „nichts gedrückt“.
* Laufende Autostart-/Kommandozeilen-Eingaben (werden beim Laden verworfen).
* Debugger-Zustand (Breakpoints, Historie, Einzelschritt).
* Audio-Ausgabepuffer und Floppy-Sounds.
* WAV-Bänder (siehe Einschränkungen).

---

## 3. Ablauf beim Speichern und Laden

### 3.1 Anhalten der Emulation

Die Emulation läuft im SDL-Audio-Callback (geschützt durch den Mutex `mutex1`), im Warp-Modus dagegen in einem eigenen Thread ohne Mutex. Deshalb gilt für Speichern **und** Laden:

1. War der Warp-Modus aktiv, wird er kurz beendet (`EnableWarpMode(false)`).
2. `mutex1` wird gesperrt, der Audio-Callback und damit die Emulation stehen.
3. Zustand wird gelesen bzw. geschrieben.
4. `mutex1` wird freigegeben, der Warp-Modus wird ggf. wieder eingeschaltet.

Der Snapshot wird immer **zwischen zwei Emulationszyklen** erzeugt. Dadurch ist keine Zwischenstufe innerhalb eines Zyklus nötig.

### 3.2 Speichern (`C64Class::SaveSnapshot`)

1. Header schreiben.
2. Chunks in fester Reihenfolge schreiben (siehe Abschnitt 5).
3. CRC32 über alles bisher Geschriebene anhängen.
4. Die Datei wird im Hauptfenster zuerst als `<name>.tmp` geschrieben und erst bei Erfolg auf den Zielnamen umbenannt. Ein Fehler zerstört also keinen vorhandenen Snapshot.

### 3.3 Laden (`C64Class::LoadSnapshot`)

Das Laden besteht aus zwei Stufen. Nur die erste kann ohne Folgen abbrechen.

**Stufe 1: Prüfung (der laufende C64 wird nicht verändert)**

1. Datei komplett in den Speicher lesen (Obergrenze 512 MiB).
2. Magic prüfen, CRC32 prüfen, Format-Version prüfen.
3. Chunk `MAIN` lesen: VIC-Typ (PAL/NTSC) gegen die aktuelle Einstellung prüfen.
4. Prüfen, dass alle Pflicht-Chunks vorhanden sind (`CPU `, `MMU `, `CIA1`, `CIA2`, `VIC `, `SID1`, `SID2`, `RSD1`, `RSD2`, `TAPE`, `CRT `).

**Stufe 2: Übernahme (Emulation angehalten)**

1. Laufende Vorgänge verwerfen: offene Reset-Anforderung, Autostart-Kommandozeile, Warten auf Reset.
2. SID-Emulation (eigene Engine / reSID) auf den Wert des Snapshots umschalten.
3. Alle Chunks laden: `MAIN`-Werte, CPU, MMU, CIA, VIC, SIDs, Floppys, Tape, Cartridge, REU/GeoRAM.
   Floppys, die im Snapshot aktiv waren, aber aktuell ausgeschaltet sind, werden eingeschaltet.
   Ist REU bzw. GeoRAM im Snapshot nicht gesteckt, wird das Modul entfernt.
4. Abgeleiteten Zustand neu aufbauen:
   * `MMU::ChangeMemMap()` (Banking-Tabellen)
   * `vic_buffer` auf den zuletzt fertigen Videopuffer
   * Tastatur-Cache ungültig setzen
   * Audio-Pufferpositionen auf 0
5. Schlägt in Stufe 2 ein Chunk fehl (inhaltlich ungültig, obwohl CRC stimmt), ist der C64 nur teilweise überschrieben. Dann wird ein **Hard-Reset** ausgeführt und `SNAPSHOT_ERROR_APPLY` gemeldet.

### 3.4 Rückgabecodes

```
SNAPSHOT_OK                 0
SNAPSHOT_ERROR_FILE         Datei nicht les-/schreibbar
SNAPSHOT_ERROR_FORMAT       keine Snapshot-Datei / beschädigt
SNAPSHOT_ERROR_VERSION      Snapshot aus neuerer Version
SNAPSHOT_ERROR_VIC_TYPE     PAL/NTSC passt nicht
SNAPSHOT_ERROR_TAPE_RECORD  Bandaufnahme läuft
SNAPSHOT_ERROR_APPLY        Übernahme fehlgeschlagen, C64 zurückgesetzt
```

---

## 4. Aufbau der Datei

Alle Zahlen sind **Little-Endian**. Es gibt keine Ausrichtung (Padding) zwischen den Feldern.

```
+------------------+------------------------------------------+
| Header (12 Byte) |                                          |
+------------------+------------------------------------------+
| Chunk 1          |  Tag, Version, Länge, Daten              |
| Chunk 2          |                                          |
| …                |                                          |
+------------------+------------------------------------------+
| CRC32 (4 Byte)   |  über alle Bytes davor                   |
+------------------+------------------------------------------+
```

### 4.1 Header

| Offset | Größe | Typ | Inhalt |
|---:|---:|---|---|
| 0 | 8 | ASCII | Magic `EMU64FRZ` |
| 8 | 2 | u16 | Format-Version (aktuell `1`) |
| 10 | 2 | u16 | reserviert (`0`) |

### 4.2 Chunk

| Offset | Größe | Typ | Inhalt |
|---:|---:|---|---|
| 0 | 4 | ASCII | Tag (z. B. `CPU `, mit Leerzeichen aufgefüllt) |
| 4 | 2 | u16 | Chunk-Version |
| 6 | 4 | u32 | Länge der Daten in Bytes |
| 10 | Länge | – | Daten |

Ein Reader springt über Chunks mit unbekanntem Tag einfach mit Hilfe der Länge hinweg. Lesezugriffe sind auf die Chunk-Länge begrenzt, ein beschädigter Chunk kann also nie über sein Ende hinaus lesen.

### 4.3 Prüfsumme

Die letzten 4 Bytes der Datei sind ein **CRC-32** (Polynom `0xEDB88320`, Startwert `0xFFFFFFFF`, Ergebnis invertiert, wie zlib/PNG) über alle Bytes von Offset 0 bis vor die Prüfsumme (Header + alle Chunks).

### 4.4 Grunddatentypen innerhalb der Chunks

| Kürzel | Bytes | Bedeutung |
|---|---:|---|
| `u8` | 1 | vorzeichenlos |
| `u16` | 2 | vorzeichenlos, Little-Endian |
| `u32` | 4 | vorzeichenlos, Little-Endian |
| `u64` | 8 | vorzeichenlos, Little-Endian |
| `i32` | 4 | vorzeichenbehaftet (Zweierkomplement) |
| `bool` | 1 | `0` oder `1` (auch nicht initialisierte Werte werden als `0`/`1` geschrieben) |
| `f32` | 4 | IEEE-754 single |
| `bytes[n]` | n | Rohdaten |

Zeiger werden nie gespeichert. Zeiger in Tabellen werden als Offset gespeichert (siehe `CPU`, `VIC `, `CRT `).

---

## 5. Die Chunks im Detail

Reihenfolge in der Datei:

```
MAIN, CPU , MMU , CIA1, CIA2, VIC , SID1, SID2, RSD1, RSD2,
FLP0..FLP3 (nur aktive Laufwerke), TAPE, CRT , REU  oder  GEO  (je nach Erweiterung)
```

Beim Laden wird jeder Chunk über seinen Tag gesucht, die Reihenfolge in der Datei ist also nicht entscheidend.

Alle Chunks haben in Format-Version 1 die Chunk-Version `1`, nur `VIC ` hat die Chunk-Version `2` (siehe 5.5).

### 5.1 `MAIN` – C64 allgemein

| Feld | Typ | Bemerkung |
|---|---|---|
| vic_typ | u8 | 0 = PAL, 1 = NTSC. Muss zur aktuellen Einstellung passen |
| c64_frequency | i32 | informativ, die aktuelle Einstellung bleibt erhalten |
| sid_emulation | i32 | 0 = eigene SID-Engine, 1 = reSID |
| io_source | u8 | 0 = nichts, 1 = Cartridge, 2 = REU, 3 = GeoRAM |
| cycle_counter | u32 | |
| last_reset_cycle | u32 | |
| floppy_clock_acc | i32 | Akkumulator 1-MHz-Floppytakt / C64-Takt |
| c64_iec_wire_drv | u8 | IEC: Sicht der Floppy auf die C64-Leitungen |
| c64_iec_wire | u8 | IEC: Leitungen vom C64 (Bit 4 ATN, 6 CLK, 7 DATA) |
| floppy_iec_wire | u8 | IEC: Leitungen von der Floppy (Bit 6 CLK, 7 DATA) |
| reset_wire | bool | |
| rdy_ba_wire | bool | |
| game_wire | bool | |
| exrom_wire | bool | |
| hi_ram_wire | bool | |
| lo_ram_wire | bool | |
| reset_hold_counter | i32 | verbleibende Zyklen eines laufenden Resets |
| reset_hold_soft | bool | |
| cpu_states[5] | 5 × bool | Index 0 = C64-CPU, 1–4 = Floppy 1–4 |
| c64_reset_ready | bool | |
| floppy_reset_ready[4] | 4 × bool | |

### 5.2 `CPU ` – MOS 6510 und CPU-Port

**6510**

| Feld | Typ | Bemerkung |
|---|---|---|
| mct_offset | u32 | Byte-Offset des Microcode-Zeigers in `MicroCodeTable6510` (muss < Tabellengröße sein) |
| Pointer | u8 | |
| Adresse | u16 | |
| BranchAdresse | u16 | |
| AktOpcode | u16 | |
| AktOpcodePC | u16 | |
| CpuWait | bool | |
| TMPByte | u8 | |
| PC | u16 | |
| AC, XR, YR, SP, SR | 5 × u8 | |
| Interrupts[10] | 10 × bool | Index: VIC_IRQ, CIA_IRQ, CIA_NMI, REU_IRQ, CRT_NMI, CRT_IRQ, EXT_IRQ, EXT_NMI, RESTORE_NMI, NOP_INT (Enum in `structs.h`) |
| irq_state | u8 | |
| irq_is_low_pegel, irq_is_active, irq_fetch_decision | 3 × bool | |
| nmi_hijack, i_flag_early | 2 × bool | |
| branch_irq_early, branch_irq_sample, branch_nmi_sample | 3 × bool | |
| irq_delay | bool | |
| irq_delay_sr_value | u8 | |
| nmi_state, nmi_edge_event, nmi_fall_edge, nmi_is_active, nmi_fetch_decision | 5 × bool | |
| shxy_dma | bool | |
| axa_byte | u8 | |
| RESET_OLD | bool | |
| idxReg | u8 | |
| JAMFlag | bool | |
| WRITE_FF00 | bool | |

**CPU-Port** (direkt anschließend)

| Feld | Typ |
|---|---|
| DIR, DATA, DIR_READ, DATA_READ, DATA_OUT | 5 × u8 |
| DATA_SET_CLK_BIT6, DATA_SET_CLK_BIT7 | 2 × u64 |
| DATA_SET_BIT6, DATA_SET_BIT7, DATA_FALLOFF_BIT6, DATA_FALLOFF_BIT7 | 4 × u8 |
| TAPE_SENSE, DATASETTE_MOTOR | 2 × bool |
| OLD_DATA_OUT, OLD_WRITE_BIT | 2 × u8 |

### 5.3 `MMU ` – Speicher

| Feld | Typ |
|---|---|
| RAM | bytes[65536] |
| Farb-RAM | bytes[1024] |

### 5.4 `CIA1` / `CIA2`

Je Chunk zuerst der CIA-Zustand, danach die beiden Ports (Port A, Port B: je `input_bits` u8, `output_bits` u8).

**CIA-Zustand** (in dieser Reihenfolge)

```
io                      bytes[16]
reset_wire_old          bool
pa_latch, pb_latch, sdr u8, u8, u8
flag_pin_old, cnt_pin   bool, bool
interupt_mask, cra, crb, icr, icr_new, icr_ack   6 × u8
irq_line, ifr_done      bool, bool
ifr_ack1, ifr_ack0, ifr_ack_m1, ifr_d7set1, ifr_d7set0,
ifr_raise1, ifr_raise0, ifr_read0, ifr_read1      9 × bool
timer_b_mode            u8
timer_a_underflow_old, timer_b_underflow_old, cnt_pin_old   3 × bool
sdr_bits                u8
sdr_valid               bool
sdr_load_delay, sdr_irq_delay                      2 × u8
clock_counter_latch, clock_counter                 2 × u32
clock_is_latched, clock_is_stopped                 2 × bool
ddr_a, ddr_b            2 × u8
enable_pb6, enable_pb7  2 × bool
pb6_mode, pb7_mode, pb_6, pb_7, prev_lp            5 × u8
Timer A: counter u16, latch u16, danach 11 × bool
         (cr_start, cr_oneshot, cr_force_load, count_phi2, count_step,
          count2, count3, load1, load, oneshot0, oneshot)
Timer B: wie Timer A
clock_latch             bytes[4]
clock_alarm             bytes[4]
```

### 5.5 `VIC ` – VIC-II

Reihenfolge der Felder (Namen wie in `mos6569_class.h`):

```
reg_mx[8]                 8 × u16
reg_my[8]                 bytes[8]
reg_mx8, reg_ctrl_1, reg_ctrl_2                 3 × u8
reg_y_scroll, reg_x_scroll                      2 × u16
reg_vbase                                       u8
reg_irq_raster                                  u16
reg_lpx, reg_lpy, reg_me, reg_mye, reg_mdp, reg_mmc, reg_mxe, reg_mm, reg_md,
reg_ec, reg_b0c, reg_b1c, reg_b2c, reg_b3c, reg_mm0, reg_mm1   16 × u8
reg_mcolor[8]             bytes[8]
creg[16]                  bytes[16]
creg_w_reg[66]            bytes[66]
creg_cur_reg, creg_cur_val, creg_pend_reg, creg_pend_val       4 × u8
current_cycle                                   u8
current_rasterline, new_irq_raster, current_x_coordinate        3 × u16
graphic_mode, irq_flag, irq_mask                3 × u8
den, vertical_blanking                          2 × bool
first_ba_cycle                                  u8
badline_enable, badline_status, display_status, display_status_g,
lp_triggered, lp_line_low, lp_pending           7 × bool
rc                                              u8
vc, vc_base, matrix_base, char_base, bitmap_base   5 × u16
vmli                                            u8
c_address, g_address, p_address, s_address      4 × u16
s_address_byte2[8]        8 × u16
s_byte2_pending                                 u8
p_data[8]                 bytes[8]
c_data_buffer_8bit[64]    bytes[64]
c_data_buffer_4bit[64]    bytes[64]
gfx_data, char_data, color_data                 3 × u8
is_write_reg_ec, is_write_reg_b0c, write_reg_0x11   3 × bool
sprite_y_exp_flip_flop                          u8
mc[8], mc_base[8]         2 × bytes[8]
sprite_dma, sprite_view                         2 × u8
sprite_sequenzer[8]       8 × u32
sprite_x_pipe[8]          8 × u16
sprite_pri_bits, sprite_mc_bits, sprite_expx_bits,
sprite_pending_bits, sprite_active_bits, sprite_halt_bits       6 × u8
sbuf_reg[8]               8 × u32
sbuf_pixel_reg[8]         bytes[8]
sbuf_expx_flops, sbuf_mc_flops                  2 × u8
csel, rsel                                      2 × u8
border_flip_flop0, border_flip_flop1, border_set_v   3 × bool
h_border_compare_left[2], h_border_compare_right[2],
v_border_compare_top[2], v_border_compare_bottom[2]    8 × u16
border_line[504]          bytes[504]
border_line_pos                                 u16
colors[4]                 bytes[4]
gfx_pipe0_g/c/v, gfx_pipe1_g/c/v, gfx_reg_g/c/v          9 × u8
gfx_pixel_reg, gfx_x_scroll, gfx_vmode11, gfx_vmode16, gfx_vmode16_2   5 × u8
gfx_mc_flop                                     bool
reg11_delay                                     u8
draw_line_counter                               i32
draw_this_line                                  bool
last_read_gp_access                             u8
creg_hist[66][16]         bytes[1056]
current_video_buffer      i32      Zähler des aktiven Puffers (Bit 0 wählt den Puffer)
video_buffer_line         i32      Offset der aktuellen Zeile in Bytes, ab Chunk-Version 2 relativ zum
                                   Anfang von video_buffer_back[0] (Bereich 0 … 2 × 157248). Die Zeile kann
                                   nach dem Puffertausch noch im anderen Puffer liegen.
                                   Chunk-Version 1: relativ zum aktiven Puffer (kann negativ sein oder > 157248)
video_buffer_back[2]      2 × bytes[157248]   (504 × 312 Byte je Puffer)
```

Beim Laden wird `video_buffer = video_buffer_back[current_video_buffer & 1]` und `video_buffer_line` aus dem Offset gesetzt (der Offset muss innerhalb der beiden Puffer liegen). Der Chunk `VIC ` hat ab Emu64 mit Format-Version 1 die **Chunk-Version 2**; Dateien mit Chunk-Version 1 werden weiterhin geladen. Konfigurationswerte des VIC (PAL/NTSC-Maße, Modell 6569/8565, Display-Zeilen) werden nicht gespeichert.

### 5.6 `SID1` / `SID2` – eigene SID-Engine

```
SidModel                i32      0 = 6581, 1 = 8580
IO[32]                  bytes[32]
LastWriteValue          u8
LastWriteCounter        u16
WriteReg                u8
Zyklencounter           i32
PotX, PotY              2 × u8
FilterKey, FilterFrequenz, FilterResonanz, Voice3Off, HpBpLp, Volume   6 × u32
MixerDC, Vhp, Vbp, Vlp, Vnf, w0, w0_ceil_1, w0_ceil_dt, _1024_div_Q, ext_in   10 × i32
FreqConvCounter         f32
Voice[0..2], je:
    OscEnable, TestBit, RingBit, SyncBit      4 × bool
    Waveform                                  u8
    Frequenz, Pulsweite, Accu, ShiftReg, MsbRising   5 × u32
    SyncSource, SyncDestination               2 × i32
    KeyBit                                    bool
    State                                     i32
    RatePeriod, RateCounter, ExponentialCounterPeriod,
    ExponentialCounter, EnvCounter            5 × u32
    HoldZero                                  bool
    Attack, Decay, Sustain, Release           4 × u32
IODelayEnable           bool
IODelayRPos, IODelayWPos  2 × i32
pending                 u32      Anzahl noch nicht ausgeführter verzögerter Schreibzugriffe
pending × (Register u8, Wert u8)
```

Das Wellenform- und Filtertabellen-Setup wird beim Laden über `SetChipType(SidModel)` neu aufgebaut. Der Zustand des IO-Delay-Puffers wird nur übernommen, wenn die Einstellung „IO-Delay“ im Snapshot und aktuell übereinstimmt.

### 5.7 `RSD1` / `RSD2` – reSID

Die Daten stammen aus `reSID::SID::read_state()` und werden mit `write_state()` zurückgespielt.

```
sid_model               i32      0 = 6581, 1 = 8580
io[32]                  bytes[32]
freq_conv_counter       f32
cycle_counter           i32
pot_x, pot_y            2 × u8
sid_register[32]        bytes[32]
bus_value               u8
bus_value_ttl           i32
write_pipeline          i32
write_address           u8
voice_mask              u8
je Stimme (3 ×, jeweils alle Felder einer Stimme zusammen):
    accumulator                   u32
    shift_register                u32
    shift_register_reset          i32
    shift_pipeline                i32
    pulse_output                  u16
    floating_output_ttl           i32
    rate_counter                  u16
    rate_counter_period           u16
    exponential_counter           u16
    exponential_counter_period    u16
    envelope_counter              u8
    envelope_state                i32   0 ATTACK, 1 DECAY_SUSTAIN, 2 RELEASE, 3 FREEZED
    hold_zero                     bool
    envelope_pipeline             i32
```

**Bekannte Grenze:** Der Resampler und die analogen Filterzustände (Vhp/Vbp/Vlp, externer Filter) gehören nicht zum reSID-State und starten nach dem Laden neu. Der `rate_counter` kann nach dem Laden um 1 vom Original abweichen (Eigenheit von `read_state`/`write_state`). Beides ist praktisch nicht hörbar.

### 5.8 `FLP0` … `FLP3` – Floppy 1541

Ein Chunk je **eingeschaltetem** Laufwerk (Nummer im Tag: `FLP0` = Laufwerk 8 … `FLP3` = Laufwerk 11).

```
WriteProtect, WriteProtectAkt, ImageReadOnly         3 × bool
DiskChangeSimState, DiskChangeSimCycleCounter        2 × i32
VIA1_IRQ, VIA2_IRQ, IRQ                              3 × bool
CycleCounter, BitAccu, BitCounter, OnesCount         4 × i32
SyncActive                                           bool
NoFluxCells, CellsSinceOne, RandFluxCountdown        3 × i32
ReadNoiseSeed                                        u32
ReadShift, ReadLatch                                 2 × u8
ReadLatched                                          bool
WriteShift                                           u8
AktHalbSpur                                          u8      (muss < 84 sein)
ImageWriteStatus, ImageDirectoryWriteStatus          2 × bool
GCRBitTrackSize, GCRBitTrackPos, SyncBitCounter      3 × u32
SyncBitsFound                                        bool
FloppyIECLocal                                       u8
StepperInc, StepperDec, StepperAnschlag, DiskMotorOn, StepperIncWait   5 × bool
stepper_bump                                         u8
RAM                                                  bytes[2048]
6502-CPU (siehe 5.8.1)
VIA1 (siehe 5.8.2)
VIA2 (siehe 5.8.2)
disk                                                 bool    Disk eingelegt?
falls disk = 1:
    ImageTyp                                         i32     Wert aus C64_FILE_TYPES (D64 = 4, G64 = 5, NIB = 10)
    D64Tracks                                        i32     35 … 42
    D64HasErrorMap                                   bool
    D64ErrorMap                                      bytes[802]
    TrackSize[256]                                   256 × u16   (jede Spurgröße ≤ 7928)
    GCRImage                                         bytes[665952]  (84 Halbspuren × 7928 Bytes)
```

Die Disk wird immer als **GCR-Image** gespeichert, denn nur dort sind Schreibzugriffe der Floppy aktuell. Beim Laden:

* Die Disk liegt nur im Speicher; ein eventuell geöffnetes Image-File wird geschlossen. Änderungen werden **nicht** in die D64-Datei zurückgeschrieben.
* Zeiger (`GCR_PTR`, `GCRSpurStart`, `GCRSpurEnde`) werden aus `AktHalbSpur` neu berechnet, danach werden `GCRBitTrackSize`/`GCRBitTrackPos` (Drehposition) wieder auf die gespeicherten Werte gesetzt.
* Beim Auswerfen über die GUI wird die Disk normal entfernt.

#### 5.8.1 6502 (Floppy-CPU)

```
mct_offset (Offset in MicroCodeTable6502)   u32
Pointer                                     u8
Adresse, BranchAdresse, AktOpcode, AktOpcodePC   4 × u16
JAMFlag                                     bool
TMPByte                                     u8
PC                                          u16
AC, XR, YR, SP, SR                          5 × u8
irq_state                                   u8
irq_is_low_pegel, irq_is_active, branch_irq_early, branch_irq_sample,
irq_fetch_decision, irq_delay               6 × bool
irq_delay_sr_value                          u8
tmp, tmp1                                   2 × u32
tmp2, src                                   2 × u16
carry_tmp                                   bool
idxReg                                      u8
```

#### 5.8.2 VIA 6522

```
IO                                          bytes[16]
PA, PB, DDRA, DDRB                          4 × u8
TimerA, TimerALatch, TimerB, TimerBLatch    4 × u16
TimerBArmed, ATNState, IECInterrupt         3 × bool
OldIECLines                                 u8
OldATNState                                 bool
TmpByte                                     u8
counter_sample_pb3, addition_sample_pb3     2 × u32
rms_pb3                                     f32
```

### 5.9 `TAPE` – Datasette

```
tape_typ                u8      0 = kein (unterstütztes) Band, 1 = TAP
PressedKeys             u8
TapeStatus              i32     0 Stop, 1 Play, 2 Rew, 3 FFw, 4 Rec, 5 Pause (TAPE_IS_*)
LastTapeStatus          i32
CassRead                bool
MotorStatusTmp          bool
CPU_PORT_OLD            u8
WaveStatus              i32
OldWaveStatus           i32
ZyklenCounter           u32
FreqConvCounter         f32
WaitCounter             u32
Counter                 f32
PlayCounter             u32
PlayAddWert             u32
falls tape_typ = 1:
    TapeVersion             u8
    WritePotected           bool
    TapePosIsStart          bool
    TapePosIsEnd            bool
    TapeBufferPos           u32      (< TapeBufferSize)
    TapePosCycles           u32
    TapeBufferSize          u32      (> 0, ≤ Rest des Chunks)
    TapeBuffer              bytes[TapeBufferSize]   Rohdaten des TAP (ohne TAP-Header)
```

Steht im Snapshot kein Band (`tape_typ = 0`), wird beim Laden das aktuell eingelegte Band ausgeworfen und alle Tasten werden gelöst.

### 5.10 `CRT ` – Cartridge

```
cartridge_is_insert     bool
falls 1:
    cartridge_type          u16      CRT-Hardwaretyp (z. B. 1 Action Replay, 3 Final Cartridge III, 32 EasyFlash)
    cartridge_exrom         bool
    cartridge_game          bool
    easyflash_jumper        bool
    easyflash_ram           bytes[256]
    easyflash_bank_reg      u8
    rom_lo_bank             u32
    ar_reg                  u8
    ar_freez                bool
    ar_active               bool
    ar_enable_ram           bool
    ar_ram                  bytes[8192]
    pla_address             u8
    led_00, led_00_old, led_01, led_01_old   4 × bool
    lo_rom: Bank u8 (0 = keiner, 1 = rom_bank1, 2 = rom_bank2), Offset u32
    hi_rom: Bank u8, Offset u32
    Flash Lo (AM29F040)     flash_type i32, status i32, last_read u8, programm_byte u8, dirty bool
    Flash Hi (AM29F040)     wie Flash Lo
    rom_bank1               bytes[524288]    (64 × 8 KiB)
    rom_bank2               bytes[524288]
```

`lo_rom` / `hi_rom` zeigen im Betrieb in `rom_bank1` bzw. `rom_bank2`. Sie werden als (Bank, Byte-Offset) gespeichert und beim Laden wieder zu Zeigern aufgelöst. Steckt im Snapshot kein Modul, wird das aktuelle Modul entfernt.

### 5.11 `REU ` (nur wenn `io_source = 2`)

```
reu_ram_mode            u8       0 = 128 KiB … 7 = 16 MiB
is_reu_insert, reu_wait_ff00, BA_STATUS   3 × bool
io                      bytes[11]
cpu_wait_counter        u8
tarnsfer_start          bool
c64_address, reu_address, counter   3 × u16
current_bank, transfer_typ, DMAStatus   3 × u8
RAM                     ram_bank_count × 64 KiB   (nur die tatsächlich genutzte Größe)
```

### 5.12 `GEO ` (nur wenn `io_source = 3`)

```
geo_ram_insert          bool
geo_ram_mode            u8       0 = 512 KiB, 1 = 1 MiB, 2 = 2 MiB, 3 = 4 MiB
mem_frame               u32
_dffe, _dfff            2 × u8
reg_start_address       u16
RAM                     geo_ram_size_tbl[geo_ram_mode] Bytes
```

---

## 6. Einschränkungen

* **PAL/NTSC** muss übereinstimmen. Der Snapshot stellt die Norm nicht um.
* **ROM-Set:** Es wird nicht geprüft, ob zum Zeitpunkt des Ladens dasselbe ROM-Set aktiv ist wie beim Speichern. Ein anderes Kernal/1541-DOS kann zu unsinnigem Verhalten führen.
* **WAV-Bänder** werden nicht gespeichert (nur TAP). Beim Laden eines Snapshots mit einem WAV-Band wird das Band ausgeworfen.
* **Bandaufnahme:** Während einer laufenden Aufnahme ist kein Snapshot möglich.
* **Disk-Schreibzugriffe nach dem Laden** bleiben im Speicher und werden nicht in die ursprüngliche Image-Datei zurückgeschrieben.
* **reSID:** siehe 5.7 (Filter-/Resampler-Zustand startet neu, `rate_counter` ±1).
* Eine Floppy, die durch das Laden eingeschaltet wurde, wird im Hauptfenster nicht automatisch als „an“ dargestellt.
* Snapshots sind nicht zwischen **inkompatiblen Format-Versionen** austauschbar (siehe 7).
* Größe: ca. 1,6 MB mit einem Laufwerk (RAM 64 KB, Videopuffer ~315 KB, Disk 650 KB, Cartridge-ROM-Bänke je 1 MB wenn gesteckt); mit REU bis zu 16 MB zusätzlich. Es wird nicht komprimiert.

---

## 7. Versionierung und Kompatibilität

* Die **Format-Version** im Header ändert sich nur bei inkompatiblen Änderungen am Aufbau (Header, Chunk-Struktur). Emu64 lehnt Dateien mit größerer Format-Version ab.
* Jeder Chunk hat zusätzlich eine eigene **Chunk-Version**. Ändert sich der Inhalt eines Chunks, wird dessen Version erhöht und der Lesecode muss die alten Versionen weiter unterstützen (`switch` auf die Chunk-Version).
* Neue Komponenten werden als **neue Chunks** ergänzt. Ältere Emu64-Versionen überspringen unbekannte Chunks. Der Tag muss dann in der Liste der Pflicht-Chunks bleiben, wenn die Komponente für den Betrieb zwingend ist.
* Das alte Freez-Format von Emu64 bis Version 4.4 (Windows) wird **nicht** unterstützt.

---

## 8. Hinweise für Entwickler

### 8.1 Dateien

| Datei | Inhalt |
|---|---|
| `src/state_stream.h/.cpp` | `StateWriter`, `StateReader`, `StateCRC32` |
| `src/c64_class.cpp` | `SaveSnapshot`, `SaveSnapshotToBuffer`, `LoadSnapshot`, `LoadSnapshotFromBuffer` |
| `src/main_window.cpp` | Menü, Slots, Dateidialoge, Fehlermeldungen (`SnapshotSave`, `SnapshotLoad`, `InitSnapshotSlotMenus`) |
| `src/*_class.cpp` | `SaveState(StateWriter&) const` / `bool LoadState(StateReader&)` je Chip |

### 8.2 API des Serializers

```cpp
StateWriter w;
w.BeginChunk("ABCD", 1);      // Tag (4 Zeichen), Chunk-Version
w.U8(x); w.U16(x); w.U32(x); w.U64(x); w.I32(x);
w.Bool(x); w.F32(x); w.F64(x);
w.Bytes(ptr, n);
w.EndChunk();                 // trägt die Länge nach

StateReader r(data, size);
uint16_t ver;
if(r.FindChunk("ABCD", ver)) { x = r.U8(); … r.Bytes(ptr, n); }
if(!r.Ok()) …                 // Fehler werden gemerkt, gelesen wird nie über das Chunk-Ende hinaus
```

Lesen über das Chunk-Ende hinaus liefert 0 und setzt den Fehlerzustand. `r.Remaining()` liefert die Restgröße des aktuellen Chunks (nützlich zur Plausibilitätsprüfung von Längenangaben).

### 8.3 Eine neue Komponente ergänzen

1. In der Klasse `SaveState`/`LoadState` implementieren. **Reihenfolge und Typen in beiden identisch halten.**
2. In `C64Class::SaveSnapshotToBuffer` einen Chunk schreiben, in `LoadSnapshotFromBuffer` laden.
3. Wenn die Komponente zwingend ist, den Tag in `required_chunks` eintragen.
4. Gespeichert werden **nur Zustandsvariablen**, keine Zeiger, `Delegate`s, `std::function`, Mutexe, Konfiguration, Debug-Daten oder Audiopuffer.
5. Nach dem Laden abgeleiteten Zustand neu berechnen (in `LoadSnapshotFromBuffer`, Abschnitt „Abgeleiteten Zustand neu aufbauen“).
6. Dieses Dokument ergänzen.

### 8.4 Fallen

* **`static`-Variablen in Funktionen** sind versteckter Zustand und werden nicht gespeichert. Trägt eine solche Variable Zustand über mehrere Zyklen, muss sie zum Member werden (so geschehen in `MOS6510::OneZyklus` (`RESET_OLD`, `idxReg`), `MOS6510_PORT::ConfigChanged`, `TAPE1530::OneCycle`, `Floppy1541::SpurDec`). Reine Zwischenwerte, die vor jedem Lesen neu gesetzt werden, sind kein Zustand.
* **Nicht initialisierte `bool`-Member** (Wert ≠ 0/1) würden als falscher Wert geschrieben. `StateWriter::Bool` normalisiert sie. Trotzdem sollten neue Member initialisiert werden.
* **Zeiger in Tabellen** (Microcode-Zeiger, Cartridge-ROM-Zeiger, Videopuffer-Zeiger, GCR-Zeiger) als Offset speichern und beim Laden prüfen, ob er im gültigen Bereich liegt.
* Beim Laden **erst alles lesen und prüfen, dann zuweisen**, wo es möglich ist (siehe `TAPE1530::LoadState`, `CartridgeClass::LoadState`). Ein fehlgeschlagener Chunk führt sonst zu einem teilweise überschriebenen Zustand.
* Der Emulationsthread darf beim Speichern/Laden nicht laufen (siehe 3.1). Neue Aufrufe müssen über `SaveSnapshot`/`LoadSnapshot` gehen.
* **Warp-Modus:** Beide Funktionen schalten ihn kurzzeitig aus und wieder an. Sie dürfen deshalb nicht aus dem Warp-Thread selbst aufgerufen werden.

---

## 9. Testen

Der Kern der Funktion ist die **Determinismus-Prüfung**: Zustand speichern, weiterlaufen lassen, den gespeicherten Zustand laden, gleich lange weiterlaufen lassen und beide Endzustände vergleichen. Sie müssen identisch sein.

Beim Entwickeln wurde dafür ein temporärer Selbsttest in `NextSystemCycle()` verwendet (inzwischen wieder entfernt):

1. Beim Zykluszähler `N` den Zustand mit `SaveSnapshotToBuffer` sichern (`s0`).
2. `M` Zyklen weiterlaufen lassen, Zustand sichern (`sa`).
3. `LoadSnapshotFromBuffer(s0)`, wieder `M` Zyklen laufen lassen, Zustand sichern (`sb`).
4. `sa` und `sb` chunkweise per CRC vergleichen.

Wichtig: Beim manuellen Weiterlaufen müssen die Audio-Pufferpositionen (SID, reSID, Floppy, Tape) regelmäßig auf 0 gesetzt werden, sonst läuft der Soundpuffer über.

Ergebnis mit dem Spiel *Hudson Hawk* (D64, Floppy lädt mitten im Test von der Disk), `N` zwischen 1,5 Mio. und 15 Mio. Zyklen, `M` = 400 000 Zyklen: MAIN, CPU, MMU, CIA 1/2, VIC, SID, Floppy, Tape und Cartridge waren in allen Läufen **bitgleich**. Der reSID-Chunk wich in manchen Läufen nur im `rate_counter` um 1 ab (siehe 5.7).

Manuelle Tests:

* Snapshot im Spiel speichern, Emu64 beenden und neu starten, Snapshot laden: das Spiel muss an derselben Stelle weiterlaufen.
* Beschädigte (abgeschnittene, veränderte) `.frz`-Dateien dürfen nur eine Fehlermeldung auslösen, nie einen Absturz.
* Speichern und Laden (auch per Hotkey) im Warp-Modus und im Debug-Modus.
