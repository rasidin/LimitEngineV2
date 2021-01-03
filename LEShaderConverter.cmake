cmake_minimum_required(VERSION 3.2)

include(ExternalProject)
ExternalProject_Add( LEShaderConverter-download
	GIT_REPOSITORY   https://github.com/rasidin/LEShaderConverter.git
	GIT_TAG			 main
	SOURCE_DIR 		 ${CMAKE_CURRENT_BINARY_DIR}/depends/source/LEShaderConverter
	BINARY_DIR		 ${CMAKE_CURRENT_BINARY_DIR}/depends/build/LEShaderConverter
	INSTALL_DIR		 ${CMAKE_CURRENT_BINARY_DIR}/tools
	CMAKE_ARGS		 -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
	INSTALL_COMMAND  ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/bin/LEShaderConverter.exe ${CMAKE_CURRENT_BINARY_DIR}/tools
)
