cmake_minimum_required(VERSION 3.18)

project(LimitEngine)
enable_testing()

include(LEMath.cmake)
include(LEShaderConverter.cmake)

option(MAKE_TEST "Make test program" OFF)
if (MAKE_TEST)
	add_subdirectory(test)
endif()

option(RAYTRACING "Rendering using raytracing" OFF)
if (WIN32)
	if (RAYTRACING)
		add_definitions(-DWINDOWS -DUSE_DX12 -DRAYTRACING)
		file(GLOB_RECURSE FILES_PLATFORM
			"source/Platform/DirectX12/*.inl"
			"source/Platform/DirectX12/*.h"
		)
		source_group("DirectX12"	FILES ${FILES_PLATFORM})
	else()
		add_definitions(-DWINDOWS -DUSE_DX11)
		file(GLOB_RECURSE FILES_DIRECTX11
			"source/Platform/DirectX11/*.inl"
			"source/Platform/DirectX11/*.h"
		)
		source_group("DirectX11"	FILES ${FILES_PLATFORM})
	endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE FILES_CORE
	"source/Core/*.cpp"
	"include/Core/*.h"
)
source_group("Core"				FILES ${FILES_CORE})
file(GLOB_RECURSE FILES_FACTORIES
	"source/Factories/*.cpp"
	"include/Factories/*.h"
)
source_group("Factories"		FILES ${FILES_FACTORIES})
file(GLOB_RECURSE FILES_MANAGERS
	"source/Managers/*.cpp"
	"include/Managers/*.h"
)
source_group("Managers"			FILES ${FILES_MANAGERS})
file(GLOB_RECURSE FILES_POSTPROCESSORS
	"source/PostProcessors/*.cpp"
	"include/PostProcessors/*.h"
)
source_group("PostProcessors"	FILES ${FILES_POSTPROCESSORS})
file(GLOB_RECURSE FILES_RENDERER
	"source/Renderer/*.cpp"
	"include/Renderer/*.h"
)
source_group("Renderer"	FILES ${FILES_RENDERER})
file(GLOB_RECURSE FILES_SHADERS
	"source/Shaders/*.h"
	"source/Shaders/*.cpp"
)
file(GLOB_RECURSE FILES_SHADERCODES
	"shader/*.vs"
	"shader/*.ps"
)
file(GLOB_RECURSE FILES_VERTEXSHADERS
	"shader/*.vs"
)
add_custom_target(PIXELSHADERS ALL
  DEPENDS ${FILES_VERTEXSHADERS})
file(GLOB_RECURSE FILES_PIXELSHADERS
	"shader/*.ps"
)

set(FILES_GENERATEDVSHEADERS)
foreach(shaderfile ${FILES_VERTEXSHADERS})
  string(REPLACE ".vs" "" shaderfilename "${shaderfile}")
  get_filename_component(shadername ${shaderfilename} NAME_WE)
  set(outshaderpath "include/Shaders/${shadername}")
  list(APPEND FILES_GENERATEDVSHEADERS "${outshaderpath}.vs.h")
  message("cmd ${CMAKE_CURRENT_BINARY_DIR}/tools/LEShaderConverter.exe -D -O0 -E=vs_main -T=VS_5_0 -N=${shadername}_VS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.vs")
  add_custom_command(
    OUTPUT "${outshaderpath}.vs.h"
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tools/LEShaderConverter.exe -D -O0 -E=vs_main -T=VS_5_0 -N=${shadername}_VS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.vs
	DEPENDS ${shaderfile}
	COMMENT "Convert ${shaderfile}"
	COMMENT "${CMAKE_CURRENT_BINARY_DIR}/tools/LEShaderConverter.exe -D -O0 -E=vs_main -T=VS_5_0 -N=${shadername}_VS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.vs"
  )
endforeach()

set(FILES_GENERATEDPSHEADERS)
foreach(shaderfile ${FILES_PIXELSHADERS})
  string(REPLACE ".ps" "" shaderfilename "${shaderfile}")
  get_filename_component(shadername ${shaderfilename} NAME_WE)
  set(outshaderpath "include/Shaders/${shadername}")
  list(APPEND FILES_GENERATEDPSHEADERS "${outshaderpath}.ps.h")
  message("cmd ${CMAKE_CURRENT_BINARY_DIR}/tools/LEShaderConverter.exe -D -O0 -E=ps_main -T=PS_5_0 -N=${shadername}_PS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.ps")
  add_custom_command(
	OUTPUT "${outshaderpath}.ps.h"
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tools/LEShaderConverter.exe -D -O0 -E=ps_main -T=PS_5_0 -N=${shadername}_PS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.ps
	DEPENDS ${shaderfile}
	COMMENT "Convert ${shaderfile}"
	COMMENT "${CMAKE_CURRENT_BINARY_DIR}/tools/LEShaderConverter.exe -D -O0 -E=ps_main -T=PS_5_0 -N=${shadername}_PS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.ps"
  )
endforeach()

add_custom_target(generatedheaders DEPENDS ${FILES_GENERATEDVSHEADERS} ${FILES_GENERATEDPSHEADERS})

add_library(libLimitEngine
	source/LimitEngine.cpp
	${FILES_CORE}
	${FILES_FACTORIES}
	${FILES_MANAGERS}
	${FILES_RENDERER}
	${FILES_SHADERS}
	${FILES_POSTPROCESSORS}
	${FILES_PLATFORM}
)
add_dependencies(libLimitEngine generatedheaders)

target_include_directories(libLimitEngine PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/externals ${PROJECT_SOURCE_DIR}/include)

install(TARGETS libLimitEngine
		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

add_test(TestLimitEngine LimitEngineTest)