# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_EASY_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    loadresultant.cpp
    bondedanchor.cpp
)

set( HDR
    easy_export.h
    loadresultant.h
    bondedanchor.h
)

# Tell CMake to create the library
add_library( qeasy
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries(
    qeasy
    varplus qstructutility Qt5::Core
)

