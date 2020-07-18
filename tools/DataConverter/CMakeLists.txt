cmake_minimum_required(VERSION 3.2)

project(DataConverter)

include(LEMath.cmake)
include(LimitEngine.cmake)
include(zlib.cmake)
include(openexr.cmake)

add_executable(DataConverter
	Source/main.cpp
	Source/TextureConverter.cpp
	Source/FontConverter.cpp
	Source/ModelConverter.cpp
)

target_link_libraries(DataConverter
	LimitEngine 
	LEMath
	zlibd
	IlmImf-2_4_d
	d3d11 d3dcompiler dxguid
)

install(TARGETS DataConverter DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)