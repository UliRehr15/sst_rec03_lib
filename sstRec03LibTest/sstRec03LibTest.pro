TEMPLATE	  = app
CONFIG	 	 += warn_on
CONFIG       -= qt
CONFIG       -= windows

# CONFIG       -= debug  # RELEASE is active
CONFIG       -= release  # DEBUG is active

INCLUDEPATH +=  ../Header

debug{
  LIBS        += ../../libs/libsst_rec03_lib_d.a
}
release{
  
  win32-g++:QMAKE_LFLAGS += -static
  win32-g++:QMAKE_LFLAGS -= -mthreads
  win32-g++:QMAKE_LFLAGS_EXCEPTIONS_ON -= -mthreads
  win32-g++:QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -mthreads
  
  
  LIBS        += ../../libs/libsst_rec03_lib_r.a
}

HEADERS    +=   sstRec03LibTest.h

SOURCES    +=   sstRec03LibTest.cpp

TARGET	  	= sst_rec03_lib_test

