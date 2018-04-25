cmake_minimum_required(VERSION 2.8.11)

# Find the required modules
find_package(Qt5 COMPONENTS Core Gui REQUIRED)

set( QRC
    shaders.qrc
    glviewer-icons.qrc
)

set( SRC
    glwindow.cpp
    glitem.cpp
    glgroup.cpp
    glline.cpp
    glpoint.cpp
    gltriangle.cpp
    gltext2d.cpp
)

set( HDR
    glwindow.h
    glitem.h
    glgroup.h
    glline.h
    glpoint.h
    gltriangle.h
    gltext2d.h
)

# Tell CMake to create the library
add_library( glviewer
    ${SRC} ${HDR} ${QRC}
)

# Use required modules from Qt 5
target_link_libraries(
    glviewer
    Qt5::Core Qt5::Gui
)

