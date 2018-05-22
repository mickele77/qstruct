cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( SRC
    soilmodelgui.cpp
    soilgui.cpp
    retainingwallgui.cpp
    retainingwallmodelgui.cpp
    retainingwallqscombinationgui.cpp
    retainingwalleqqscombinationgui.cpp
    retainingwallguiprivate.cpp
    soilsheetgui.cpp
    soilsheetmodelgui.cpp
)

set( HDR
    soilmodelgui.h
    soilgui.h
    retainingwallgui.h
    retainingwallmodelgui.h
    retainingwallqscombinationgui.h
    retainingwalleqqscombinationgui.h
    retainingwallqscombinationguiprivate.h
    retainingwallguiprivate.h
    soilsheetgui.h
    soilsheetmodelgui.h
)

set( FRM
    soilmodelgui.ui
    soilgui.ui
    retainingwallgui.ui
    retainingwallmodelgui.ui
    retainingwallqscombinationgui.ui
    soilsheetgui.ui
    soilsheetmodelgui.ui
)

# Tell CMake to create the library
add_library( qsoilgui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqstructutility
    ../libvarplus
    ../libvarplusgui
    ../libqsoil
)

# Use required libraries
target_link_libraries(
    qsoilgui
    qsoil varplusgui
)
