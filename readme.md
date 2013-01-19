Es wird ein Makefile mittels qmake erstellt:

    >> qmake -o Makefile emu64_projekt.pro

Nun kann das Projekt mittels make kompiliert werden

    >> make

Was gehört in den Emu64 Arbeitsordner:

- ausführbare emu64 Datei für das jeweilige System
- 1 Verzeichnis "languages" mit den Sprachdateien qt_de.qm / qt_en.qm / emu64_de.qm / emu64_en.qm
                            und dann noch die Bilder aus dem Verzeichnis grafik/flaggen/
                            Sollten noch mehr Sprachen dazu kommen, jeweils die qt_*.qm / emu64_*.qm und emu64_*.png
                            mit rein packen.

- 1 Verzeichnis "roms" welcher einfach aus dem emu64_source Verzeichnis kopiert werden kann
- 1 Verzeichnis "floppy_sounds" welcher auch einfach aus dem emu64_source Verzeichnis kopiert werden kann
- 1 Verzeichnis "gfx" welches folgende Dateien aus dem grafik Verzeichnis enthalten muss
                            kreis0.png / kreis1.png / pfeil0.png / pfeil1.png / sdl_icon.png
