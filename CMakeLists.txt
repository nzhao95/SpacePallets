<<<<<<< HEAD
#-------------------------------------------------------------------------------
# Project: Recruitment
# File: CMakeLists.txt
#
# Copyright (C) 2018 Nintendo, All rights reserved.
#
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
#
# The content herein is highly confidential and should be handled accordingly.
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.17)
set(CORE_MSVC_USE_COMMON_OPTIONS off) # This flags allows not to use the warning as error flag (and others) from NerdCommonLibs


project(NerdRecruitment C CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/")

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/NerdCMakeModules")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/NerdCMakeModules")
endif()

get_filename_component(NERD_RECRUITMENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
add_definitions("-DNERD_RECRUITMENT_DIR=\"${NERD_RECRUITMENT_DIR}\"")

if (WIN32)
	add_definitions("-DUNICODE" "-D_UNICODE")
endif()

if (MSVC)
	add_compile_definitions("_CRT_SECURE_NO_WARNINGS" "_USE_MATH_DEFINES")
	add_compile_options(
		"$<$<COMPILE_LANGUAGE:C,CXX>:/W4>"
		"$<$<COMPILE_LANGUAGE:C,CXX>:/MP>"
		"$<$<COMPILE_LANGUAGE:C,CXX>:/Zi>"
		"$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>"
		"$<$<COMPILE_LANGUAGE:CXX>:/permissive->"
		"$<$<COMPILE_LANGUAGE:CXX>:/std:c++latest>"
		"$<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_STANDARD 23)
	add_compile_options("-Wall")

	if (CMAKE_SYSTEM_PROCESSOR MATCHES "[xX]86|[xX]86[-_]64|[aA][mM][dD]64")
		include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckCXXCompilerX86ArchFlag.cmake")
		# Enable supported SIMD instruction sets
		foreach (flag IN ITEMS "avx512f" "avx2" "fma" "avx" "bmi2" "sse4.2")
			check_cxx_compiler_x86_arch_flag("${flag}" COMPILER_ENABLE_${flag} COMPILER_SUPPORTS_${flag})
		endforeach()

		# Enable supported AVX512 instruction subsets
		if (COMPILER_ENABLE_avx512f)
			foreach (flag "avx512bw" "avx512dq")
				check_cxx_compiler_x86_arch_flag("${flag}" COMPILER_ENABLE_${flag} COMPILER_SUPPORTS_${flag})
			endforeach()
		endif()
	endif()
else()
	message(STATUS "This compiler type is not explicitly supported by the build system")
	message(STATUS "Hoping for the best with a very basic setup")
	set(CMAKE_CXX_STANDARD 23)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(NERD_CANDIDATE "REC991" CACHE STRING "Override candidate namespace")
string(STRIP "${NERD_CANDIDATE}" NERD_CANDIDATE)
if (NOT "${NERD_CANDIDATE}" STREQUAL "NONE" AND NERD_CANDIDATE MATCHES "[a-zA-Z_]+")
	add_compile_definitions("CANDIDATE=${NERD_CANDIDATE}")
	message(STATUS "NERD exercises > CANDIDATE namespace overridden to ${NERD_CANDIDATE}")
endif()

###############################################################################
# Sub-projects
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/NerdLibCore")
	find_package(NerdLibCore REQUIRED)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/FindAnagrams")
	option(NERD_ENABLE_EXERCISE_ANAGRAMS "Enable anagram exercise" OFF)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/Rotation")
	option(NERD_ENABLE_EXERCISE_ROTATION "Enable rotation exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/LeCompteEstBon")
	option(NERD_ENABLE_EXERCISE_LECOMPTEESTBON "Enable 'le compte est bon' exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/T10")
	option(NERD_ENABLE_EXERCISE_T10 "Enable T10 exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/StarSystem")
	option(NERD_ENABLE_EXERCISE_STARSYSTEM "Enable Star system exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/MoMoMotus")
	option(NERD_ENABLE_EXERCISE_MOMOMOTUS "Enable MoMoMotus exercise" ON)
endif()
if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/DeepLearningInference")
	find_package(CUDAToolkit QUIET)
	option(NERD_ENABLE_EXERCISE_DEEPLEARNING_INFERENCE "Enable the DeepLearning Inference exercice" ${CUDAToolkit_FOUND})
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/RigidBodyPhysics")
	option(NERD_ENABLE_EXERCISE_RIGID_BODY_PHYSICS "Enable the rigid body physics exercise" ON)
endif()

add_subdirectory(source/Helpers)
if (NERD_ENABLE_EXERCISE_ANAGRAMS)
	add_subdirectory(source/FindAnagrams)
endif()
if (NERD_ENABLE_EXERCISE_ROTATION)
	add_subdirectory(source/Rotation)
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/RotationGui")
		add_subdirectory(source/RotationGui)
	endif()
endif()
if (NERD_ENABLE_EXERCISE_LECOMPTEESTBON)
	add_subdirectory(source/LeCompteEstBon)
endif()
if (NERD_ENABLE_EXERCISE_T10)
	add_subdirectory(source/T10)
endif()
if (NERD_ENABLE_EXERCISE_STARSYSTEM)
	add_subdirectory(source/StarSystem)
endif()
if (NERD_ENABLE_EXERCISE_MOMOMOTUS)
	add_subdirectory(source/MoMoMotus)
endif()
if (NERD_ENABLE_EXERCISE_DEEPLEARNING_INFERENCE)
	add_subdirectory(source/DeepLearningInference)
endif()
if (NERD_ENABLE_EXERCISE_RIGID_BODY_PHYSICS)
	add_subdirectory(source/RigidBodyPhysics)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Submission.cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(Helpers Submission PROPERTIES FOLDER "Helpers")
foreach (tgt ImGui ImGuiApp NerCore NerdMain NerdPicore)
	if (TARGET ${tgt})
		set_target_properties(${tgt} PROPERTIES FOLDER "external")
	endif()
endforeach()
=======
#-------------------------------------------------------------------------------
# Project: Recruitment
# File: CMakeLists.txt
#
# Copyright (C) 2018 Nintendo, All rights reserved.
#
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
#
# The content herein is highly confidential and should be handled accordingly.
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.17)
set(CORE_MSVC_USE_COMMON_OPTIONS off) # This flags allows not to use the warning as error flag (and others) from NerdCommonLibs


project(NerdRecruitment C CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/")

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/NerdCMakeModules")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/NerdCMakeModules")
endif()

get_filename_component(NERD_RECRUITMENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
add_definitions("-DNERD_RECRUITMENT_DIR=\"${NERD_RECRUITMENT_DIR}\"")

if (WIN32)
	add_definitions("-DUNICODE" "-D_UNICODE")
endif()

if (MSVC)
	add_compile_definitions("_CRT_SECURE_NO_WARNINGS" "_USE_MATH_DEFINES")
	add_compile_options(
		"$<$<COMPILE_LANGUAGE:C,CXX>:/W4>"
		"$<$<COMPILE_LANGUAGE:C,CXX>:/MP>"
		"$<$<COMPILE_LANGUAGE:C,CXX>:/Zi>"
		"$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>"
		"$<$<COMPILE_LANGUAGE:CXX>:/permissive->"
		"$<$<COMPILE_LANGUAGE:CXX>:/std:c++latest>"
		"$<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_STANDARD 23)
	add_compile_options("-Wall")

	if (CMAKE_SYSTEM_PROCESSOR MATCHES "[xX]86|[xX]86[-_]64|[aA][mM][dD]64")
		include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckCXXCompilerX86ArchFlag.cmake")
		# Enable supported SIMD instruction sets
		foreach (flag IN ITEMS "avx512f" "avx2" "fma" "avx" "bmi2" "sse4.2")
			check_cxx_compiler_x86_arch_flag("${flag}" COMPILER_ENABLE_${flag} COMPILER_SUPPORTS_${flag})
		endforeach()

		# Enable supported AVX512 instruction subsets
		if (COMPILER_ENABLE_avx512f)
			foreach (flag "avx512bw" "avx512dq")
				check_cxx_compiler_x86_arch_flag("${flag}" COMPILER_ENABLE_${flag} COMPILER_SUPPORTS_${flag})
			endforeach()
		endif()
	endif()
else()
	message(STATUS "This compiler type is not explicitly supported by the build system")
	message(STATUS "Hoping for the best with a very basic setup")
	set(CMAKE_CXX_STANDARD 23)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(NERD_CANDIDATE "REC991" CACHE STRING "Override candidate namespace")
string(STRIP "${NERD_CANDIDATE}" NERD_CANDIDATE)
if (NOT "${NERD_CANDIDATE}" STREQUAL "NONE" AND NERD_CANDIDATE MATCHES "[a-zA-Z_]+")
	add_compile_definitions("CANDIDATE=${NERD_CANDIDATE}")
	message(STATUS "NERD exercises > CANDIDATE namespace overridden to ${NERD_CANDIDATE}")
endif()

###############################################################################
# Sub-projects
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/NerdLibCore")
	find_package(NerdLibCore REQUIRED)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/FindAnagrams")
	option(NERD_ENABLE_EXERCISE_ANAGRAMS "Enable anagram exercise" OFF)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/Rotation")
	option(NERD_ENABLE_EXERCISE_ROTATION "Enable rotation exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/LeCompteEstBon")
	option(NERD_ENABLE_EXERCISE_LECOMPTEESTBON "Enable 'le compte est bon' exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/T10")
	option(NERD_ENABLE_EXERCISE_T10 "Enable T10 exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/StarSystem")
	option(NERD_ENABLE_EXERCISE_STARSYSTEM "Enable Star system exercise" ON)
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/MoMoMotus")
	option(NERD_ENABLE_EXERCISE_MOMOMOTUS "Enable MoMoMotus exercise" ON)
endif()
if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/DeepLearningInference")
	find_package(CUDAToolkit QUIET)
	option(NERD_ENABLE_EXERCISE_DEEPLEARNING_INFERENCE "Enable the DeepLearning Inference exercice" ${CUDAToolkit_FOUND})
endif()
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/RigidBodyPhysics")
	option(NERD_ENABLE_EXERCISE_RIGID_BODY_PHYSICS "Enable the rigid body physics exercise" ON)
endif()

add_subdirectory(source/Helpers)
if (NERD_ENABLE_EXERCISE_ANAGRAMS)
	add_subdirectory(source/FindAnagrams)
endif()
if (NERD_ENABLE_EXERCISE_ROTATION)
	add_subdirectory(source/Rotation)
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/RotationGui")
		add_subdirectory(source/RotationGui)
	endif()
endif()
if (NERD_ENABLE_EXERCISE_LECOMPTEESTBON)
	add_subdirectory(source/LeCompteEstBon)
endif()
if (NERD_ENABLE_EXERCISE_T10)
	add_subdirectory(source/T10)
endif()
if (NERD_ENABLE_EXERCISE_STARSYSTEM)
	add_subdirectory(source/StarSystem)
endif()
if (NERD_ENABLE_EXERCISE_MOMOMOTUS)
	add_subdirectory(source/MoMoMotus)
endif()
if (NERD_ENABLE_EXERCISE_DEEPLEARNING_INFERENCE)
	add_subdirectory(source/DeepLearningInference)
endif()
if (NERD_ENABLE_EXERCISE_RIGID_BODY_PHYSICS)
	add_subdirectory(source/RigidBodyPhysics)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Submission.cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(Helpers Submission PROPERTIES FOLDER "Helpers")
foreach (tgt ImGui ImGuiApp NerCore NerdMain NerdPicore)
	if (TARGET ${tgt})
		set_target_properties(${tgt} PROPERTIES FOLDER "external")
	endif()
endforeach()
>>>>>>> 65dbe86 (Reorder stuff)
