cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_TIMBERCONNECTION_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    timberconnection.cpp
    timberconnector.cpp
    timberconnectionmodel.cpp
)

set( HDR
    timberconnection.h
    timberconnector.h
    timberconnectionmodel.h
    timberconnection_export.h
)

# Tell CMake to create the library
add_library( qtimberconnection
    ${SRC} ${HDR}
)

include_directories(
    ../libqsection
    ../libqmaterial
    ../libqload
    ../libvarplus
    ../libqstructutility
)

# Use required libraries
target_link_libraries(
    qtimberconnection
    qsection
)

