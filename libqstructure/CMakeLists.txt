# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_STRUCTURE_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( QRC
    icons.qrc
)

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
add_library( qstructure
    ${SRC} ${HDR} ${QRC}
)

include_directories(
    ../libqstructutility
    ../libvarplus
    ../libqload
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
    ../libglviewer
)

# Use required libraries
target_link_libraries(
    qstructure
    qeasy
    qeasygui
    qeasycncr
    qeasycncrgui
    qmaterial
    qmaterialgui
    qsection
    qsectiongui
    qsoil
    qsoilgui
    qfem
    qfemgui
    qfem2dgui
    qtimberconnection
    qtimberconnectiongui
    glviewer
    Qt5::Core
)
