cmake_minimum_required(VERSION 2.8.11)

# Find the required modules
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Tell CMake to create the library
add_executable( QStruct
    main.cpp
)

# Use required libraries
target_link_libraries(QStruct libqstructure )
