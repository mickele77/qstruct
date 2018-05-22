cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_EASYCNCR_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    simplesectionrcncrrect.cpp
    simplesectionrcncrcomp.cpp
    steelamodel.cpp
    steelfepsmodel.cpp
    slssteelsigmamodel.cpp
    cncrrectmodel.cpp
)

set( HDR
    simplesectionrcncrrect.h
    simplesectionrcncrcomp.h
    steelamodel.h
    steelfepsmodel.h
    slssteelsigmamodel.h
    cncrrectmodel.h
    easycncr_export.h
)

# Tell CMake to create the library
add_library( qeasycncr
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries(
    qeasycncr
    varplus qstructutility Qt5::Core
)
