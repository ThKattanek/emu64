#ifndef DOS_ERROR_MESSAGES_H
#define DOS_ERROR_MESSAGES_H

#include <QString>

#define DOS_ERROR_COUNT 34
const uint8_t dos_error_numbers[DOS_ERROR_COUNT] = {0,1,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,39,50,51,52,60,61,62,63,64,65,66,67,70,71,72,73,74};

QString dos_error_msg[DOS_ERROR_COUNT] =
{
{"00 OK (alles in Ordnung):<br>"
"Diese Meldung wird vom DOS immer dann geschickt, wenn ein Befehl einwandfrei"
" und fehlerlos ausgeführt werden konnte."},

{"01 FILES SCRATCHED (keine Fehlermeldung):<br>"
"Diese Meldung wird nach jedem SCRATCH-Befehl ausgegeben und gibt in einer"
" weiteren Nummer die Anzahl der geSCRATCHten Files an. Es handelt sich hier"
" um eine Kontrollmeldung."},

{"20 READ ERROR (Blockheader nicht gefunden):<br>"
"Der Header eines Datenblocks konnte vom Disk-Controller nicht ausfindig"
" gemacht werden. Das kann bei der Angabe einer ungültigen Sektornummer oder"
" bei einem zerstörten Blockheader passieren."},

{"21 READ ERROR (SYNC-Markierung nicht rechtzeitig gefunden):<br>"
"Es konnte innerhalb der Toleranzzeit keine SYNC-Markierung auf dem "
"gewünschten Track gefunden werden. Dieser Fehler tritt bei einer defekten "
"Diskette oder einem dejustierten Tonkopf bevorzugt auf. Auch eine nicht "
"formatierte Diskette kann für diesen Fehler verantwortlich sein."},

{"22 READ ERROR (Datenblock nicht gefunden):<br>"
"Bei dieser Meldung konnte der Datenblock hinter dein Header nicht"
" identifiziert werden. Hier handelt es sich zumeist um eine Fehlformatierung"
" des entsprechenden Blocks, wobei das Datenblockkennzeichen nicht mit dem"
" Wert in Speicherstelle $38 der Zeropage übereinstimmt."},

{"23 READ ERROR (Prüfsummenfehler im Datenblock):<br>"
"Hier stimmt die Prüfsumme über den Datenblock nicht mit den gelesenen Werten"
" überein. Der Block konnte zwar in den DOS-Puffer gelesen werden; es besteht"
" jedoch die Gefahr einer defekten Diskette. Auch Erdungsprobleme der Floppy"
" können durch diese Fehlermeldung angezeigt werden."},

{"24 READ ERROR (Fehler bei der GCR-Recodierung):<br>"
"Beim Recodieren des Datenblocks sind ungültige Werte aufgetreten, die die"
" Leseelektronik nicht einwandfrei verarbeiten kann (mehr als 9 '1'-Bits oder"
" mehr als 2 \"0\"-Bits). Auch diese Fehlermeldung kann Erdungsprobleme"
" anzeigen."},

{"25 WRITE ERROR (Fehler beim Verifizieren):<br>"
" Bei der Vergleichskontrolle eines eben geschriebenen Blocks mit dem"
" Pufferinhalt wurde ein Fehler entdeckt. Diese Meldung ist mit großer"
" Wahrscheinlichkeit auf eine defekte Diskette zurückzuführen."},

{"26 WRITE PROTECT ON:<br>"
"Hier wurde versucht, auf eine Diskette zu schreiben, die eine Schreibschutzplakette trägt."},

{"27 READ ERROR (Prüfsummenfehler im Blockheader):<br>"
"Es wurde bei der Überprüfung der Headerprüfsumme ein Fehler entdeckt. Hier"
" handelt es sich sehr wahrscheinlich um eine defekte Diskette oder um"
" Erdungsprobleme."},

{"28 WRITE ERROR (zu langer Block):<br>"
"Hier wurde nach dem Schreiben eines Datenblocks in einer festgelegten Zeit"
" nicht die SYNC-Markierung des nächsten Blockheaders gefunden. Diese Meldung"
" zeigt eine fehlformatierte Diskette (Datenblock hat folgende SYNC-Markierung"
" überschrieben) oder einen Hardware-Defekt an."},

{"29 DISK ID MISMATCH:<br>"
"Es wurde auf eine Diskette zugegriffen, die vorher nicht initalisiert worden"
" ist. Bei dieser Fehlermeldung kann es sich aber auch um die Folge eines"
" zerstörten Blockheaders handeln."},

{"30 SYNTAX ERROR (allgemeiner Syntaxfehler):<br>"
"Hier konnte ein gesendetes Kommando nicht interpretiert werden, weil"
" entweder die Parameterübergabe falsch war oder das gegebene Kommando nicht"
" exisitiert."},

{"31 SYNTAX ERROR (ungültiger Befehl):<br>"
"Das DOS war hier nicht in der Lage, einen Befehl zu erkennen. Das kann unter"
" Umständen auf Leerzeichen vor dem Kommandowort hindeuten (Befehl muß immer"
" an erster Position stehen)."},

{"32 SYNTAX ERROR (Befehlszeile zu lang):<br>"
"Hier wurde eine Zeile mit einer Länge von mehr als 58 Zeichen zur Floppy"
" geschickt."},

{"33 SYNTAX ERROR (unerlaubte Verwendung eines 'Jokers'):<br>"
"Es wurde ein Joker in einem Filenamen verwendet, obwohl das für den"
" gewünschten Befehl nicht zulässig ist."},

{"34 SYNTAX ERROR (Filename nicht gefunden):<br>"
"Das DOS konnte in der Eingabezeile keinen Filenamen finden. Hierfür ist oft"
" das Fehlen eines Doppelpunkts ':' verantwortlich."},

{"39 SYNTAX ERROR (ungültiger Befehl):<br>"
"Dieser Fehler kann auftreten, wenn ein Befehl, der über den Kommandokanal"
" geschickt worden ist, vom DOS nicht interpretiert werden konnte."},

{"50 RECORD NOT PRESENT:<br>"
"Diese Fehlermeldung zeigt an, daß in einer relativen Datei über den letzten"
" Record hinaus positioniert worden ist. Ist der nächste Dateizugriff ein"
" Schreibzugriff, kann diese Meldung ignoriert werden, da eine Erweiterung"
" automatisch stattfindet."
" Eine weitere Funktion dieser Meldung zeigt sich bei den &-Files. Hier wird"
" auf eine verkehrte Prüf summe hingewiesen."},

{"51 OVERFLOW IN RECORD:<br>"
"Hier wurde der Versuch unternommen, in einen Datensatz mehr als die"
" zulässige Zeichenanzahl hineinzuschreiben."
" Bei der Behandlung von &-Files bedeutet diese Meldung eine falsche Angabe"
" über die Anzahl der Bytes im nächsten Abschnitt."},

{"52 FILE TOO LARGE:<br>"
"Eine Positionierung innerhalb einer relativen Datei zu deren Erweiterung ist"
" nicht mehr möglich, da die Diskette voll ist."},

{"60 WRITE FILE OPEN:<br>"
"Diese Fehlermeldung erscheint, wenn ein Lesezugriff auf ein nicht ordnungsgemäß geschlossenes File stattfindet."},

{"61 FILE NOT OPEN:<br>"
"Es wird auf ein File zugegriffen, das vorher vom DOS nicht geöffnet worden war."},

{"62 FILE NOT FOUND:<br>"
"Das zum Lesen angeforderte File ist auf dem aktuellen Laufwerk nicht vorhanden."},

{"63 FILE EXISTS:<br>"
"Es soll ein File zum Schreiben geöffnet werden, das bereits auf Diskette"
" existiert."},

{"64 FILE TYPE MISMATCH:<br>"
"Der angeforderte Filetyp stimmt nicht mit dem Filetyp im Directory überein."},

{"65 NO BLOCK:<br>"
"Diese Fehlermeldung zeigt an, daß ein Block mit dem B-A-Befehl belegt werden"
" sollte, der bereits belegt ist. Die Track- und Sektornummer geben den"
" nächsten freien Block an und sind 0, wenn alle höheren Blöcke bereits belegt"
" sind."},

{"66 ILLEGAL TRACK OR SECTOR:<br>"
"Hier wurde versucht, auf einen ungültigen Track oder einen ungültigen Sektor"
" zuzugreifen."},

{"67 ILLEGAL TRACK OR SECTOR:<br>"
"Hier zeigte der Linker eines Datenblocks auf eine ungültige Spur- oder"
" Sektornummer."},

{"70 NO CHANNEL:<br>"
"Hier sind entweder alle Kanäle belegt, oder es wurde versucht, einen schon"
" belegten Kanal zu reservieren."},

{"71 DIR ERROR:<br>"
"Hier stimmt das Verhältnis zwischen der Angabe der gesamten freien Blöcke"
" und der Summe der freien Blöcke jedes Tracks in der BAM nicht. Dieser Fehler"
" zeigt eine zerstörte BAM an."},

{"72 DISK FULL:<br>"
"Diese Fehlermeldung erscheint entweder, wenn alle Blöcke der Diskette belegt"
" sind, oder wenn das Directory voll ist (144 Einträge bei der 1541)."},

{"73 CBM DOS V2.6 1541 (Einschalt- oder Fehlermeldung):<br>"
"Es wurde versucht, mit einer Diskette anderen Formats zu arbeiten, oder die"
" Floppy wurde soeben neu eingeschaltet."},

{"74 DRIVE NOT READY:<br>"
"Es wurde versucht, auf ein Laufwerk zuzugreifen, in dem sich keine Diskette"
" befindet."},
};

#endif // DOS_ERROR_MESSAGES_H
