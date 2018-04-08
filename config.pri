CONFIG += qt \
    warn_on
DESTDIR = ../bin
MOC_DIR = ../moc
OBJECTS_DIR = ../obj
UI_DIR = ../ui
QMAKE_LFLAGS += " -Wl,--no-undefined"

win32 {
    DEFINES += BUILD_SHARED_WIN
    QMAKE_LFLAGS += " -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc"

    BASE = D:/Documenti-Mocciola/tmp
    # BASE = C:

    EIGEN_INCLUDE = "$${BASE}/eigen/3.2.1"
}

unix {
    EIGEN_INCLUDE = /usr/include/eigen3
}

# DEFINES += BUILD_STATIC
