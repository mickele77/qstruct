IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_GLVIEWER_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set( QRC
    shaders.qrc
    glviewer-icons.qrc
)

set( SRC
    glwidget.cpp
    glwindow.cpp
    glitem.cpp
    glgroup.cpp
    glline.cpp
    glpoint.cpp
    gltriangle.cpp
    gltext2d.cpp
)

set( HDR
    glwidget.h
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
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

