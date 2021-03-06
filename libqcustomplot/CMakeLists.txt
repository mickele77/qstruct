cmake_minimum_required(VERSION 2.8.11)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_QCUSTOMPLOT_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

set( SRC
    qcustomplot.cpp
    qcustomplotdialog.cpp
)

set( HDR
    qcustomplot.h
    qcustomplotdialog.h
    qcustomplot_export.h
)

set( FRM
    qcustomplotdialog.ui
)

# Tell CMake to create the library
add_library( qcustomplot
    ${SRC} ${HDR} ${FRM}
)

# Use required modules from Qt 5
target_link_libraries( qcustomplot Qt5::Widgets Qt5::PrintSupport)

