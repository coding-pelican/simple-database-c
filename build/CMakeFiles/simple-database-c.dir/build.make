# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\HP\source\cmake-workspace\simple-database-c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\HP\source\cmake-workspace\simple-database-c\build

# Include any dependencies generated for this target.
include CMakeFiles/simple-database-c.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/simple-database-c.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/simple-database-c.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/simple-database-c.dir/flags.make

CMakeFiles/simple-database-c.dir/main.c.obj: CMakeFiles/simple-database-c.dir/flags.make
CMakeFiles/simple-database-c.dir/main.c.obj: CMakeFiles/simple-database-c.dir/includes_C.rsp
CMakeFiles/simple-database-c.dir/main.c.obj: C:/Users/HP/source/cmake-workspace/simple-database-c/main.c
CMakeFiles/simple-database-c.dir/main.c.obj: CMakeFiles/simple-database-c.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\HP\source\cmake-workspace\simple-database-c\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/simple-database-c.dir/main.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/simple-database-c.dir/main.c.obj -MF CMakeFiles\simple-database-c.dir\main.c.obj.d -o CMakeFiles\simple-database-c.dir\main.c.obj -c C:\Users\HP\source\cmake-workspace\simple-database-c\main.c

CMakeFiles/simple-database-c.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple-database-c.dir/main.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\HP\source\cmake-workspace\simple-database-c\main.c > CMakeFiles\simple-database-c.dir\main.c.i

CMakeFiles/simple-database-c.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple-database-c.dir/main.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\HP\source\cmake-workspace\simple-database-c\main.c -o CMakeFiles\simple-database-c.dir\main.c.s

# Object files for target simple-database-c
simple__database__c_OBJECTS = \
"CMakeFiles/simple-database-c.dir/main.c.obj"

# External object files for target simple-database-c
simple__database__c_EXTERNAL_OBJECTS =

simple-database-c.exe: CMakeFiles/simple-database-c.dir/main.c.obj
simple-database-c.exe: CMakeFiles/simple-database-c.dir/build.make
simple-database-c.exe: CMakeFiles/simple-database-c.dir/linkLibs.rsp
simple-database-c.exe: CMakeFiles/simple-database-c.dir/objects1
simple-database-c.exe: CMakeFiles/simple-database-c.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\HP\source\cmake-workspace\simple-database-c\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable simple-database-c.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\simple-database-c.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/simple-database-c.dir/build: simple-database-c.exe
.PHONY : CMakeFiles/simple-database-c.dir/build

CMakeFiles/simple-database-c.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\simple-database-c.dir\cmake_clean.cmake
.PHONY : CMakeFiles/simple-database-c.dir/clean

CMakeFiles/simple-database-c.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\HP\source\cmake-workspace\simple-database-c C:\Users\HP\source\cmake-workspace\simple-database-c C:\Users\HP\source\cmake-workspace\simple-database-c\build C:\Users\HP\source\cmake-workspace\simple-database-c\build C:\Users\HP\source\cmake-workspace\simple-database-c\build\CMakeFiles\simple-database-c.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/simple-database-c.dir/depend

