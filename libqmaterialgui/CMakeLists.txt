cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

set( SRC
    materialgui.cpp
    materialdefaultgui.cpp
    concretegui.cpp
    steelgui.cpp
    steelcncrgui.cpp
    timbergui.cpp
    frpgui.cpp
    existmasonrygui.cpp
    changerichnamedialog.cpp
    materialmodelgui.cpp
)

set( HDR
    materialgui.h
    materialdefaultgui.h
    concretegui.h
    steelcncrgui.h
    steelgui.h
    timbergui.h
    frpgui.h
    existmasonrygui.h
    changerichnamedialog.h
    materialmodelgui.h
)

set ( FRM
    materialdefaultgui.ui
    concretegui.ui
    steelcncrgui.ui
    steelgui.ui
    timbergui.ui
    existmasonrygui.ui
    changerichnamedialog.ui
    materialmodelgui.ui
    frpgui.ui
)

# Tell CMake to create the library
add_library( libqeasygui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libvarplus
    ../libvarplusgui
    ../libqcustomplot
    ../libqmaterial
    ../libqsection
)

# Use required libraries
target_link_libraries(libqmaterialgui libqmaterial Qt5::Widgets Qt5::Gui)

