cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    soil.cpp
    soilmodel.cpp
    retainingwall.cpp
    retainingwallmodel.cpp
    soilsheet.cpp
    soilsheetmodel.cpp
)

set( HDR
    soil.h
    soilmodel.h
    retainingwall.h
    retainingwallmodel.h
    soilsheet.h
    soilsheetmodel.h
)

# Tell CMake to create the library
add_library( qsoil
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
    ../libvarplus
    ../libqsection
)

# Use required libraries
target_link_libraries(
    qsoil
    qsection
)

