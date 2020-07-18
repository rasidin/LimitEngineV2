cmake_minimum_required(VERSION 3.2)

include(ExternalProject)
ExternalProject_Add( LimitEngine-download
	GIT_REPOSITORY   https://github.com/rasidin/LimitEngineV2.git
	GIT_TAG			 master
	SOURCE_DIR 		 ${CMAKE_CURRENT_BINARY_DIR}/depends/source/LimitEngineV2
	BINARY_DIR		 ${CMAKE_CURRENT_BINARY_DIR}/depends/build/LimitEngineV2
	CMAKE_ARGS		 -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
)
ExternalProject_Get_Property(LimitEngine)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/LimitEngineV2/include ${CMAKE_CURRENT_BINARY_DIR}/depends/source/LimitEngineV2/externals)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

add_definitions(-DWINDOWS -DUSE_DX11)
