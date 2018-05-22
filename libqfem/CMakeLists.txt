cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

find_package (Eigen3 REQUIRED)

set( SRC
    hypothesis.cpp
    hypothesis2d.cpp
    hypothesis3d.cpp
    vertex.cpp
    vertexmodel.cpp
    element.cpp
    elementmodel.cpp
    elementvertexmodel.cpp
    beam.cpp
    femstructure.cpp
    ssbeam.cpp
    ssbeamtimber.cpp
    beamqmodel.cpp
    beamq.cpp
    beamfmodel.cpp
    beamf.cpp
)

set( HDR
    hypothesis.h
    hypothesis2d.h
    hypothesis3d.h
    vertex.h
    vertexmodel.h
    element.h
    elementmodel.h
    elementvertexmodel.h
    beam.h
    femstructure.h
    ssbeam.h
    ssbeamtimber.h
    beamqmodel.h
    beamq.h
    beamfmodel.h
    beamf.h)

# Tell CMake to create the library
add_library( qfem
    ${SRC} ${HDR}
)

include_directories(
    ${EIGEN3_INCLUDE_DIRS}
    ../libglviewer
    ../libqstructutility
    ../libvarplus
    ../libqload
    ../libqmaterial
    ../libqsection
)

# Use required libraries
target_link_libraries(
    qfem
    qsection qload glviewer Eigen3::Eigen
)

