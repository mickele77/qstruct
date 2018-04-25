cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    material.cpp
    concrete.cpp
    steel.cpp
    steelcncr.cpp
    timber.cpp
    frp.cpp
    materialmodel.cpp
)

set( HDR
    material.h
    concrete.h
    steel.h
    steelcncr.h
    timber.h
    frp.h
    materialmodel.h
    material_export.h
)

# Tell CMake to create the library
add_library( libqmaterial
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries(libqmaterial libvarplus Qt5::Core)

