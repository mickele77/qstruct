cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    structmodule.cpp
    materialmodule.cpp
    sectionmodule.cpp
    fem2dmodule.cpp
    timberconnectionmodule.cpp
    soilmodule.cpp
    retainingwallmodule.cpp
    qstructuregui.cpp
    qstructure.cpp
    easymodule.cpp
    easycncrmodule.cpp
    steelconnectionmodule.cpp
    soilsheetmodule.cpp
)

set( HDR
    structmodule.h
    materialmodule.h
    sectionmodule.h
    fem2dmodule.h
    timberconnectionmodule.h
    soilmodule.h
    retainingwallmodule.h
    qstructuregui.h
    qstructure.h
    easymodule.h
    easycncrmodule.h
    steelconnectionmodule.h
    soilsheetmodule.h
)

# Tell CMake to create the library
add_library( libqstructure
    ${SRC} ${HDR}
)

include_directories(
    ../libqmaterial
    ../libqmaterialgui
    ../libqsection
    ../libqsectiongui
    ../libqsoil
    ../libqsoilgui
    ../libqfem
    ../libqfemgui
    ../libqfem2dgui
    ../libqtimberconnection
    ../libqtimberconnectiongui
    ../libqeasycncr
    ../libqeasycncrgui
    ../libqeasy
    ../libqeasygui
)

# Use required libraries
target_link_libraries(
    libqstructure
    libqsectiongui
    libqmaterialgui
    libqsoilgui
    liqfem2dgui
    libqtimberconnectiongui
    libqeasycncrgui
    libqeasygui
)

