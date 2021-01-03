cmake_minimum_required(VERSION 3.2)

include(ExternalProject)
ExternalProject_Add( LEMath-download
	GIT_REPOSITORY   https://github.com/rasidin/LEMath.git
	GIT_TAG			 master
	SOURCE_DIR 		 ${CMAKE_CURRENT_BINARY_DIR}/depends/source/LEMath
	BINARY_DIR		 ${CMAKE_CURRENT_BINARY_DIR}/depends/build/LEMath
	CMAKE_ARGS		 -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
)
ExternalProject_Get_Property(LEMath)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/LEMath/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)