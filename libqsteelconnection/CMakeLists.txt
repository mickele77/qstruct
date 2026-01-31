# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    steelbolt.cpp
    steelboltconnection.cpp
)

set( HDR
    steelbolt.h
    steelboltconnection.h
)

# Tell CMake to create the library
add_library( qsteelconnection
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
    ../libvarplus
    ../libqmaterial
)

# Use required libraries
target_link_libraries(
    qsteelconnection
    qmaterial
)

