cmake_minimum_required(VERSION 2.8.11)

OPTION (BUILD_SHARED_LIBS "Build shared libraries." ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic automatically when needed.
set(CMAKE_AUTOUIC ON)
# Instruct CMake to run rcc automatically when needed.
set(CMAKE_AUTORCC ON)

project(qstruct)

add_subdirectory( libglviewer )
add_subdirectory( libsimpledataplotter )
add_subdirectory( libqcustomplot )
add_subdirectory( libqstructutility )
add_subdirectory( libvarplus )
add_subdirectory( libqload )
add_subdirectory( libqmaterial )
add_subdirectory( libqsection )
add_subdirectory( libqtimberconnection )
add_subdirectory( libqsteelconnection )
add_subdirectory( libqfem )
add_subdirectory( libqsoil )
add_subdirectory( libqeasy )
add_subdirectory( libqeasycncr )
add_subdirectory( libvarplusgui )
add_subdirectory( libqmaterialgui )
add_subdirectory( libqsoilgui )
add_subdirectory( libqsectiongui )
add_subdirectory( libqtimberconnectiongui )
add_subdirectory( libqfemgui )
add_subdirectory( libqfem2dgui )
add_subdirectory( libqeasygui )
add_subdirectory( libqeasycncrgui )
add_subdirectory( libqstructure )
add_subdirectory( qstruct )
