
#
# Copyright (C) 2012 Evgeny Panov
# This file is part of libvrender.
#

cmake_minimum_required(VERSION 2.6)

set(PROJECT vrender)

project(${PROJECT} C)

find_package(OpenGL REQUIRED)

set(BUILDDIR ${PROJECT_SOURCE_DIR}/build/)
set(SRCDIR ${PROJECT_SOURCE_DIR}/src/)
set(INCLUDEDIR ${PROJECT_SOURCE_DIR}/include/)

include_directories(${INCLUDEDIR})

set(SOURCES 
		  ${SRCDIR}/math/matrix.c
		  ${SRCDIR}/math/quaternion.c
          ${SRCDIR}/math/noise.c
		  ${SRCDIR}/render/render.c
		  ${SRCDIR}/render/camera.c
		  ${SRCDIR}/render/shader.c
		  ${SRCDIR}/render/shader_program.c
		  ${SRCDIR}/render/shader_program_file.c
		  ${SRCDIR}/utils.c
		  ${SRCDIR}/input.c
          ${SRCDIR}/parser.c
		  ${SRCDIR}/render/texture.c 
          ${SRCDIR}/render/marching_cubes.c )
set(HEADERS
		  ${INCLUDEDIR}/math/dmath.h
		  ${INCLUDEDIR}/math/vector.h
		  ${INCLUDEDIR}/math/matrix.h
		  ${INCLUDEDIR}/math/quaternion.h
          ${INCLUDEDIR}/math/noise.h
		  ${INCLUDEDIR}/common.h
		  ${INCLUDEDIR}/camera.h
		  ${INCLUDEDIR}/shader.h
		  ${INCLUDEDIR}/shader_program.h
		  ${INCLUDEDIR}/shader_program_file.h
		  ${INCLUDEDIR}/texture.h
		  ${INCLUDEDIR}/render.h
		  ${INCLUDEDIR}/input.h
		  ${INCLUDEDIR}/utils.h
          ${INCLUDEDIR}/parser.h
          ${INCLUDEDIR}/marching_cubes.h 
          ${INCLUDEDIR}/thread.h
          ${INCLUDEDIR}/main_shader.h )

if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	set(LIBRARIES m GL)
    add_definitions(-D__linux)

    set(SOURCES ${SOURCES} ${SRCDIR}/threads/thread_linux.c )

elseif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set(LIBRARIES m opengl32)
	add_definitions(-D__win32)

    set(SOURCES ${SOURCES} ${SRCDIR}/threads/thread_win32.c
                           ${SRCDIR}/gl_funcs.c )
    set(HEADERS ${HEADERS} ${INCLUDEDIR}/gl_funcs.h )

endif()

if(CMAKE_COMPILER_IS_GNUCC)
	set(EXTRA_C_FLAGS "-std=gnu99 -g -fgnu89-inline -march=i686 -funroll-loops -msse -finline-functions -O2 -Wall")
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(EXTRA_C_FLAGS "-std=gnu99 -g -fgnu89-inline -march=i686 -funroll-loops -msse -finline-functions -O2 -Wall")
elseif(MSVC)
	source_group ("Header Files" FILES ${HEADERS})
	source_group ("Source Files" FILES ${SOURCES})
	set(EXTRA_C_FLAGS "/W4")
else()
	message(FATAL_ERROR "-- Unknown compiler " ${CMAKE_C_COMPILER})
endif()


set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${EXTRA_C_FLAGS})

add_library(${PROJECT} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} ${LIBRARIES})
