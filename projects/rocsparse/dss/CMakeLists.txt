# ########################################################################
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# ########################################################################

# This option only works for make/nmake and the ninja generators, but no reason it shouldn't be on all the time
# This tells cmake to create a compile_commands.json file that can be used with clang tooling or vim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Print verbose compiler flags
if(BUILD_VERBOSE)
  include(../cmake/Verbose.cmake)
endif()

if (rocblas_FOUND)
  message("-- Build hipDSS library with rocBLAS")
  add_compile_options(-DHIPDSS_WITH_ROCBLAS)
endif()

if (rocsparse_FOUND)
  message("-- Build hipDSS library with rocSPARSE")
  add_compile_options(-DROCSPARSE_WITH_ROCBLAS)
endif()

# Configure a header file to pass the hipDSS version
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/hipdss-version.h.in"
               "${PROJECT_BINARY_DIR}/include/hipdss/hipdss-version.h"
)

# Public hipdss headers: the only headers under library/include
file(GLOB_RECURSE hipdss_headers_public "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
list(APPEND hipdss_headers_public ${PROJECT_BINARY_DIR}/include/hipdss/hipdss-version.h)

source_group("Header Files\\Public" FILES ${hipdss_headers_public})

# Include sources
include(src/CMakeLists.txt)

# Create hipDSS library
add_library(hipdss ${hipdss_source} ${hipdss_headers_public})
add_library(roc::hipdss ALIAS hipdss)

# Target compile options
target_compile_options(hipdss PRIVATE -Wno-unused-command-line-argument -Wall)

# library/include/hipdss-export.h keys off HIPDSS_STATIC_DEFINE
if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(hipdss PUBLIC HIPDSS_STATIC_DEFINE)
endif()

# Target include directories
target_include_directories(hipdss
                           PRIVATE ${ROCPRIM_INCLUDE_DIRS}
                                   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
                                   $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/library/include>
                           PUBLIC  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/hipdss>
                                   $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Target link libraries
target_link_libraries(hipdss PRIVATE roc::rocprim hip::device)
set(static_depends PACKAGE rocprim)

if (BUILD_WITH_ROCBLAS AND rocblas_FOUND)
  target_link_libraries(hipdss PRIVATE roc::rocblas)
  if(NOT BUILD_SHARED_LIBS)
    list(APPEND static_depends PACKAGE rocblas)
  endif()
endif()



if (BUILD_WITH_ROCSPARSE AND rocsparse_FOUND)
  target_link_libraries(hipdss PRIVATE roc::rocsparse)

  if(NOT BUILD_SHARED_LIBS)
    list(APPEND static_depends PACKAGE rocsparse)
  endif()
endif()

if (NOT WIN32)
  if (BUILD_SHARED_LIBS AND BUILD_WITH_ROCTX)

    set(ROCTX_PATH "" CACHE STRING "Path to the roctx library (directory containing libroctx64.so)")

    find_path(ROCTRACER_INCLUDE_DIR
      NAMES roctracer/roctx.h
      HINTS
      ${ROCTX_PATH}
      ${ROCTX_PATH}/include
      /opt/rocm/include
      ${ROCTX_PATH}/../include
      DOC "Path to the roctracer include directory containing roctx.h")

    find_library(ROCTX_LIBRARY
      NAMES roctx64
      HINTS
      ${ROCTX_PATH} # User-provided path
      ${ROCTX_PATH}/lib
      ${ROCTX_PATH}/lib64
      PATHS
      /opt/rocm/lib # Default ROCm path
    )

    if(ROCTRACER_INCLUDE_DIR AND ROCTX_LIBRARY)
      message(STATUS "Found roctracer include directory: ${ROCTRACER_INCLUDE_DIR}")
      message(STATUS "Found roctx library: ${ROCTX_LIBRARY}")

      # Add the include directory
      target_include_directories(hipdss PRIVATE ${ROCTRACER_INCLUDE_DIR})

      # Link the rocTx lib
      target_link_libraries(hipdss PRIVATE ${ROCTX_LIBRARY})
      target_compile_definitions( hipdss PRIVATE HIPDSS_BUILT_WITH_ROCTX )
      message(STATUS "ROCTX tracing support enabled for target hipdss.")
    else()
      message(WARNING "ROCTX tracing will be disabled for target hipdss.")

      if(NOT ROCTRACER_INCLUDE_DIR)
        message(WARNING "Header 'roctracer/roctx.h' not found.")
      endif()

      if(NOT ROCTX_LIBRARY)
        message(WARNING "Library 'roctx64' not found. ROCTX_PATH: '${ROCTX_PATH}' Default PATH: '/opt/rocm/lib'.")
      endif()
    endif()
  endif()
endif()

# Target properties

# Offload compression ?
if(BUILD_WITH_OFFLOAD_COMPRESS)
  set_target_properties(hipdss PROPERTIES COMPILE_FLAGS "--offload-compress")
endif()

rocm_set_soversion(hipdss ${hipdss_SOVERSION})
set_target_properties(hipdss PROPERTIES CXX_VISIBILITY_PRESET "hidden" VISIBILITY_INLINES_HIDDEN ON)
set_target_properties(hipdss PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/staging")
set_target_properties(hipdss PROPERTIES DEBUG_POSTFIX "-d")

if (WIN32 AND BUILD_CLIENTS)
  add_custom_command( TARGET hipdss POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/staging/$<TARGET_FILE_NAME:hipdss> ${PROJECT_BINARY_DIR}/clients/staging/$<TARGET_FILE_NAME:hipdss> )
  if( ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_custom_command( TARGET hipdss POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/staging/hipdss.pdb ${PROJECT_BINARY_DIR}/clients/staging/hipdss.pdb )
  endif()
endif()

# Get all header files from the public and private (hipdss) include directories
file(GLOB_RECURSE HIPDSS_SOURCE_HEADERS
  "${PROJECT_SOURCE_DIR}/library/include/*"
)

# Create custom command to copy headers with proper dependencies
add_custom_command(
  OUTPUT ${PROJECT_BINARY_DIR}/include/hipdss/.header_stamp
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/library/include ${PROJECT_BINARY_DIR}/include/hipdss
  COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_BINARY_DIR}/include/hipdss/.header_stamp
  DEPENDS ${HIPDSS_SOURCE_HEADERS}
  COMMENT "Copying hipDSS headers to build directory"
)

# Create custom target for header copying
add_custom_target(hipdss_headers DEPENDS ${PROJECT_BINARY_DIR}/include/hipdss/.header_stamp)

# Make the library depend on the headers
add_dependencies(hipdss hipdss_headers)

# Install targets
rocm_install_targets(TARGETS hipdss)

# Install only the public hipdss headers (private hipdss headers are not installed)
if(ROCM_USE_DEV_COMPONENT)
  set(HIPDSS_HEADER_COMPONENT devel)
else()
  set(HIPDSS_HEADER_COMPONENT runtime)
endif()
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/library/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hipdss
  COMPONENT ${HIPDSS_HEADER_COMPONENT}
  FILES_MATCHING PATTERN "*.h"
)

# Export targets
rocm_export_targets(TARGETS roc::hipdss
                    DEPENDS PACKAGE hip
                    STATIC_DEPENDS ${static_depends}
                    NAMESPACE roc::
)
