cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Sql Widgets REQUIRED)

set( SRC
    sectiongeometricdatapanel.cpp
    sectiongui.cpp
    sectiondefaultgui.cpp
    sectionrcncrgui.cpp
    sectionsteelgui.cpp
    sectionclassdiagramgui.cpp
    sectiontimbergui.cpp
    sectionthingui.cpp
    sectionthincontgui.cpp
    sectionthincontsteelgui.cpp
    sectiontransformpanel.cpp
    sectionmodelgui.cpp
    sectionmaterialdelegate.cpp
    sectionprofilegui.cpp
    sectionprofiledatapanel.cpp
    sectionpointsgui.cpp
    multisectiongui.cpp
)

set( HDR
    sectiongeometricdatapanel.h
    sectiongui.h
    sectiondefaultgui.h
    sectionrcncrgui.h
    sectionsteelgui.h
    sectionclassdiagramgui.h
    sectiontimbergui.h
    sectionthingui.h
    sectionthincontgui.h
    sectionthincontsteelgui.h
    sectiontransformpanel.h
    sectionmodelgui.h
    sectionmaterialdelegate.h
    sectionprofilegui.h
    sectionprofiledatapanel.h
    sectionpointsgui.h
    multisectiongui.h
)

set ( FRM
    sectiongeometricdatapanel.ui
    sectiondefaultgui.ui
    sectionrcncrgui.ui
    sectionsteelgui.ui
    sectionclassdiagramgui.ui
    sectiontimbergui.ui
    sectionthingui.ui
    sectionthincontgui.ui
    sectionthincontsteelgui.ui
    sectiontransformpanel.ui
    sectionmodelgui.ui
    sectionprofilegui.ui
    sectionprofiledatapanel.ui
    sectionpointsgui.ui
    multisectiongui.ui
)

# Tell CMake to create the library
add_library( qsectiongui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqstructutility
    ../libvarplus
    ../libvarplusgui
    ../libqcustomplot
    ../libqload
    ../libqmaterial
    ../libqsection
    ../libqfem
)

# Use required libraries
target_link_libraries(
    qsectiongui
    qfem
    qsection
    varplusgui
    qcustomplot
    Qt5::Sql Qt5::Widgets
)
