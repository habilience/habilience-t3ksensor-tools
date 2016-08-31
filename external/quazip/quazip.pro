CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

TEMPLATE = lib
CONFIG += qt warn_on
QT -= gui
DEPENDPATH += .
INCLUDEPATH += .

DEFINES += QUAZIP_BUILD
CONFIG += static staticlib
CONFIG(staticlib): DEFINES += QUAZIP_STATIC

QMAKE_CFLAGS_RELEASE = -Os
QMAKE_LFLAGS += -static -flto

#################################################################################################
## linux-g++
linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64

linux-g++ { # depend on Qt Creator's setting
    contains(QMAKE_TARGET.arch, x86_64):{
        message( "building for 64bit" );
        TARGET = $$join(TARGET,,,_x64)
    }
    !contains(QMAKE_TARGET.arch, x86_64):{
        message( "building for 32bit" );
    }
}

linux-g++-32 { # generic g++ 32bit compiler
    message( "building for 32bit" );
}
linux-g++-64 { # generic g++ 64bit compiler
    message( "building for 64bit" );
    TARGET = $$join(TARGET,,,_x64)
}
##
#################################################################################################

# Input
HEADERS += \
    crypt.h\
    ioapi.h\
    JlCompress.h\
    quaadler32.h\
    quachecksum32.h\
    quacrc32.h\
    quazip.h\
    quazipfile.h\
    quazipfileinfo.h\
    quazipnewinfo.h\
    quazip_global.h\
    unzip.h\
    zip.h\

SOURCES += *.c *.cpp

unix:!symbian {
    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target

    contains(QMAKE_TARGET.arch, x86_64):{
        CONFIG(debug, debug|release): OBJECTS_DIR=.obj_x64/debug
        CONFIG(debug, debug|release): MOC_DIR=.moc_x64/debug
        CONFIG(release, debug|release): OBJECTS_DIR=.obj_x64/release
        CONFIG(release, debug|release): MOC_DIR=.moc_x64/release
    }

    !contains(QMAKE_TARGET.arch, x86_64):{
        CONFIG(debug, debug|release): OBJECTS_DIR=.obj/debug
        CONFIG(debug, debug|release): MOC_DIR=.moc/debug
        CONFIG(release, debug|release): OBJECTS_DIR=.obj/release
        CONFIG(release, debug|release): MOC_DIR=.moc/release
    }

    #LIBS += -lz
}

win32 {
    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target

    #*-g++*: LIBS += -lz.dll
    *-msvc*: LIBS += -lzlibwapi
    *-msvc*: QMAKE_LFLAGS += /IMPLIB:$$DESTDIR\\quazip.lib
}


symbian {

    # Note, on Symbian you may run into troubles with LGPL.
    # The point is, if your application uses some version of QuaZip,
    # and a newer binary compatible version of QuaZip is released, then
    # the users of your application must be able to relink it with the
    # new QuaZip version. For example, to take advantage of some QuaZip
    # bug fixes.

    # This is probably best achieved by building QuaZip as a static
    # library and providing linkable object files of your application,
    # so users can relink it.

    CONFIG += staticlib
    CONFIG += debug_and_release

    LIBS += -lezip

    #Export headers to SDK Epoc32/include directory
    exportheaders.sources = $$HEADERS
    exportheaders.path = quazip
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$exportheaders.path/$$basename(header)"
    }
}
