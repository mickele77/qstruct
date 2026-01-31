find_package(Qt5 COMPONENTS Widgets)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Tell CMake to create the library
add_executable( QStruct
    main.cpp
)

install( TARGETS QStruct DESTINATION bin )

include_directories(
    ../libqstructure
)

# Use required libraries
target_link_libraries( QStruct
    qstructure
)
