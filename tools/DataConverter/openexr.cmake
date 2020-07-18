cmake_minimum_required( VERSION 3.2 )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

include( ExternalProject )

ExternalProject_Add( openexr-download
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/openexr
  INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/lib
  SOURCE_DIR 		${CMAKE_CURRENT_BINARY_DIR}/depends/source/openexr
  BINARY_DIR		${CMAKE_CURRENT_BINARY_DIR}/depends/build/openexr
  GIT_REPOSITORY    https://github.com/AcademySoftwareFoundation/openexr.git
  GIT_TAG           v2.4.0
  CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
)
ExternalProject_Get_Property(openexr)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/openexr/OpenEXR/IlmImf)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/openexr/IlmBase/Imath)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/openexr/IlmBase/Iex)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/source/openexr/IlmBase/half)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/build/openexr/OpenEXR/config)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/depends/build/openexr/IlmBase/config)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)
