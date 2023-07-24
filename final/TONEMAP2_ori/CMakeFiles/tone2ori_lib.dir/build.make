# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori

# Include any dependencies generated for this target.
include CMakeFiles/tone2ori_lib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tone2ori_lib.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tone2ori_lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tone2ori_lib.dir/flags.make

CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o: CMakeFiles/tone2ori_lib.dir/flags.make
CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o: tone_mapping.cpp
CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o: CMakeFiles/tone2ori_lib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o -MF CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o.d -o CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o -c /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/tone_mapping.cpp

CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/tone_mapping.cpp > CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.i

CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/tone_mapping.cpp -o CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.s

# Object files for target tone2ori_lib
tone2ori_lib_OBJECTS = \
"CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o"

# External object files for target tone2ori_lib
tone2ori_lib_EXTERNAL_OBJECTS =

libtone2ori_lib.a: CMakeFiles/tone2ori_lib.dir/tone_mapping.cpp.o
libtone2ori_lib.a: CMakeFiles/tone2ori_lib.dir/build.make
libtone2ori_lib.a: CMakeFiles/tone2ori_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libtone2ori_lib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/tone2ori_lib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tone2ori_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tone2ori_lib.dir/build: libtone2ori_lib.a
.PHONY : CMakeFiles/tone2ori_lib.dir/build

CMakeFiles/tone2ori_lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tone2ori_lib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tone2ori_lib.dir/clean

CMakeFiles/tone2ori_lib.dir/depend:
	cd /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/CMakeFiles/tone2ori_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tone2ori_lib.dir/depend

