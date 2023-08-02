<<<<<<< HEAD
#!/usr/bin/make -f

#-------------------------------------------------------------------------------
# Project: Recruitment
# File: Makefile
# 
# Copyright (C) 2018-2019 Nintendo, All rights reserved.
# 
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
# 
# The content herein is highly confidential and should be handled accordingly.
#-------------------------------------------------------------------------------


# Helper makefile to ease using CMake for building with GNU make
#
# Variables that alter the behaviour:
#   PLATFORM:     windows, unix (automatically detected if not specified)
#   MODE:         The value for the CMake variable CMAKE_BUILD_TYPE (release by default)
#   COLOR:        0 or 1: whether to use colored output
#   V or VERBOSE: 0, 1 or 2: the verbosity level (0 is default)
#   MAKE_TYPE:    Type of makefile generated by CMake: Unix, MSYS, MinGW or Ninja (defaults to Unix)

HERE := $(abspath .)


###############################################################################
# Customizable part

# List of CMake targets to build (the first one is the default)
CMAKE_TARGETS := all FindAnagrams LeCompteEstBon MoMoMotus Rotation StarSystem T10 Submission

# Folder that contains the root CMakeLists.txt
SOURCE_DIR := $(HERE)

# Folder that contains the build chain (CMake modules & build tools)
BUILDCHAIN := $(abspath environment)

# Folders for generated files
BUILD_DIR := build
LIB_DIR   := $(HERE)/lib
EXE_DIR   := $(HERE)/bin
DLL_DIR   := $(HERE)/bin


###############################################################################
# Internal stuff

# Default values for options
MODE      ?= release
COLOR     ?= $(if $(findstring j,$(firstword $(MAKEFLAGS))),0,1)
MAKE_TYPE ?= Unix
V         ?= 0
VERBOSE   ?= $(V)

# This makefile
SELF := $(lastword $(MAKEFILE_LIST))

# Detect build platform
ifeq ($(OSTYPE),cygwin)
    HOST_PLATFORM := cygwin
else ifneq ($(findstring cygdrive,$(ORIGINAL_PATH)),)
    HOST_PLATFORM := cygwin
else ifeq ($(OS),Windows_NT)
    HOST_PLATFORM := windows
    HOST_ARCH     := $(PROCESSOR_ARCHITECTURE)
    CMAKE         ?= $(wildcard $(BUILDCHAIN)/cmake/bin/cmake.exe)
    CMAKE         := $(if $(CMAKE),$(CMAKE),cmake)
    MKDIR         := mkdir
else
    HOST_PLATFORM := $(shell uname -s | tr '[:upper:]' '[:lower:]')
endif
HOST_ARCH ?= $(shell uname -m)
DOXYGEN   ?= doxygen
CMAKE     ?= cmake
MKDIR     ?= mkdir -p

BUILDER        = $(MAKE)
GENERATOR      = -G "$(MAKE_TYPE) Makefiles" -DCMAKE_COLOR_MAKEFILE=$(COLOR) 
BUILDER_FLAGS  = $(VERBOSE_FLAGS)

# Ensure some consistency between C and C++ compilers (both GCC or both Clang)
# But don't do it if both are default values
DEFAULT_CC  := cc
DEFAULT_CXX := g++
ifneq ($(CC)$(CXX),$(DEFAULT_CC)$(DEFAULT_CXX))
    ifeq ($(CC),$(DEFAULT_CC))
        export CC := $(if $(findstring g++,$(CXX)),$(subst g++,gcc,$(CXX)),$(CC))
        export CC := $(if $(findstring clang++,$(CXX)),$(subst clang++,clang,$(CXX)),$(CC))
    endif
    ifeq ($(CXX),$(DEFAULT_CXX))
        export CXX := $(if $(findstring gcc,$(CC)),$(subst gcc,g++,$(CC)),$(CXX))
        export CXX := $(if $(findstring clang,$(CC)),$(subst clang,clang++,$(CC)),$(CXX))
    endif
endif

# Try to extract the compiler's name from the CC and CXX variables
ifneq ($(CXX),$(DEFAULT_CXX))
    COMPILER := $(CXX)
else ifneq ($(CC),$(DEFAULT_CC))
    COMPILER := $(CC)
else
    COMPILER := $(CXX)
