cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

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
add_library( libqeasygui
    ${SRC} ${HDR} ${FRM}
)

include_directories(
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries(libvarplusgui libvarplus Qt5::Widgets Qt5::Gui)

