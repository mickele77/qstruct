cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

IF( BUILD_SHARED_LIBS AND WIN32 )
  add_definitions( -DBUILD_VARPLUS_LIB )
ENDIF( BUILD_SHARED_LIBS AND WIN32 )

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    unitmeasure.cpp \
    unitmeasuregui.cpp \
    varplus.cpp \
    doubleplus.cpp \
    point2dplus.cpp \
    line2dplus.cpp \
    point3dplus.cpp \
    qstringplus.cpp \
    intplus.cpp \
    uintplus.cpp \
    boolplus.cpp \
    point2dplusmodel.cpp \
    varpluscontainer.cpp \
    varplusmath.cpp \
    enumplus.cpp \
    tablemodelplus.cpp \
    varpluscontainerplus.cpp
)

set( HDR
    unitmeasure.h \
    unitmeasuregui.h \
    varplus.h \
    doubleplus.h \
    point2dplus.h \
    line2dplus.h \
    point3dplus.h \
    qstringplus.h \
    intplus.h \
    uintplus.h \
    boolplus.h \
    point2dplusmodel.h \
    varpluscontainer.h \
    varplusmath.h \
    enumplus.h \
    tablemodelplus.h \
    tablemodelplusprivate.h \
    varplus_export.h \
    varpluscontainerplus.h
)

# Tell CMake to create the library
add_library( libvarplus
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
)

# Use required libraries
target_link_libraries(libvarplus libqstructutility Qt5::Core)

