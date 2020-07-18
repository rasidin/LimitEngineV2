cmake_minimum_required( VERSION 3.2 )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

include( ExternalProject )

ExternalProject_Add( zlib-download
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/zlib
  INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/lib
  SOURCE_DIR 		${CMAKE_CURRENT_BINARY_DIR}/depends/source/zlib
  BINARY_DIR		${CMAKE_CURRENT_BINARY_DIR}/depends/build/zlib
  GIT_REPOSITORY    https://github.com/madler/zlib.git
  GIT_TAG           v1.2.11
  CMAKE_ARGS        -DASM686=off
                    -DAMD64=off
                    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
                    -DCMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX}
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_C_FLAGS=${GL_BINDING_C_FLAGS}
)
ExternalProject_Get_Property(zlib)