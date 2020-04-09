cmake_minimum_required(VERSION 3.1)

project(LimitEngine)

include(LEMath.cmake)

add_subdirectory(test)

add_definitions(-DWINDOWS -DUSE_DX11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(LimitEngine
	source/LimitEngine.cpp
	source/Core/Archive.cpp
	source/Core/Debug.cpp
	source/Core/Event.cpp
	source/Core/Memory.cpp
	source/Core/MemoryAllocator.cpp
	source/Core/String.cpp
	source/Core/TextParser.cpp
	source/Core/Thread.cpp
	source/Core/Timer.cpp
	source/Core/Util.cpp
	source/Factories/ArchiveFactory.cpp
	source/Factories/ModelFactory.cpp
	source/Factories/ShaderFactory.cpp
	source/Factories/TextureFactory.cpp
	source/Factories/TextParserFactory.cpp
	source/Managers/DrawManager.cpp
	source/Managers/Draw2DManager.cpp
	source/Managers/ResourceManager.cpp
	source/Managers/SceneManager.cpp
	source/Managers/ShaderDriverManager.cpp
	source/Managers/ShaderManager.cpp
	source/Managers/TaskManager.cpp
	source/Renderer/AABB.cpp
	source/Renderer/CommandBuffer.cpp
	source/Renderer/Camera.cpp
	source/Renderer/FPolygon.cpp
	source/Renderer/Font.cpp
	source/Renderer/IndexBuffer.cpp
	source/Renderer/Light.cpp
	source/Renderer/Material.cpp
	source/Renderer/Model.cpp
	source/Renderer/RenderContext.cpp
	source/Renderer/Shader.cpp
	source/Renderer/ShaderDriver.cpp
	source/Renderer/ShaderDriverBakedBRDF.cpp
	source/Renderer/ShaderDriverGeneral.cpp
	source/Renderer/ShaderDriverLight.cpp
	source/Renderer/ShaderDriverParameter.cpp
	source/Renderer/ShaderParameterParser.cpp
	source/Renderer/Sprite.cpp
	source/Renderer/Texture.cpp
	source/Renderer/VertexBuffer.cpp
)
target_include_directories(LimitEngine PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

install(TARGETS LimitEngine
		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)