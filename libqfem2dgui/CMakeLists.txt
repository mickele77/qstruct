cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS OpenGl Xml Widgets Gui REQUIRED)

set( SRC
    qfem2dgui.cpp
    femstructureviewer.cpp
)

set( HDR
    qfem2dgui.h
    femstructureviewer.h
)

# Tell CMake to create the library
add_library( libqeasygui
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
target_link_libraries(libqfem2d libqfemgui libqfem Qt5::Widgets Qt5::Gui)

