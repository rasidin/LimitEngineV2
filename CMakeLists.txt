cmake_minimum_required(VERSION 3.18)

project(LimitEngine)
enable_testing()

add_subdirectory(ToolBase)
add_subdirectory(LEShaderConverter)
add_subdirectory(LEMath)
include_directories(LEMath/include)

option(MAKE_TEST "Make test program" OFF)
if (MAKE_TEST)
	add_subdirectory(test)
endif()

option(RAYTRACING "Rendering using raytracing" OFF)
if (RAYTRACING)
	add_definitions(-DRAYTRACING)
endif()

if (WIN32)
	add_definitions(-DWINDOWS -DUSE_DX12)
	file(GLOB_RECURSE FILES_PLATFORM
		"source/Platform/DirectX12/*.inl"
		"source/Platform/DirectX12/*.h"
	)
	source_group("DirectX12"	FILES ${FILES_PLATFORM})
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
	"generated/Shaders/*.h"
	"generated/Shaders/*.cpp"
)
source_group("GeneratedShaders"	FILES ${FILES_SHADERS})
file(GLOB_RECURSE FILES_SHADERCODES
	"shader/*.vs"
	"shader/*.ps"
)
file(GLOB_RECURSE FILES_VERTEXSHADERS
	"shader/*.vs"
)
file(GLOB_RECURSE FILES_PIXELSHADERS
	"shader/*.ps"
)

set(FILES_GENERATEDVSHEADERS)
foreach(shaderfile ${FILES_VERTEXSHADERS})
  string(REPLACE ".vs" "" shaderfilename "${shaderfile}")
  get_filename_component(shadername ${shaderfilename} NAME_WE)
  set(outshaderpath "generated/Shaders/${shadername}")
  list(APPEND FILES_GENERATEDVSHEADERS "${outshaderpath}.vs.h")
  message("cmd ${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe -D -O0 -E=vs_main -T=VS_5_0 -N=${shadername}_VS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.vs")
  add_custom_command(
    OUTPUT "${PROJECT_SOURCE_DIR}/${outshaderpath}.vs.h"
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/bin/LEShaderConverter.exe ${PROJECT_SOURCE_DIR}/tools
	COMMAND ${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe -D -O0 -E=vs_main -T=VS_5_0 -N=${shadername}_VS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.vs
	DEPENDS ${shaderfile} ${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe
	COMMENT "Convert ${shaderfile}"
	COMMENT "${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe -D -O0 -E=vs_main -T=VS_5_0 -N=${shadername}_VS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.vs"
  )
endforeach()

set(FILES_GENERATEDPSHEADERS)
foreach(shaderfile ${FILES_PIXELSHADERS})
  string(REPLACE ".ps" "" shaderfilename "${shaderfile}")
  get_filename_component(shadername ${shaderfilename} NAME_WE)
  set(outshaderpath "generated/Shaders/${shadername}")
  list(APPEND FILES_GENERATEDPSHEADERS "${outshaderpath}.ps.h")
  message("cmd ${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe -D -O0 -E=ps_main -T=PS_5_0 -N=${shadername}_PS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.ps")
  add_custom_command(
	OUTPUT "${PROJECT_SOURCE_DIR}/${outshaderpath}.ps.h"
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/bin/LEShaderConverter.exe ${PROJECT_SOURCE_DIR}/tools
	COMMAND ${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe -D -O0 -E=ps_main -T=PS_5_0 -N=${shadername}_PS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.ps
	DEPENDS ${shaderfile} ${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe
	COMMENT "Convert ${shaderfile}"
	COMMENT "${PROJECT_SOURCE_DIR}/tools/LEShaderConverter.exe -D -O0 -E=ps_main -T=PS_5_0 -N=${shadername}_PS ${shaderfile} ${PROJECT_SOURCE_DIR}/${outshaderpath}.ps"
  )
endforeach()

add_custom_target(generatedheaders DEPENDS ${FILES_GENERATEDVSHEADERS} ${FILES_GENERATEDPSHEADERS})

add_library(LimitEngine
	source/LimitEngine.cpp
	${FILES_CORE}
	${FILES_FACTORIES}
	${FILES_MANAGERS}
	${FILES_RENDERER}
	${FILES_SHADERS}
	${FILES_POSTPROCESSORS}
	${FILES_PLATFORM}
)
add_dependencies(LimitEngine generatedheaders)

target_include_directories(LimitEngine PUBLIC 
	${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/externals 
	${PROJECT_SOURCE_DIR}/include 
	${PROJECT_SOURCE_DIR}/generated
)

install(TARGETS LimitEngine
		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

add_test(TestLimitEngine LimitEngineTest)