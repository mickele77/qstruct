cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS OpenGL Widgets REQUIRED)

set( SRC
    qfem2dgui.cpp
    femstructureviewer.cpp
)

set( HDR
    qfem2dgui.h
    femstructureviewer.h
)

# Tell CMake to create the library
add_library( qfem2dgui
    ${SRC} ${HDR}
)

include_directories(
    ../libqfemgui
    ../libqfem
    ../libglviewer
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries(
    qfem2dgui
    qfemgui qfem Qt5::OpenGL Qt5::Widgets
)
