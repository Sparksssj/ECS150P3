# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /home/sparks/Downloads/CLion-2019.2.4/clion-2019.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/sparks/Downloads/CLion-2019.2.4/clion-2019.2.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sparks/Desktop/Project3INIT/libfs

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/FS.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/FS.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/FS.dir/flags.make

CMakeFiles/FS.dir/fs.c.o: CMakeFiles/FS.dir/flags.make
CMakeFiles/FS.dir/fs.c.o: ../fs.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/FS.dir/fs.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FS.dir/fs.c.o   -c /home/sparks/Desktop/Project3INIT/libfs/fs.c

CMakeFiles/FS.dir/fs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FS.dir/fs.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/sparks/Desktop/Project3INIT/libfs/fs.c > CMakeFiles/FS.dir/fs.c.i

CMakeFiles/FS.dir/fs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FS.dir/fs.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/sparks/Desktop/Project3INIT/libfs/fs.c -o CMakeFiles/FS.dir/fs.c.s

CMakeFiles/FS.dir/disk.c.o: CMakeFiles/FS.dir/flags.make
CMakeFiles/FS.dir/disk.c.o: ../disk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/FS.dir/disk.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FS.dir/disk.c.o   -c /home/sparks/Desktop/Project3INIT/libfs/disk.c

CMakeFiles/FS.dir/disk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FS.dir/disk.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/sparks/Desktop/Project3INIT/libfs/disk.c > CMakeFiles/FS.dir/disk.c.i

CMakeFiles/FS.dir/disk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FS.dir/disk.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/sparks/Desktop/Project3INIT/libfs/disk.c -o CMakeFiles/FS.dir/disk.c.s

# Object files for target FS
FS_OBJECTS = \
"CMakeFiles/FS.dir/fs.c.o" \
"CMakeFiles/FS.dir/disk.c.o"

# External object files for target FS
FS_EXTERNAL_OBJECTS =

FS: CMakeFiles/FS.dir/fs.c.o
FS: CMakeFiles/FS.dir/disk.c.o
FS: CMakeFiles/FS.dir/build.make
FS: CMakeFiles/FS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable FS"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FS.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/FS.dir/build: FS

.PHONY : CMakeFiles/FS.dir/build

CMakeFiles/FS.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/FS.dir/cmake_clean.cmake
.PHONY : CMakeFiles/FS.dir/clean

CMakeFiles/FS.dir/depend:
	cd /home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sparks/Desktop/Project3INIT/libfs /home/sparks/Desktop/Project3INIT/libfs /home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug /home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug /home/sparks/Desktop/Project3INIT/libfs/cmake-build-debug/CMakeFiles/FS.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/FS.dir/depend

