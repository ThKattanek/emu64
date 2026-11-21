# Messungen am echten C64

Ergebnisse der Messungen mit den Skripten in diesem Verzeichnis (siehe `README.md`). Pro Messkampagne ein Abschnitt, neueste oben.

Die Rohdaten (Aufnahmen) liegen lokal unter `build/hardware_captures/` und sind wegen ihrer Größe (ca. 1 GB) nicht im Git.

## Geräte

| Gerät | Assy/PCB | Bestückung | VIC-Revision |
|---|---|---|---|
| C64-01 | - | nicht erfasst | MOS-6569R5 |
| C64-02 | 250469 | MOS-8500, MOS-8580R5, MOS-8565R2, 2x MOS-6526A | MOS-8565R2 |
| C64-05 | 250469 | CSG-8500, CSG-8580R5, CSG-8565R2, 2x CSG-6526A | CSG-8565R2 |
| C64-07 | 250469 | CSG-8500, CSG-8580R5, CSG-8565R2, 2x CSG-6526A | CSG-8565R2 |
| C64-06 | 250407 | MOS-6510, MOS-6581, MOS-6569R3, 2x MOS-6526R4 | MOS-6569R3 |
| C64-08 | 251137 | MOS-6510, ThePicoSid, MOS-6569R3, 2x MOS-6526R4 | MOS-6569R3 |

Messaufbau: 1541 Ultimate II+ (Firmware 3.14d, Programmstart per `runners:run_prg`), Grabber stk1160 (EasyCAP, PAL 720x576, Composite), Kontrast während der Aufnahme 48 (bei 64 übersteuert Weiß), 20 Bilder gemittelt.

---

## #446: ECM/BMM-Umschaltung mitten in der Zeile (2026-11-21)

> **Nachtrag 8565 (2026-11-21 abends):** Von drei gemessenen 8565R2 zeigen zwei warm den Nachlauf (C64-02, C64-07), einer nicht (C64-05). Die Regel "8565: gelöschtes Bit wirkt ab Pixel 1" bildet damit die Mehrheit ab, gilt aber nicht für jeden Chip, siehe Abschnitt "Weitere 8565 (C64-05, C64-07)" unten. Die übrigen Ergebnisse (6569, ECM-Verzögerung im Grafikabruf) sind davon nicht betroffen.

### Anlass

Die VICE-Testbench `VICII/vicii_timing` (`vicii_reg_timing.prg`, `-a5`, `-ff`) schlug auf beiden VIC-Modellen fehl. Die Referenzbilder sind laut Testliste unsicher (`comment:unsafe reference`) und laut VICE-SVN r37337 von Hand angepasst ("the references probably still need tweaking, its very hard to see on a CRT"). VICE besteht `-a5`/`-ff` selbst nicht. Deshalb wurde an echten Geräten gemessen, wie sich der VIC beim Ein- und Ausschalten von ECM und BMM verhält.

### Ergebnis (in Emu64 umgesetzt)

| | 6569 (R3 und R5) | 8565 (R2) |
|---|---|---|
| Pixelfarbe, Bit wird gesetzt | ab Pixel 4 der Zelle | ab Pixel 0 |
| Pixelfarbe, BMM wird gelöscht | ab **Pixel 5** | ab **Pixel 1** |
| Pixelfarbe, ECM wird gelöscht | ab Pixel 6 | ab **Pixel 1** |
| Grafikabruf, Bit wird gesetzt | sofort | einen Zyklus später |
| Grafikabruf, Bit wird gelöscht | **ECM und BMM** einen Zyklus später | einen Zyklus später |

Fett: weicht von VICE ab. VICE verzögert beim 6569 im Grafikabruf nur BMM und löscht beide Bits in der Pixelfarbe ab Pixel 6, beim 8565 wirkt dort jede Änderung ab Pixel 0.

Folge für den ungültigen Modus (ECM+BMM, schwarz): Werden beide Bits gleichzeitig gelöscht, ist beim 6569 Pixel 5 bereits im ECM-Textmodus (BMM ist weg, ECM noch nicht). Beim 8565 bleibt Pixel 0 der folgenden Zelle noch schwarz.

**Temperatur:** Beim kalten 8565 liegen die Kanten etwa ein Pixel früher (siehe Aufwärmserie). Nachgebildet ist der warme Chip. Vor jeder Messung das Gerät mindestens 15 Minuten laufen lassen.

### VIC-Erkennung

`hw_suite.sh` vergleicht den Bereich der Moduswechsel mit beiden Modellen (RMSE, kleiner = passender):

| Gerät | gegen 6569 | gegen 8565 | erkannt |
|---|---|---|---|
| C64-01 | 8,6 | 17,6 | 6569 |
| C64-02 | 22,5 | 9,8 | 8565 |
| C64-05 | 23,3 | 14,1 | 8565 |
| C64-07 | 22,0 | 10,1 | 8565 |
| C64-06 | 13,4 | 19,6 | 6569 |
| C64-08 | 12,3 | 18,1 | 6569 |

