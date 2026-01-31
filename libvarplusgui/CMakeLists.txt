# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( SRC
    qwidgetplus.cpp
    qlabelunitmeasure.cpp
    qlabelrichname.cpp
    qlineeditvalue.cpp
    qcheckboxreadonly.cpp
    qcheckboxbool.cpp
    varplusguiutility.cpp
    qwidgetenumplus.cpp
    qcomboboxenum.cpp
    qradiobuttonenum.cpp
)

set( HDR
    qwidgetplus.h
    qlabelunitmeasure.h
    qlabelrichname.h
    qlineeditvalue.h
    qcheckboxreadonly.h
    qcheckboxbool.h
    varplusguiutility.h
    qwidgetenumplus.h
    qcomboboxenum.h
    qradiobuttonenum.h
)

# Tell CMake to create the library
add_library( varplusgui
    ${SRC} ${HDR} ${FRM}
)

if ( BUILD_SHARED_LIBS )
    install( TARGETS varplusgui DESTINATION varplusgui )
endif()

include_directories(
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries( varplusgui varplus Qt5::Widgets)
