cmake_minimum_required(VERSION 2.8.9)
if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

project(TestOpenCL)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(TestOpenCL_Sources
	containers.cpp
	glhiddenwidget.cpp
	glwidget.cpp
	loadKernel.cpp
	loadShaders.cpp
	main.cpp
	testopencl.cpp
)

set(TestOpenCL_Headers
	containers.h
	glhiddenwidget.h
	glwidget.h
	loadKernel.h
	loadShaders.h
	testopencl.h
)

set(TestOpenCL_Libraries
	${Boost_LIBRARIES}
	${OPENCL_LIBRARIES}
	opengl32.lib
	glew64.lib
)

QT5_WRAP_UI(TestOpenCL_Uis
	testopencl.ui
)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${OPENCL_INCLUDE_DIRS})
link_directories(${OPENCL_LIB_DIR})

add_executable(TestOpenCL WIN32
	${TestOpenCL_Sources}
	${TestOpenCL_Headers}
	${TestOpenCL_Uis}
)

target_link_libraries(TestOpenCL ${TestOpenCL_Libraries} Qt5::WinMain)
qt5_use_modules(TestOpenCL Core Gui Widgets Network OpenGL)

create_target_launcher(TestOpenCL
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
