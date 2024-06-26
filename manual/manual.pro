# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: manual.pro                            //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 26.06.2024                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

DISTFILES += \
	images/* \
	locale/attributes-de.adoc \
	*.adoc

system(asciidoctor index.adoc)
