# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: emu64_project.pro                     //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 07.08.2019                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

TEMPLATE = subdirs

  SUBDIRS = emu64

  emu64.subdir = src

  CONFIG += ordered

# Example for more subdirs
# SUBDIRS = src1\
#            src2
#
# src1.subdir = src1
# src2.subdir = src2
#
# app.depends = src1 src2
