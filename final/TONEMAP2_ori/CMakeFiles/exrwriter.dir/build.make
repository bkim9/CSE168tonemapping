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
include CMakeFiles/exrwriter.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/exrwriter.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/exrwriter.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/exrwriter.dir/flags.make

CMakeFiles/exrwriter.dir/tone_mapping.cpp.o: CMakeFiles/exrwriter.dir/flags.make
CMakeFiles/exrwriter.dir/tone_mapping.cpp.o: tone_mapping.cpp
CMakeFiles/exrwriter.dir/tone_mapping.cpp.o: CMakeFiles/exrwriter.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/exrwriter.dir/tone_mapping.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/exrwriter.dir/tone_mapping.cpp.o -MF CMakeFiles/exrwriter.dir/tone_mapping.cpp.o.d -o CMakeFiles/exrwriter.dir/tone_mapping.cpp.o -c /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/tone_mapping.cpp

CMakeFiles/exrwriter.dir/tone_mapping.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exrwriter.dir/tone_mapping.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/tone_mapping.cpp > CMakeFiles/exrwriter.dir/tone_mapping.cpp.i

CMakeFiles/exrwriter.dir/tone_mapping.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exrwriter.dir/tone_mapping.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/tone_mapping.cpp -o CMakeFiles/exrwriter.dir/tone_mapping.cpp.s

# Object files for target exrwriter
exrwriter_OBJECTS = \
"CMakeFiles/exrwriter.dir/tone_mapping.cpp.o"

# External object files for target exrwriter
exrwriter_EXTERNAL_OBJECTS =

exrwriter: CMakeFiles/exrwriter.dir/tone_mapping.cpp.o
exrwriter: CMakeFiles/exrwriter.dir/build.make
exrwriter: /opt/homebrew/lib/libOpenEXR-3_1.30.9.1.dylib
exrwriter: /opt/homebrew/lib/libImath-3_1.29.8.0.dylib
exrwriter: /opt/homebrew/lib/libIlmThread-3_1.30.9.1.dylib
exrwriter: /opt/homebrew/lib/libIex-3_1.30.9.1.dylib
exrwriter: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.3.sdk/usr/lib/libz.tbd
exrwriter: CMakeFiles/exrwriter.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable exrwriter"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/exrwriter.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/exrwriter.dir/build: exrwriter
.PHONY : CMakeFiles/exrwriter.dir/build

CMakeFiles/exrwriter.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/exrwriter.dir/cmake_clean.cmake
.PHONY : CMakeFiles/exrwriter.dir/clean

CMakeFiles/exrwriter.dir/depend:
	cd /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori /Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/TONEMAP2_ori/CMakeFiles/exrwriter.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/exrwriter.dir/depend

