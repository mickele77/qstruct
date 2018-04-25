cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the required modules
find_package(Qt5 COMPONENTS Core REQUIRED)

set( SRC
    section.cpp
    sectionpoints.cpp
    sectionsteelcncr.cpp
    sectioncncr.cpp
    sectionrcncr.cpp
    sectionsteel.cpp
    sectiontimber.cpp
    sectionsteeldatasetmodel.cpp
    sectiontimberdatasetmodel.cpp
    sectionmodel.cpp
    sectionline.cpp
    sectionarc.cpp
    sectionthinsimple.cpp
    sectionthin.cpp
    sectionsteeli.cpp
    sectionsteelcirc.cpp
    sectionsteelomega.cpp
    sectionsteelrect.cpp
    sectionsteelvar.cpp
    sectionsteelprofile.cpp
    sectiontimbercncr.cpp
    multisectionarcmodel.cpp
    multisection.cpp
    multisectioncncrmodel.cpp
    multisectionlinemodel.cpp
    multisectionsteelcncrmodel.cpp
    sectionthincont.cpp
    sectionthincontpointsmodel.cpp
    sectionprofile.cpp
    multisectionmodelbase.cpp
    sectionsteell.cpp
    multisectionsteelmodel.cpp
    sectionthincontsteel.cpp
    sectionthincontprivate.cpp
    sectionfrp.cpp
    multisectionfrpmodel.cpp
    sectionloadphase.cpp
    sectionloadphasemodel.cpp
)

set( HDR
    section.h
    sectionpoints.h
    sectionsteelcncr.h
    sectioncncr.h
    sectionrcncr.h
    sectionsteel.h
    sectiontimber.h
    sectiontimberdatasetmodel.h 
    sectionsteeldatasetmodel.h 
    sectionmodel.h
    sectionline.h
    sectionarc.h
    sectionthinsimple.h
    sectionthin.h
    sectionsteeli.h
    sectionsteelcirc.h
    sectionsteelomega.h
    sectionsteelrect.h
    sectionsteelvar.h
    sectionsteelprofile.h
    sectiontimbercncr.h
    multisectionarcmodel.h
    multisectioncncrmodel.h
    multisectionlinemodel.h
    multisectionsteelcncrmodel.h
    multisection.h
    sectionthincont.h
    sectionthincontpointsmodel.h
    sectionthinprivate.h
    sectionprofile.h
    section_export.h
    multisectionmodelbase.h
    section_def.h
    sectionsteell.h
    multisectionsteelmodel.h
    sectionthincontsteel.h
    sectionthincontprivate.h
    sectionfrp.h
    multisectionfrpmodel.h
    sectionloadphase.h
    sectionloadphasemodel.h
)

# Tell CMake to create the library
add_library( qsection
    ${SRC} ${HDR}
)

include_directories(
    ../libqstructutility
    ../libvarplus
    ../libqmaterial
    ../libqload
)

# Use required libraries
target_link_libraries(
    qsection
    qmaterial qload
)