endif
COMPILER_NAME := $(notdir $(subst .exe,,$(COMPILER)))
COMPILER_NAME := $(subst clang++,clang,$(COMPILER_NAME))
COMPILER_NAME := $(subst g++,gcc,$(COMPILER_NAME))

# Configure target platform
CMAKE_OPTIONS   :=
PLATFORM_SUFFIX :=
# If host is Windows, assume compiler is MSVC. Let's try to find out which one exactly.
ifeq ($(HOST_PLATFORM),windows)
    CL := $(shell cl.exe 2>&1)
    ifeq ($(find Microsoft (R) C/C++ Optimizing Compiler $(CL)),)
        $(warning Could not find MS's cl.exe)
    else
        CL            := $(subst 32-bit,,$(CL)) # Remove "32-bit" when present so that the version number is always the 7th word
        CL_VERSION    := $(firstword $(subst ., ,$(word 7,$(CL))))
        PLATFORM      := windows
        ARCH          := $(word 9,$(CL))
        COMPILER_NAME := $(if $(findstring 14,$(CL_VERSION)),msvc2005,)
        COMPILER_NAME := $(if $(findstring 15,$(CL_VERSION)),msvc2008,$(COMPILER_NAME))
        COMPILER_NAME := $(if $(findstring 16,$(CL_VERSION)),msvc2010,$(COMPILER_NAME))
        COMPILER_NAME := $(if $(findstring 17,$(CL_VERSION)),msvc2012,$(COMPILER_NAME))
        COMPILER_NAME := $(if $(findstring 18,$(CL_VERSION)),msvc2013,$(COMPILER_NAME))
    endif
# Other platforms, assume not cross compiling
else
    PLATFORM := $(HOST_PLATFORM)
    ARCH     := $(HOST_ARCH)

    # Append GCC and Clang version numbers to the compiler name
    ifneq ($(findstring gcc,$(COMPILER_NAME)),)
        COMPILER_NAME := gcc$(shell $(COMPILER) -dumpversion)
    else ifneq ($(findstring clang,$(COMPILER_NAME)),)
        CLANG_VERSION := $(shell $(COMPILER) --version)
        CLANG_VERSION := $(subst clang version ,clang-version-,$(CLANG_VERSION))
        CLANG_VERSION := $(filter clang-version-%,$(CLANG_VERSION))
        CLANG_VERSION := $(word 3,$(subst -, ,$(CLANG_VERSION)))
        COMPILER_NAME := clang$(CLANG_VERSION)
    endif
endif

# Normalize architecture name (the first name in each list is the normalized one)
normalize_arch = $(if $(filter $1,$(ARCH)),$(firstword $1),$(ARCH))
ARCH := $(call normalize_arch, x86 80x86 i%86)
ARCH := $(call normalize_arch, x86_64 amd64 x64 x86-64)
ARCH := $(call normalize_arch, arm armeabi% ARM)
ARCH := $(call normalize_arch, arm64 aarch64 arm64-v8a)
ARCH := $(call normalize_arch, mips mipsel)

# Set some more variables
FOLDER         := $(PLATFORM)$(PLATFORM_SUFFIX)-$(ARCH)-$(COMPILER_NAME)-$(notdir $(subst .exe,,$(BUILDER)))/$(MODE)
CMAKELISTS     := $(SOURCE_DIR)/CMakeLists.txt
DEFAULT_TARGET := $(word 1,$(CMAKE_TARGETS))

# Compute verbosity
ifeq ($(VERBOSE),0)
    VERBOSE_FLAGS := -s
.SILENT:
else ifeq ($(VERBOSE),1)
    VERBOSE_FLAGS := VERBOSE=$(VERBOSE)
endif

# Helper macro to define a target we will forward to CMake
define cmake_target
.PHONY: $1
$1: $$(BUILD_DIR)/$$(FOLDER)/CMakeCache.txt
	+$$(BUILDER) -C $$(BUILD_DIR)/$$(FOLDER) $$(BUILDER_FLAGS) $1

.PHONY: rebuild-$1
rebuild-$1: clobber
	$$(MAKE) -f $$(SELF) $$(VERBOSE_FLAGS) $1

endef


###############################################################################
# Targets

# Default target
.PHONY: default
default: $(DEFAULT_TARGET)

# Removes intermediate files
.PHONY: clean
ifneq ($(wildcard $(BUILD_DIR)/$(FOLDER)),)
clean: $(BUILD_DIR)/$(FOLDER)/CMakeCache.txt
	+$(BUILDER) -C $(BUILD_DIR)/$(FOLDER) $(BUILDER_FLAGS) clean
else
clean:
endif

# Removes everything, including CMake's generated makefiles
.PHONY: clobber
clobber:
ifneq ($(wildcard $(BUILD_DIR)/$(FOLDER)),)
	-+$(BUILDER) -C $(BUILD_DIR)/$(FOLDER) $(BUILDER_FLAGS) clean
	-$(CMAKE) -E remove_directory $(BUILD_DIR)/$(FOLDER)
endif

# Removes everything and rebuilds the default target
.PHONY: rebuild
rebuild: clobber
	$(MAKE) -f $(SELF) $(VERBOSE_FLAGS) default

# Targets forwarded to CMake
$(foreach t,$(CMAKE_TARGETS),$(eval $(call cmake_target,$(t))))

# Rule that runs CMake when CMakeLists.txt file changes
$(BUILD_DIR)/$(FOLDER)/CMakeCache.txt: $(CMAKELISTS)
	$(MAKE) -f $(SELF) $(VERBOSE_FLAGS) build-files
	$(CMAKE) -E touch $(BUILD_DIR)/$(FOLDER)/CMakeCache.txt

# Forces to regenerate build files
.PHONY: build-files
build-files: | $(BUILD_DIR)/$(FOLDER)
	cd "$(BUILD_DIR)/$(FOLDER)" && "$(CMAKE)" $(GENERATOR) "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$(LIB_DIR)/$(FOLDER)" "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$(EXE_DIR)/$(FOLDER)" "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(DLL_DIR)/$(FOLDER)" -DCMAKE_BUILD_TYPE=$(MODE) "-DCMAKE_MAKE_PROGRAM=$(BUILDER)" "$(SOURCE_DIR)"

# Creates the build dir
$(BUILD_DIR)/$(FOLDER):
	$(CMAKE) -E make_directory $(BUILD_DIR)/$(FOLDER)

# Lists available targets
.PHONY: help
help:
	$(info default ($(DEFAULT_TARGET)))
	$(info clean)
	$(info clobber)
	$(info rebuild)
	$(info build-files)
	$(foreach t,$(CMAKE_TARGETS),$(info $(t)))
	$(foreach t,$(CMAKE_TARGETS),$(info rebuild-$(t)))

# Only prints out the build directory, useful for automated builds
.PHONY: echo-build-dir
echo-build-dir:
	$(info $(abspath $(BUILD_DIR)/$(FOLDER)))

# Debug target that prints out the variables we have computed and their values 
.PHONY: echo-vars
echo-vars:
	$(info MAKEFLAGS       = $(MAKEFLAGS))
	$(info SOURCE_DIR      = $(SOURCE_DIR))
	$(info BUILDCHAIN      = $(BUILDCHAIN))
	$(info MODE            = $(MODE))
	$(info COLOR           = $(COLOR))
	$(info MAKE_TYPE       = $(MAKE_TYPE))
	$(info VERBOSE         = $(VERBOSE))
	$(info SELF            = $(SELF))
	$(info CMAKE           = $(CMAKE))
	$(info NINJA           = $(NINJA))
	$(info BUILDER         = $(BUILDER))
	$(info VFLAG           = $(VFLAG))
	$(info JFLAG           = $(JFLAG))
	$(info BUILDER_FLAGS   = $(BUILDER_FLAGS))
	$(info CC              = $(CC))
	$(info CXX             = $(CXX))
	$(info COMPILER        = $(COMPILER))
	$(info COMPILER_NAME   = $(COMPILER_NAME))
	$(info CLANG_VERSION   = $(CLANG_VERSION))
	$(info HOST_PLATFORM   = $(HOST_PLATFORM))
	$(info HOST_ARCH       = $(HOST_ARCH))
	$(info PLATFORM        = $(PLATFORM))
	$(info ARCH            = $(ARCH))
	$(info BUILD_DIR       = $(BUILD_DIR))
	$(info GENERATOR       = $(GENERATOR))
=======
#!/usr/bin/make -f

#-------------------------------------------------------------------------------
# Project: Recruitment
# File: Makefile
# 
# Copyright (C) 2018-2019 Nintendo, All rights reserved.
# 
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
# 
# The content herein is highly confidential and should be handled accordingly.
#-------------------------------------------------------------------------------


# Helper makefile to ease using CMake for building with GNU make
#
# Variables that alter the behaviour:
#   PLATFORM:     windows, unix (automatically detected if not specified)
#   MODE:         The value for the CMake variable CMAKE_BUILD_TYPE (release by default)
#   COLOR:        0 or 1: whether to use colored output
#   V or VERBOSE: 0, 1 or 2: the verbosity level (0 is default)
#   MAKE_TYPE:    Type of makefile generated by CMake: Unix, MSYS, MinGW or Ninja (defaults to Unix)

HERE := $(abspath .)


###############################################################################
# Customizable part

# List of CMake targets to build (the first one is the default)
CMAKE_TARGETS := all FindAnagrams LeCompteEstBon MoMoMotus Rotation StarSystem T10 Submission

# Folder that contains the root CMakeLists.txt
SOURCE_DIR := $(HERE)

# Folder that contains the build chain (CMake modules & build tools)
BUILDCHAIN := $(abspath environment)

# Folders for generated files
BUILD_DIR := build
LIB_DIR   := $(HERE)/lib
EXE_DIR   := $(HERE)/bin
DLL_DIR   := $(HERE)/bin


###############################################################################
# Internal stuff

# Default values for options
MODE      ?= release
COLOR     ?= $(if $(findstring j,$(firstword $(MAKEFLAGS))),0,1)
MAKE_TYPE ?= Unix
V         ?= 0
VERBOSE   ?= $(V)

# This makefile
SELF := $(lastword $(MAKEFILE_LIST))

# Detect build platform
ifeq ($(OSTYPE),cygwin)
    HOST_PLATFORM := cygwin
else ifneq ($(findstring cygdrive,$(ORIGINAL_PATH)),)
    HOST_PLATFORM := cygwin
else ifeq ($(OS),Windows_NT)
    HOST_PLATFORM := windows
    HOST_ARCH     := $(PROCESSOR_ARCHITECTURE)
    CMAKE         ?= $(wildcard $(BUILDCHAIN)/cmake/bin/cmake.exe)
    CMAKE         := $(if $(CMAKE),$(CMAKE),cmake)
    MKDIR         := mkdir
else
    HOST_PLATFORM := $(shell uname -s | tr '[:upper:]' '[:lower:]')
endif
HOST_ARCH ?= $(shell uname -m)
DOXYGEN   ?= doxygen
CMAKE     ?= cmake
MKDIR     ?= mkdir -p

BUILDER        = $(MAKE)
GENERATOR      = -G "$(MAKE_TYPE) Makefiles" -DCMAKE_COLOR_MAKEFILE=$(COLOR) 
BUILDER_FLAGS  = $(VERBOSE_FLAGS)

# Ensure some consistency between C and C++ compilers (both GCC or both Clang)
# But don't do it if both are default values
DEFAULT_CC  := cc
DEFAULT_CXX := g++
ifneq ($(CC)$(CXX),$(DEFAULT_CC)$(DEFAULT_CXX))
    ifeq ($(CC),$(DEFAULT_CC))
        export CC := $(if $(findstring g++,$(CXX)),$(subst g++,gcc,$(CXX)),$(CC))
        export CC := $(if $(findstring clang++,$(CXX)),$(subst clang++,clang,$(CXX)),$(CC))
    endif
    ifeq ($(CXX),$(DEFAULT_CXX))
        export CXX := $(if $(findstring gcc,$(CC)),$(subst gcc,g++,$(CC)),$(CXX))
        export CXX := $(if $(findstring clang,$(CC)),$(subst clang,clang++,$(CC)),$(CXX))
    endif
endif

# Try to extract the compiler's name from the CC and CXX variables
ifneq ($(CXX),$(DEFAULT_CXX))
    COMPILER := $(CXX)
else ifneq ($(CC),$(DEFAULT_CC))
    COMPILER := $(CC)
else
    COMPILER := $(CXX)
endif
COMPILER_NAME := $(notdir $(subst .exe,,$(COMPILER)))
COMPILER_NAME := $(subst clang++,clang,$(COMPILER_NAME))
COMPILER_NAME := $(subst g++,gcc,$(COMPILER_NAME))

# Configure target platform
CMAKE_OPTIONS   :=
PLATFORM_SUFFIX :=
# If host is Windows, assume compiler is MSVC. Let's try to find out which one exactly.
ifeq ($(HOST_PLATFORM),windows)
    CL := $(shell cl.exe 2>&1)
    ifeq ($(find Microsoft (R) C/C++ Optimizing Compiler $(CL)),)
        $(warning Could not find MS's cl.exe)
    else
        CL            := $(subst 32-bit,,$(CL)) # Remove "32-bit" when present so that the version number is always the 7th word
        CL_VERSION    := $(firstword $(subst ., ,$(word 7,$(CL))))
        PLATFORM      := windows
        ARCH          := $(word 9,$(CL))
        COMPILER_NAME := $(if $(findstring 14,$(CL_VERSION)),msvc2005,)
        COMPILER_NAME := $(if $(findstring 15,$(CL_VERSION)),msvc2008,$(COMPILER_NAME))
        COMPILER_NAME := $(if $(findstring 16,$(CL_VERSION)),msvc2010,$(COMPILER_NAME))
        COMPILER_NAME := $(if $(findstring 17,$(CL_VERSION)),msvc2012,$(COMPILER_NAME))
        COMPILER_NAME := $(if $(findstring 18,$(CL_VERSION)),msvc2013,$(COMPILER_NAME))
    endif
# Other platforms, assume not cross compiling
else
    PLATFORM := $(HOST_PLATFORM)
    ARCH     := $(HOST_ARCH)

    # Append GCC and Clang version numbers to the compiler name
    ifneq ($(findstring gcc,$(COMPILER_NAME)),)
        COMPILER_NAME := gcc$(shell $(COMPILER) -dumpversion)
    else ifneq ($(findstring clang,$(COMPILER_NAME)),)
        CLANG_VERSION := $(shell $(COMPILER) --version)
        CLANG_VERSION := $(subst clang version ,clang-version-,$(CLANG_VERSION))
        CLANG_VERSION := $(filter clang-version-%,$(CLANG_VERSION))
        CLANG_VERSION := $(word 3,$(subst -, ,$(CLANG_VERSION)))
        COMPILER_NAME := clang$(CLANG_VERSION)
    endif
endif

# Normalize architecture name (the first name in each list is the normalized one)
normalize_arch = $(if $(filter $1,$(ARCH)),$(firstword $1),$(ARCH))
ARCH := $(call normalize_arch, x86 80x86 i%86)
ARCH := $(call normalize_arch, x86_64 amd64 x64 x86-64)
ARCH := $(call normalize_arch, arm armeabi% ARM)
ARCH := $(call normalize_arch, arm64 aarch64 arm64-v8a)
ARCH := $(call normalize_arch, mips mipsel)

# Set some more variables
FOLDER         := $(PLATFORM)$(PLATFORM_SUFFIX)-$(ARCH)-$(COMPILER_NAME)-$(notdir $(subst .exe,,$(BUILDER)))/$(MODE)
CMAKELISTS     := $(SOURCE_DIR)/CMakeLists.txt
DEFAULT_TARGET := $(word 1,$(CMAKE_TARGETS))

# Compute verbosity
ifeq ($(VERBOSE),0)
    VERBOSE_FLAGS := -s
.SILENT:
else ifeq ($(VERBOSE),1)
    VERBOSE_FLAGS := VERBOSE=$(VERBOSE)
endif

# Helper macro to define a target we will forward to CMake
define cmake_target
.PHONY: $1
$1: $$(BUILD_DIR)/$$(FOLDER)/CMakeCache.txt
	+$$(BUILDER) -C $$(BUILD_DIR)/$$(FOLDER) $$(BUILDER_FLAGS) $1

.PHONY: rebuild-$1
rebuild-$1: clobber
	$$(MAKE) -f $$(SELF) $$(VERBOSE_FLAGS) $1

endef


###############################################################################
# Targets

# Default target
.PHONY: default
default: $(DEFAULT_TARGET)

# Removes intermediate files
.PHONY: clean
ifneq ($(wildcard $(BUILD_DIR)/$(FOLDER)),)
clean: $(BUILD_DIR)/$(FOLDER)/CMakeCache.txt
	+$(BUILDER) -C $(BUILD_DIR)/$(FOLDER) $(BUILDER_FLAGS) clean
else
clean:
endif

# Removes everything, including CMake's generated makefiles
.PHONY: clobber
clobber:
ifneq ($(wildcard $(BUILD_DIR)/$(FOLDER)),)
	-+$(BUILDER) -C $(BUILD_DIR)/$(FOLDER) $(BUILDER_FLAGS) clean
	-$(CMAKE) -E remove_directory $(BUILD_DIR)/$(FOLDER)
endif

# Removes everything and rebuilds the default target
.PHONY: rebuild
rebuild: clobber
	$(MAKE) -f $(SELF) $(VERBOSE_FLAGS) default

# Targets forwarded to CMake
$(foreach t,$(CMAKE_TARGETS),$(eval $(call cmake_target,$(t))))

# Rule that runs CMake when CMakeLists.txt file changes
$(BUILD_DIR)/$(FOLDER)/CMakeCache.txt: $(CMAKELISTS)
	$(MAKE) -f $(SELF) $(VERBOSE_FLAGS) build-files
	$(CMAKE) -E touch $(BUILD_DIR)/$(FOLDER)/CMakeCache.txt

# Forces to regenerate build files
.PHONY: build-files
build-files: | $(BUILD_DIR)/$(FOLDER)
	cd "$(BUILD_DIR)/$(FOLDER)" && "$(CMAKE)" $(GENERATOR) "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$(LIB_DIR)/$(FOLDER)" "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$(EXE_DIR)/$(FOLDER)" "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(DLL_DIR)/$(FOLDER)" -DCMAKE_BUILD_TYPE=$(MODE) "-DCMAKE_MAKE_PROGRAM=$(BUILDER)" "$(SOURCE_DIR)"

# Creates the build dir
$(BUILD_DIR)/$(FOLDER):
	$(CMAKE) -E make_directory $(BUILD_DIR)/$(FOLDER)

# Lists available targets
.PHONY: help
help:
	$(info default ($(DEFAULT_TARGET)))
	$(info clean)
	$(info clobber)
	$(info rebuild)
	$(info build-files)
	$(foreach t,$(CMAKE_TARGETS),$(info $(t)))
	$(foreach t,$(CMAKE_TARGETS),$(info rebuild-$(t)))

# Only prints out the build directory, useful for automated builds
.PHONY: echo-build-dir
echo-build-dir:
	$(info $(abspath $(BUILD_DIR)/$(FOLDER)))

# Debug target that prints out the variables we have computed and their values 
.PHONY: echo-vars
echo-vars:
	$(info MAKEFLAGS       = $(MAKEFLAGS))
	$(info SOURCE_DIR      = $(SOURCE_DIR))
	$(info BUILDCHAIN      = $(BUILDCHAIN))
	$(info MODE            = $(MODE))
	$(info COLOR           = $(COLOR))
	$(info MAKE_TYPE       = $(MAKE_TYPE))
	$(info VERBOSE         = $(VERBOSE))
	$(info SELF            = $(SELF))
	$(info CMAKE           = $(CMAKE))
	$(info NINJA           = $(NINJA))
	$(info BUILDER         = $(BUILDER))
	$(info VFLAG           = $(VFLAG))
	$(info JFLAG           = $(JFLAG))
	$(info BUILDER_FLAGS   = $(BUILDER_FLAGS))
	$(info CC              = $(CC))
	$(info CXX             = $(CXX))
	$(info COMPILER        = $(COMPILER))
	$(info COMPILER_NAME   = $(COMPILER_NAME))
	$(info CLANG_VERSION   = $(CLANG_VERSION))
	$(info HOST_PLATFORM   = $(HOST_PLATFORM))
	$(info HOST_ARCH       = $(HOST_ARCH))
	$(info PLATFORM        = $(PLATFORM))
	$(info ARCH            = $(ARCH))
	$(info BUILD_DIR       = $(BUILD_DIR))
	$(info GENERATOR       = $(GENERATOR))
>>>>>>> 65dbe86 (Reorder stuff)