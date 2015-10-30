TEMPLATE	= lib
CONFIG		+= warn_on
CONFIG    -= qt
CONFIG    -= windows

# CONFIG    -= debug  # RELEASE is active
CONFIG    -= release  # DEBUG is active

CONFIG    += staticlib

INCLUDEPATH = ./Header

HEADERS		+=  ./Header/sstRec03Lib.h
HEADERS		+= sstRec03LibInt.h

SOURCES		+=  sstRec03.cpp \
    sstRec03TestIntern.cpp \
    sstRec03TestRec.cpp \
    sstRec03Header.cpp \
    sstRec03CargoMem.cpp
    #sstRec02Cargo.cpp
SOURCES		+=  sstRec03CargoKey.cpp
SOURCES		+=  sstRec03CargoKeyInt.cpp
SOURCES		+=  sstRec03Int.cpp
SOURCES		+=  sstRec03Vector.cpp

OTHER_FILES += README.md

debug{
  TARGET		= sst_rec03_lib_d
}
release{
  TARGET		= sst_rec03_lib_r
}

DESTDIR     = ../libs