### Gesamtauswertung aller Messsätze

`hw_suite_eval.py --emu VICE-Stand=<bdf914e> --emu aktuell=<master> --all`, gezählt über die strittigen Stellen aus `suite_regions.txt`, an denen sich die Kandidaten um mehr als 5 % unterscheiden. "VICE-Stand" ist Emu64 `bdf914e` (bei `vicii_reg_timing*` und `modesplit` geprüft pixelgleich zu VICE r46305), "aktuell" ist der Stand nach #446, "Referenz" sind die Referenzbilder der VICE-Testbench. Gleichstände zählen für alle Beteiligten.

| Messsatz | Stellen | aktuell | Referenz | VICE-Stand |
|---|---|---|---|---|
| C64-01_6569R5 (warm, ohne modesplit) | 15 | 14 | 4 | 0 |
| C64-06_6569R3 (warm) | 14 | 12 | 4 | 1 |
| C64-08_6569R3 (warm) | 13 | 11 | 3 | 1 |
| C64-02_8565R2_mittag (warm, nur vicii_timing) | 6 | 6 | 2 | 0 |
| C64-02_8565R2_t00 (kalt) | 14 | 9 | 4 | 5 |
| C64-02_8565R2_t15 | 15 | 15 | 3 | 0 |
| C64-02_8565R2_t30 | 15 | 15 | 3 | 0 |
| C64-02_8565R2_t45 | 15 | 15 | 3 | 0 |

