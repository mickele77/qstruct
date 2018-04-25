cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

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
add_library( libqtimberconnectiongui
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
target_link_libraries(libqtimberconnectiongui libqeasy libsimpledataplotter Qt5::Widgets Qt5::Gui)
