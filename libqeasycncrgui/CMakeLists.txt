cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

set( SRC
    qeasycncrgui.cpp
    simplesectionrcncrrectgui.cpp
    simplesectionrcncrcompgui.cpp
    simplesectionrcncrfrpgui.cpp
)

set( HDR
    qeasycncrgui.h
    simplesectionrcncrrectgui.h
    simplesectionrcncrcompgui.h
    simplesectionrcncrfrpgui.h
)

set ( FRM
    qeasycncrgui.ui
    simplesectionrcncrrectgui.ui
    simplesectionrcncrcompgui.ui
    simplesectionrcncrfrpgui.ui
)

# Tell CMake to create the library
add_library( qeasycncrgui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqeasycncr
    ../libsimpledataplotter
    ../libqstructutility
    ../libvarplusgui
    ../libvarplus
)

# Use required libraries
target_link_libraries(
    qeasycncrgui
    qeasycncr simpledataplotter Qt5::Widgets
)

