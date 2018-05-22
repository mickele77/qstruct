cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Sql Widgets REQUIRED)

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
add_library( qmaterialgui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libvarplus
    ../libvarplusgui
    ../libqcustomplot
    ../libqload
    ../libqmaterial
    ../libqsection
)

# Use required libraries
target_link_libraries(
    qmaterialgui
    qmaterial Qt5::Sql Qt5::Widgets
)
