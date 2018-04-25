cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Widgets Gui Core PrintSupport REQUIRED)

set( SRC
    simpledataplotter.cpp \
    simpledataplotterdialog.cpp
)

set( HDR
    simpledataplotter.h \
    simpledataplotterdialog.h
)

set( FRM
    simpledataplotterdialog.ui
)

set( QRC
    simpledataplottericons.qrc
)

# Tell CMake to create the library
add_library( libsimpledataplotter
    ${SRC} ${HDR} ${FRM} ${QRC}
)

# Use required libraries
target_link_libraries(libsimpledataplotter Qt5::Widgets Qt5::Gui Qt5::Core Qt5::PrintSupport )

