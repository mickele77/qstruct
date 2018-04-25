cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

set( SRC
    vertexmodelgui.cpp
    elementmodelgui.cpp
    beamgui.cpp
    elementgui.cpp
    elementvertexdelegate.cpp
    beamqdelegate.cpp
    beamfdelegate.cpp
    calcresultsgui.cpp
    beamstressgui.cpp
    beamdeformationgui.cpp
)

set( HDR
    vertexmodelgui.h
    elementmodelgui.h
    beamgui.h
    elementgui.h
    elementvertexdelegate.h
    beamqdelegate.h
    beamfdelegate.h
    calcresultsgui.h
    beamstressgui.h
    beamdeformationgui.h
)

set( FRM
    vertexmodelgui.ui
    elementmodelgui.ui
    calcresultsgui.ui
)

# Tell CMake to create the library
add_library( qfemgui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqfem
    ../libqsection
    ../libqmaterial
    ../libqcustomplot
    ../libvarplusgui
    ../libvarplus
    ../libqstructutility
)

# Use required libraries
target_link_libraries( qfemgui qfem Qt5::Widgets Qt5::Gui)
