# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /mnt/c/Users/kjpra/Desktop/RPS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/kjpra/Desktop/RPS/build

# Include any dependencies generated for this target.
include shared/CMakeFiles/comms.dir/depend.make

# Include the progress variables for this target.
include shared/CMakeFiles/comms.dir/progress.make

# Include the compile flags for this target's objects.
include shared/CMakeFiles/comms.dir/flags.make

shared/CMakeFiles/comms.dir/comm.cpp.o: shared/CMakeFiles/comms.dir/flags.make
shared/CMakeFiles/comms.dir/comm.cpp.o: ../shared/comm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/kjpra/Desktop/RPS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object shared/CMakeFiles/comms.dir/comm.cpp.o"
	cd /mnt/c/Users/kjpra/Desktop/RPS/build/shared && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/comms.dir/comm.cpp.o -c /mnt/c/Users/kjpra/Desktop/RPS/shared/comm.cpp

shared/CMakeFiles/comms.dir/comm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/comms.dir/comm.cpp.i"
	cd /mnt/c/Users/kjpra/Desktop/RPS/build/shared && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/kjpra/Desktop/RPS/shared/comm.cpp > CMakeFiles/comms.dir/comm.cpp.i

shared/CMakeFiles/comms.dir/comm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/comms.dir/comm.cpp.s"
	cd /mnt/c/Users/kjpra/Desktop/RPS/build/shared && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/kjpra/Desktop/RPS/shared/comm.cpp -o CMakeFiles/comms.dir/comm.cpp.s

# Object files for target comms
comms_OBJECTS = \
"CMakeFiles/comms.dir/comm.cpp.o"

# External object files for target comms
comms_EXTERNAL_OBJECTS =

shared/libcomms.a: shared/CMakeFiles/comms.dir/comm.cpp.o
shared/libcomms.a: shared/CMakeFiles/comms.dir/build.make
shared/libcomms.a: shared/CMakeFiles/comms.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/kjpra/Desktop/RPS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libcomms.a"
	cd /mnt/c/Users/kjpra/Desktop/RPS/build/shared && $(CMAKE_COMMAND) -P CMakeFiles/comms.dir/cmake_clean_target.cmake
	cd /mnt/c/Users/kjpra/Desktop/RPS/build/shared && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/comms.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
shared/CMakeFiles/comms.dir/build: shared/libcomms.a

.PHONY : shared/CMakeFiles/comms.dir/build

shared/CMakeFiles/comms.dir/clean:
	cd /mnt/c/Users/kjpra/Desktop/RPS/build/shared && $(CMAKE_COMMAND) -P CMakeFiles/comms.dir/cmake_clean.cmake
.PHONY : shared/CMakeFiles/comms.dir/clean

shared/CMakeFiles/comms.dir/depend:
	cd /mnt/c/Users/kjpra/Desktop/RPS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/kjpra/Desktop/RPS /mnt/c/Users/kjpra/Desktop/RPS/shared /mnt/c/Users/kjpra/Desktop/RPS/build /mnt/c/Users/kjpra/Desktop/RPS/build/shared /mnt/c/Users/kjpra/Desktop/RPS/build/shared/CMakeFiles/comms.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : shared/CMakeFiles/comms.dir/depend

