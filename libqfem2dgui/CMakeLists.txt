if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required modules
find_package(Qt5 COMPONENTS OpenGL Widgets REQUIRED)

file(GLOB FRM *.ui)
file(GLOB QRC *.qrc)
file(GLOB HDR *.h)
file(GLOB SRC *.cpp)

# Tell CMake to create the library
add_library( qfem2dgui
    ${SRC} ${HDR} ${QRC} ${FRM}
)

include_directories(
    ../libqfemgui
    ../libqfem
    ../libglviewer
    ../libqstructutility
    ../libvarplus
)

# Use required libraries
target_link_libraries(
    qfem2dgui
    qfemgui qfem Qt5::OpenGL Qt5::Widgets
)
