cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_QSTRUCTUTILITY_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( SRC
    structmath.cpp
    convertutility.cpp
    qgraphicspointitem.cpp
    qgraphicsitemviewer.cpp
    functionzeros.cpp
    point2dmodel.cpp
)

set( HDR
    qstructutility_export.h
    structmath.h
    convertutility.h
    eigen-core.h
    qgraphicspointitem.h
    qgraphicsitemviewer.h
    functionzeros.h
    point2dmodel.h
)

# Tell CMake to create the library
add_library( qstructutility
    ${SRC} ${HDR}
)

# Use required libraries
target_link_libraries( qstructutility Qt5::Widgets )

