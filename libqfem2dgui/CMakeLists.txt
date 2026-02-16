# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_FEM2DGUI_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

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
