cmake_minimum_required(VERSION 3.2)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

include(ExternalProject)
ExternalProject_Add( ToolBase-download
	GIT_REPOSITORY   https://github.com/rasidin/ToolBase.git
	GIT_TAG			 master
	SOURCE_DIR 		 ${CMAKE_CURRENT_BINARY_DIR}/depends/source/ToolBase
	BINARY_DIR		 ${CMAKE_CURRENT_BINARY_DIR}/depends/build/ToolBase
	CMAKE_ARGS		 -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
)
ExternalProject_Get_Property(ToolBase)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/ToolBase/include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)