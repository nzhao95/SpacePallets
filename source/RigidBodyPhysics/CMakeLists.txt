#-------------------------------------------------------------------------------
# Project: Recruitment
# File: Physics/CMakeLists.txt
#
# Copyright (C) 2023 Nintendo, All rights reserved.
#
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
#
# The content herein is highly confidential and should be handled accordingly.
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.10)

include(../helpers.cmake)

set(SRC_FILES
	"main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/all.h"
)

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/all.h" PROPERTIES GENERATED TRUE)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/check.cpp")
	list(APPEND SRC_FILES "check.cpp" "check.h")
	add_definitions("-DHAVE_CHECK")
endif()

source_group("Source Files" FILES ${SRC_FILES})

build_src_files_list(${CMAKE_CURRENT_SOURCE_DIR} TRUE)

add_executable(RigidBodyPhysics
	${SRC_FILES}
)

target_include_directories(RigidBodyPhysics
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR} 
		glfw
)

target_link_libraries(RigidBodyPhysics PRIVATE Helpers)

set(SUBMISSION_FILES "${SUBMISSION_FILES}" PARENT_SCOPE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(GLU REQUIRED)

target_link_libraries(RigidBodyPhysics PRIVATE glfw ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
