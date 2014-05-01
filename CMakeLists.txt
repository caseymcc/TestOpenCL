cmake_minimum_required (VERSION 2.8.9)
if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

project(TestOpenCL)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message(STATUS "CMake prefix: $ENV{CMAKE_PREFIX_PATH}")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

message(STATUS "module path: ${CMAKE_MODULE_PATH}")

#check for 64bit build
if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
	set(BUILD_x64 TRUE)
else( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
    set(BUILD_x64 FALSE)
endif( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
  
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "Library directory" FORCE)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED )
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)
add_definitions(-DBOOST_ALL_DYN_LINK)

set(BOOST_ROOT $ENV{BOOST_DIR})
find_package(Boost REQUIRED COMPONENTS thread program_options system)
message(status "******Boost")
message(status "*Include Dir: ${Boost_INCLUDE_DIR}")
message(status "*Libraries Dir: ${Boost_LIBRARY_DIRS}")
message(status "*Libraries: ${Boost_LIBRARIES}")

set(OPENCL_ROOT $ENV{OPENCL_DIR})
find_package(OpenCL)
message(status "******OpenCl")
message(status "*Include Dir: ${OPENCL_INCLUDE_DIRS}")
message(status "*Libraries Dir: ${OPENCL_LIBRARIES}")

#Set build directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/ )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/ )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/ )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/bin/${OUTPUTCONFIG}/ )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/lib/${OUTPUTCONFIG}/ )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/lib/${OUTPUTCONFIG}/ )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

#default paths
set(RUNTIME_LIBS
	${QT_BINARY_DIR}
)

include(CreateLaunchers)

add_subdirectory(TestOpenCL)