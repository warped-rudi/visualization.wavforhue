# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/osboxes/xbmc/project/cmake/addons

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/osboxes/visualization.wavforhue

# Utility rule file for kodi-platform.

# Include the progress variables for this target.
include depends/CMakeFiles/kodi-platform.dir/progress.make

depends/CMakeFiles/kodi-platform: depends/CMakeFiles/kodi-platform-complete

depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-install
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-mkdir
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-update
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-patch
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-build
depends/CMakeFiles/kodi-platform-complete: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-install
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Completed 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/depends/CMakeFiles
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/depends/CMakeFiles/kodi-platform-complete
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-done

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-install: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-build
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Performing install step for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build && $(MAKE) install
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-install

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-mkdir:
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Creating directories for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/build/kodi-platform
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/build/kodi-platform/tmp
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E make_directory /home/osboxes/visualization.wavforhue/build/kodi-platform/src
	cd /home/osboxes/visualization.wavforhue/depends && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-mkdir

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-gitinfo.txt
build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-mkdir
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Performing download step (git clone) for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src && /usr/bin/cmake -P /home/osboxes/visualization.wavforhue/build/kodi-platform/tmp/kodi-platform-gitclone.cmake
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-update: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Performing update step for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform && /usr/bin/cmake -P /home/osboxes/visualization.wavforhue/build/kodi-platform/tmp/kodi-platform-gitupdate.cmake

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-patch: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "No patch step for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-patch

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure: build/kodi-platform/tmp/kodi-platform-cfgcmd.txt
build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-update
build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-patch
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Performing configure step for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build && /usr/bin/cmake -DCMAKE_PREFIX_PATH=/home/osboxes/visualization.wavforhue/build/depends /home/osboxes/xbmc/project/cmake/addons/output/addons /home/osboxes/visualization.wavforhue/build/depends -DOUTPUT_DIR=/home/osboxes/visualization.wavforhue/build/depends -DCMAKE_BUILD_TYPE=Release -DCMAKE_USER_MAKE_RULES_OVERRIDE= -DCMAKE_USER_MAKE_RULES_OVERRIDE_CXX= -DCMAKE_INSTALL_PREFIX=/home/osboxes/visualization.wavforhue/build/depends -DCORE_SYSTEM_NAME=linux -DENABLE_STATIC=1 -DBUILD_SHARED_LIBS=0 "-DCMAKE_C_FLAGS= -DTARGET_POSIX -DTARGET_LINUX -D_LINUX -fPIC" "-DCMAKE_CXX_FLAGS= -DTARGET_POSIX -DTARGET_LINUX -D_LINUX -fPIC" "-GUnix Makefiles" /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure

build/kodi-platform/src/kodi-platform-stamp/kodi-platform-build: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure
	$(CMAKE_COMMAND) -E cmake_progress_report /home/osboxes/visualization.wavforhue/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Performing build step for 'kodi-platform'"
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build && $(MAKE)
	cd /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-build && /usr/bin/cmake -E touch /home/osboxes/visualization.wavforhue/build/kodi-platform/src/kodi-platform-stamp/kodi-platform-build

kodi-platform: depends/CMakeFiles/kodi-platform
kodi-platform: depends/CMakeFiles/kodi-platform-complete
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-install
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-mkdir
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-download
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-update
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-patch
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-configure
kodi-platform: build/kodi-platform/src/kodi-platform-stamp/kodi-platform-build
kodi-platform: depends/CMakeFiles/kodi-platform.dir/build.make
.PHONY : kodi-platform

# Rule to build all files generated by this target.
depends/CMakeFiles/kodi-platform.dir/build: kodi-platform
.PHONY : depends/CMakeFiles/kodi-platform.dir/build

depends/CMakeFiles/kodi-platform.dir/clean:
	cd /home/osboxes/visualization.wavforhue/depends && $(CMAKE_COMMAND) -P CMakeFiles/kodi-platform.dir/cmake_clean.cmake
.PHONY : depends/CMakeFiles/kodi-platform.dir/clean

depends/CMakeFiles/kodi-platform.dir/depend:
	cd /home/osboxes/visualization.wavforhue && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/osboxes/xbmc/project/cmake/addons /home/osboxes/xbmc/project/cmake/addons/depends /home/osboxes/visualization.wavforhue /home/osboxes/visualization.wavforhue/depends /home/osboxes/visualization.wavforhue/depends/CMakeFiles/kodi-platform.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : depends/CMakeFiles/kodi-platform.dir/depend

