cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( SRC
    timberconnectionmodelgui.cpp
    timberconnectiongui.cpp
)

set( HDR
    timberconnectionmodelgui.h
    timberconnectiongui.h
)

set ( FRM
    timberconnectionmodelgui.ui
    timberconnectiongui.ui
)

# Tell CMake to create the library
add_library( qtimberconnectiongui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqtimberconnection
    ../libqsection
    ../libqmaterial
    ../libqload
    ../libvarplusgui
    ../libvarplus
    ../libqstructutility
)

# Use required libraries
target_link_libraries( qtimberconnectiongui
    qtimberconnection
    varplusgui
    Qt5::Widgets
)