Die einzige Stelle, an der warm nicht der aktuelle Stand gewinnt, ist BMM-aus mit Muster ff (weißer Vordergrund, Weiß neben Weiß, im Grabberbild kaum unterscheidbar) und `videomode-z` Zeile 100 auf dem 6569 (unabhängig von #446, siehe offene Punkte).

### Aufwärmserie C64-02 (8565R2)

C64-02 um 16:09 kalt eingeschaltet, Messsatz direkt danach (t00) und nach 15, 30, 45 Minuten. Helligkeit am Ende des schwarzen Blocks in der E+B-Zeile (Zeile 190), normiert auf 0 = schwarz, 100 = grau. Die Zellgrenze liegt zwischen x=111 und x=112.

| x | 110 | 111 | 112 | 113 |
|---|---|---|---|---|
| a5, kalt (t00) | 10 | **107** | **109** | 73 |
| a5, t15 / t30 / t45 | 1 / 0 / 0 | **2 / 1 / 1** | **5 / 5 / 5** | 84 / 85 / 84 |
| a5, mittags (warm) | 0 | **1** | **5** | 90 |
| ff, kalt (t00) | 6 | **68** | **120** | 104 |
| ff, t15 / t30 / t45 | 1 | **2** | **42 / 43 / 43** | 89 |
| ff, mittags (warm) | 1 | **2** | **42** | 89 |

Kalt endet das Schwarz einen Pixel vor der Zellgrenze, warm einen Pixel danach. Ab 15 Minuten ändert sich nichts mehr, und warm stimmt die Messung vom Mittag (nach längerer Laufzeit) exakt mit der vom Nachmittag überein.

### Mustermessungen

Das Füllmuster der Bitmap (`vicii_reg_timing.prg`, Byte 8565, Original `$5a`) in 13 Varianten: `00 01 40 41 80 81 c0 c1 5a a5 ff 7f fe`. Damit variieren das letzte Pixel der alten Zelle sowie Pixel 0 und 1 der neuen Zelle unabhängig voneinander. Ausgewertet wird jeweils um die Ausschaltkante (x 100-124, je Zeile).

**C64-02 (8565R2), Einzelpixel x=112 nach dem Löschen.** Hypothese "Nachlauf" (Pixel 0 noch im alten Modus) gegen "ohne" (Stand VICE). In allen eindeutigen Fällen gewinnt der Nachlauf, unentschieden sind nur BMM-Fälle mit weißem Vordergrund in beiden Modi (81, c1, a5, ff). Beispiele (RMSE Nachlauf / ohne):

| Muster | BMM | ECM | E+B |
|---|---|---|---|
| 00 | 3,9 / 17,4 | 9,9 / 16,4 | gleich |
| 80 | 9,1 / 17,0 | 9,9 / 16,4 | 1,0 / 26,8 |
| c0 | 8,1 / 20,6 | 9,8 / 16,4 | 7,2 / 29,5 |
| 5a | 15,6 / 26,9 | 9,8 / 16,4 | gleich |
| fe | 31,6 / 38,4 | 9,9 / 16,5 | 4,5 / 26,5 |

Ein- und Ausschaltkante getrennt (Mittel über alle 13 Muster): Nachlauf nur beim Löschen ist an beiden Kanten am besten.

| Variante | Einschaltkante | Ausschaltkante |
|---|---|---|
| ohne Nachlauf (VICE) | 11,65 | 16,40 |
| Nachlauf beim Setzen und Löschen | 16,20 | 11,62 |
| **Nachlauf nur beim Löschen** | **11,65** | **11,61** |

**C64-01 (6569R5), Pixel, ab dem das gelöschte Bit wirkt** (Mittel über 13 Muster, je Zeile):

| Variante | BMM | ECM | E+B |
|---|---|---|---|
| Löschen ab Pixel 4 | 13,81 | 29,94 | 11,74 |
| Löschen ab Pixel 5 | 9,07 | 28,24 | 6,73 |
| Löschen ab Pixel 6 (VICE) | 15,67 | 27,49 | 10,25 |
| Löschen ab Pixel 7 | 23,46 | 28,77 | 15,52 |
| Pixel 5, mit ECM-Verzögerung im Grafikabruf | 9,12 | 9,52 | 6,75 |
| Pixel 6, mit ECM-Verzögerung im Grafikabruf | 15,72 | 6,98 | 10,26 |
| **BMM ab Pixel 5, ECM ab Pixel 6, ECM-Verzögerung im Abruf** | **9,12** | **6,98** | **6,75** |

Ohne die ECM-Verzögerung im Grafikabruf passt die ECM-Zeile gar nicht (das ECM-Glyphenmuster läuft auf der Hardware eine Zelle länger). Einschaltkante: Setzen ab Pixel 4 = 7,48, ab Pixel 3 = 12,06, ab Pixel 5 = 10,78. Gesamtbild 4,72 (VICE-Stand) gegen 4,37 (aktuell).

**C64-08 (6569R3), Prüfung, ob R3 sich anders verhält** (Mittel über 13 Muster):

| Variante | BMM | E+B |
|---|---|---|
| **aktuell (BMM ab Pixel 5)** | **13,27** | **10,45** |
| BMM ab Pixel 6 | 16,78 | 10,44 |
| E+B wie VICE (ab Pixel 6) | 13,27 | 11,31 |

Pixel 5 gewinnt in 10 von 13 Mustern. Nur ff und 7f (fast nur weißer Vordergrund) liegen knapp bei Pixel 6. Kein Unterschied zwischen R3 und R5, der nachgebildet werden müsste.

### Weitere 8565 (C64-05, C64-07)

#### C64-05

Gleiche Platine wie C64-02 (Assy 250469), aber CSG- statt MOS-Chips. Eingeschaltet 19:38, gemessen nach 16, 40 und 60 Minuten (Messsätze `C64-05_8565R2_t16/t40/t60`, je 13 Muster in `muster_C64-05_8565R2_t*`).

Ende des schwarzen Blocks in der E+B-Zeile (Zeile 190), normiert auf 0 = schwarz, 100 = grau, Zellgrenze zwischen x=111 und x=112:

| Muster a5 | x=110 | x=111 | x=112 | x=113 |
|---|---|---|---|---|
| C64-05 nach 16 / 40 / 60 Min. | 7 / 6 / 7 | **92 / 91 / 92** | **94 / 94 / 93** | 99 / 99 / 98 |
| C64-02 kalt | 10 | **107** | **109** | 73 |
| C64-02 warm (45 Min.) | 0 | **1** | **5** | 84 |

Mustermessung (Mittel über 13 Muster, Pixelnachlauf beim Löschen gegen VICE-Verhalten):

| C64-05 | BMM | ECM | E+B |
|---|---|---|---|
| nach 16 Min.: mit Nachlauf / ohne | 28,5 / **25,2** | 16,2 / **9,3** | 16,9 / **6,8** |
| nach 40 Min.: mit Nachlauf / ohne | **25,6** / 27,4 | 16,3 / **9,4** | 17,3 / **7,3** |
| nach 60 Min.: mit Nachlauf / ohne | **25,4** / 27,8 | 16,3 / **9,4** | 18,3 / **8,0** |

Ergebnis: Der C64-05 ist ab 16 Minuten stabil, und seine Kante liegt warm einen Pixel **vor** der Zellgrenze (wie der C64-02 kalt). Beim 8565 gibt es damit drei Lagen derselben Kante: C64-05 warm einen Pixel vor der Zellgrenze, VICE an der Zellgrenze, C64-02 warm einen Pixel danach. Ob das an MOS gegen CSG liegt oder am einzelnen Chip, ist mit zwei Geräten nicht zu sagen. Emu64 bildet derzeit den C64-02 nach (seit #446). Entscheidung über das Standardverhalten erst nach Messung weiterer 8565 (zwei Geräte vorhanden).

#### C64-07

Gleiche Platine und ebenfalls CSG-Chips wie C64-05. Eingeschaltet 21:38, gemessen direkt danach (kalt) sowie nach 15, 40 und 45 Minuten (`C64-07_8565R2_t00/t15/t40/t45`, je 13 Muster in `muster_C64-07_8565R2_t*`).

| E+B, normiert (0 = schwarz, 100 = grau) | x=111 | x=112 | x=113 |
|---|---|---|---|
| C64-07 a5, kalt bis 45 Min. | 1 bis 2 | **-1 bis 1** | 84 bis 88 |
| C64-07 ff, kalt bis 45 Min. | 3 | **42 bis 44** | 92 bis 93 |
| zum Vergleich C64-02 warm (a5 / ff) | 1 / 2 | **5 / 43** | 84 / 89 |

| C64-07, Mittel über 13 Muster | BMM | ECM | E+B |
|---|---|---|---|
| mit Nachlauf (aktuell), t00 / t45 | **15,9 / 16,1** | **9,6 / 10,1** | **6,5 / 6,5** |
| ohne (VICE), t00 / t45 | 21,3 / 21,4 | 14,2 / 14,7 | 17,8 / 17,7 |

Ergebnis: Der C64-07 hat den Nachlauf wie der C64-02 warm, und zwar schon kalt, ohne Änderung bis 45 Minuten.

#### Zwischenstand 8565

| Gerät | Chips | warm | kalt |
|---|---|---|---|
| C64-02 | MOS | Nachlauf (1 Pixel nach der Zellgrenze) | 1 Pixel vor der Zellgrenze |
| C64-05 | CSG | 1 Pixel vor der Zellgrenze | (nicht kalt gemessen, ab 16 Min. stabil) |
| C64-07 | CSG | Nachlauf | Nachlauf |

MOS gegen CSG ist nicht die Ursache (C64-05 und C64-07 haben dieselbe Bestückung). Die Kante streut von Chip zu Chip und je nach Chip auch mit der Temperatur. Zwei von drei Geräten (und die modesplit-Referenzen von gpz' C64C) zeigen warm den Nachlauf, Emu64 bildet dieses Verhalten ab. Ein weiterer 8565 ist noch zu messen.

### Folgen für die VICE-Testbench

| Test | vor #446 | danach | Bemerkung |
|---|---|---|---|
| `VICII/split-tests/modesplit` (6569 und 8565) | error | **ok** | Referenzen laut README von gpz im Dezember 2025 gegen echte Geräte abgeglichen, VICE besteht sie nicht |
| `VICII/vicii_timing/vicii_reg_timing*` | error | error | Abweichung in einzelnen Pixeln von den von Hand angepassten Referenzen, dort stimmt Emu64 mit den gemessenen Geräten überein |
| `VICII/videomode/*` | teils ok | teils error | Abweichungen von 1 bis 5 Pixeln. Laut README hängen diese Kanten von Chip und Temperatur ab, an allen geänderten Stellen liegt Emu64 näher an den gemessenen Geräten als die Referenz |

### Offene Punkte

- `videomode-z` Zeile 100 (x 228-244) auf dem 6569: Die Referenz liegt auf allen drei 6569 näher an der Hardware als Emu64 (z.B. C64-06: 2,99 gegen 5,64). Unabhängig von #446 (VICE-Stand und aktueller Stand sind dort gleich).
- 8565: Kante streut je nach Chip um etwa einen Pixel (C64-02 und C64-07 mit Nachlauf, C64-05 ohne). Einen weiteren 8565 messen, dann das Standardverhalten endgültig festlegen (derzeit Nachlauf, wie die Mehrheit). Die `-8565`-Referenzen von gpz' C64C sind dabei keine Hilfe: modesplit passt zum C64-02-Verhalten, videomode zum VICE-Verhalten.

### Wiederholen

```bash
cd testbench/hardware
./hw_suite_eval.py --all                                   # alle Messsätze mit dem aktuellen Emu64 (EMU64_BIN)
./hw_suite_eval.py --emu alt=/pfad/emu64 --emu neu=/pfad/emu64 C64-06_6569R3
./hwcompare.py compare --region BMM=172:179,100:124 \
    ../../build/hardware_captures/muster_C64-08_6569R3/ff:a=<bild-a>.png,b=<bild-b>.png
```

Die Mustermessungen liegen als `build/hardware_captures/muster_<gerät>/<muster>/` vor (mit `test.prg`), die Messsätze unter `build/hardware_captures/suite/<gerät>/`. Für die Vergleiche der Pixel-Varianten wurde Emu64 vorübergehend mit `getenv()`-Schaltern gebaut (Setzen/Löschen-Pixel, ECM-Verzögerung), diese sind im Code nicht mehr enthalten.
