# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    load.cpp
    loadtablemodel.cpp
    loadcase.cpp
    loaddurationclass.cpp
    loadgroup.cpp
    loadcompl.cpp
    loadcompltablemodel.cpp
    serviceclass.cpp
    lstype.cpp
    loadtype.cpp
    loadcompltabledelegate.cpp
)

set( HDR
    load.h
    loadtablemodel.h
    loaddurationclass.h
    loadcase.h
    loadgroup.h
    loadcompl.h
    loadcompltablemodel.h
    serviceclass.h
    lstype.h
    loadtype.h
    loadcompltabledelegate.h
    load_export.h
)

# Tell CMake to create the library
add_library( qload
    ${SRC} ${HDR}
)

include_directories(
    ../libvarplus
)

# Use required libraries
target_link_libraries(
    qload
    varplus Qt5::Core
)

