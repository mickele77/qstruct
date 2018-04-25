cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

set( SRC
    loadresultantgui.cpp
    qeasygui.cpp
    bondedanchorgui.cpp
)

set( HDR
    loadresultantgui.h
    qeasygui.h
    bondedanchorgui.h
)

set ( FRM
    loadresultantgui.ui
    qeasygui.ui
    bondedanchorgui.ui
)

# Tell CMake to create the library
add_library( libqeasygui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqeasy
    ../libsimpledataplotter
    ../libqstructutility
    ../libvarplusgui
    ../libvarplus
)

# Use required libraries
target_link_libraries(libqeasygui libqeasy libsimpledataplotter Qt5::Widgets Qt5::Gui)
