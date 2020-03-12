cmake_minimum_required(VERSION 3.1)
project(LimitEngineTest)

include(ToolBase.cmake)

add_definitions(-DWIN32 -DWINDOWS -DUSE_DX11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

set(DEPEND_LIB_ROOT ${PROJECT_BINARY_DIR}/depends)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(LimitEngineTest test.cpp)
target_link_libraries(LimitEngineTest ToolBase LimitEngine LEMath d3d11 d3dcompiler dxguid)

target_include_directories(LimitEngineTest PUBLIC ${PROJECT_SOURCE_DIR}/../include)
add_dependencies(LimitEngineTest ToolBase-download)
