# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/WorkSpace/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/WorkSpace/sylar/build

# Include any dependencies generated for this target.
include CMakeFiles/test_thread.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_thread.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_thread.dir/flags.make

CMakeFiles/test_thread.dir/test_thread.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/test_thread.cc.o: ../test_thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_thread.dir/test_thread.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/test_thread.cc.o -c /root/WorkSpace/sylar/test_thread.cc

CMakeFiles/test_thread.dir/test_thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/test_thread.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/test_thread.cc > CMakeFiles/test_thread.dir/test_thread.cc.i

CMakeFiles/test_thread.dir/test_thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/test_thread.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/test_thread.cc -o CMakeFiles/test_thread.dir/test_thread.cc.s

CMakeFiles/test_thread.dir/test_thread.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/test_thread.cc.o.requires

CMakeFiles/test_thread.dir/test_thread.cc.o.provides: CMakeFiles/test_thread.dir/test_thread.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/test_thread.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/test_thread.cc.o.provides

CMakeFiles/test_thread.dir/test_thread.cc.o.provides.build: CMakeFiles/test_thread.dir/test_thread.cc.o


CMakeFiles/test_thread.dir/thread.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/thread.cc.o: ../thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/test_thread.dir/thread.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/thread.cc.o -c /root/WorkSpace/sylar/thread.cc

CMakeFiles/test_thread.dir/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/thread.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/thread.cc > CMakeFiles/test_thread.dir/thread.cc.i

CMakeFiles/test_thread.dir/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/thread.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/thread.cc -o CMakeFiles/test_thread.dir/thread.cc.s

CMakeFiles/test_thread.dir/thread.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/thread.cc.o.requires

CMakeFiles/test_thread.dir/thread.cc.o.provides: CMakeFiles/test_thread.dir/thread.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/thread.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/thread.cc.o.provides

CMakeFiles/test_thread.dir/thread.cc.o.provides.build: CMakeFiles/test_thread.dir/thread.cc.o


CMakeFiles/test_thread.dir/scheduler.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/scheduler.cc.o: ../scheduler.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/test_thread.dir/scheduler.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/scheduler.cc.o -c /root/WorkSpace/sylar/scheduler.cc

CMakeFiles/test_thread.dir/scheduler.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/scheduler.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/scheduler.cc > CMakeFiles/test_thread.dir/scheduler.cc.i

CMakeFiles/test_thread.dir/scheduler.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/scheduler.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/scheduler.cc -o CMakeFiles/test_thread.dir/scheduler.cc.s

CMakeFiles/test_thread.dir/scheduler.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/scheduler.cc.o.requires

CMakeFiles/test_thread.dir/scheduler.cc.o.provides: CMakeFiles/test_thread.dir/scheduler.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/scheduler.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/scheduler.cc.o.provides

CMakeFiles/test_thread.dir/scheduler.cc.o.provides.build: CMakeFiles/test_thread.dir/scheduler.cc.o


CMakeFiles/test_thread.dir/mutex.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/mutex.cc.o: ../mutex.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/test_thread.dir/mutex.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/mutex.cc.o -c /root/WorkSpace/sylar/mutex.cc

CMakeFiles/test_thread.dir/mutex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/mutex.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/mutex.cc > CMakeFiles/test_thread.dir/mutex.cc.i

CMakeFiles/test_thread.dir/mutex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/mutex.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/mutex.cc -o CMakeFiles/test_thread.dir/mutex.cc.s

CMakeFiles/test_thread.dir/mutex.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/mutex.cc.o.requires

CMakeFiles/test_thread.dir/mutex.cc.o.provides: CMakeFiles/test_thread.dir/mutex.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/mutex.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/mutex.cc.o.provides

CMakeFiles/test_thread.dir/mutex.cc.o.provides.build: CMakeFiles/test_thread.dir/mutex.cc.o


CMakeFiles/test_thread.dir/utils.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/utils.cc.o: ../utils.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/test_thread.dir/utils.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/utils.cc.o -c /root/WorkSpace/sylar/utils.cc

CMakeFiles/test_thread.dir/utils.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/utils.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/utils.cc > CMakeFiles/test_thread.dir/utils.cc.i

CMakeFiles/test_thread.dir/utils.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/utils.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/utils.cc -o CMakeFiles/test_thread.dir/utils.cc.s

CMakeFiles/test_thread.dir/utils.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/utils.cc.o.requires

CMakeFiles/test_thread.dir/utils.cc.o.provides: CMakeFiles/test_thread.dir/utils.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/utils.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/utils.cc.o.provides

CMakeFiles/test_thread.dir/utils.cc.o.provides.build: CMakeFiles/test_thread.dir/utils.cc.o


CMakeFiles/test_thread.dir/fiber.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/fiber.cc.o: ../fiber.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/test_thread.dir/fiber.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/fiber.cc.o -c /root/WorkSpace/sylar/fiber.cc

CMakeFiles/test_thread.dir/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/fiber.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/fiber.cc > CMakeFiles/test_thread.dir/fiber.cc.i

CMakeFiles/test_thread.dir/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/fiber.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/fiber.cc -o CMakeFiles/test_thread.dir/fiber.cc.s

CMakeFiles/test_thread.dir/fiber.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/fiber.cc.o.requires

CMakeFiles/test_thread.dir/fiber.cc.o.provides: CMakeFiles/test_thread.dir/fiber.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/fiber.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/fiber.cc.o.provides

CMakeFiles/test_thread.dir/fiber.cc.o.provides.build: CMakeFiles/test_thread.dir/fiber.cc.o


CMakeFiles/test_thread.dir/iomanager.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/iomanager.cc.o: ../iomanager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/test_thread.dir/iomanager.cc.o"
	/apps/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/iomanager.cc.o -c /root/WorkSpace/sylar/iomanager.cc

CMakeFiles/test_thread.dir/iomanager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/iomanager.cc.i"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/WorkSpace/sylar/iomanager.cc > CMakeFiles/test_thread.dir/iomanager.cc.i

CMakeFiles/test_thread.dir/iomanager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/iomanager.cc.s"
	/apps/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/WorkSpace/sylar/iomanager.cc -o CMakeFiles/test_thread.dir/iomanager.cc.s

CMakeFiles/test_thread.dir/iomanager.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/iomanager.cc.o.requires

CMakeFiles/test_thread.dir/iomanager.cc.o.provides: CMakeFiles/test_thread.dir/iomanager.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/iomanager.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/iomanager.cc.o.provides

CMakeFiles/test_thread.dir/iomanager.cc.o.provides.build: CMakeFiles/test_thread.dir/iomanager.cc.o


# Object files for target test_thread
test_thread_OBJECTS = \
"CMakeFiles/test_thread.dir/test_thread.cc.o" \
"CMakeFiles/test_thread.dir/thread.cc.o" \
"CMakeFiles/test_thread.dir/scheduler.cc.o" \
"CMakeFiles/test_thread.dir/mutex.cc.o" \
"CMakeFiles/test_thread.dir/utils.cc.o" \
"CMakeFiles/test_thread.dir/fiber.cc.o" \
"CMakeFiles/test_thread.dir/iomanager.cc.o"

# External object files for target test_thread
test_thread_EXTERNAL_OBJECTS =

../bin/test_thread: CMakeFiles/test_thread.dir/test_thread.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/thread.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/scheduler.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/mutex.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/utils.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/fiber.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/iomanager.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/build.make
../bin/test_thread: CMakeFiles/test_thread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/WorkSpace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable ../bin/test_thread"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_thread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_thread.dir/build: ../bin/test_thread

.PHONY : CMakeFiles/test_thread.dir/build

CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/test_thread.cc.o.requires
CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/thread.cc.o.requires
CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/scheduler.cc.o.requires
CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/mutex.cc.o.requires
CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/utils.cc.o.requires
CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/fiber.cc.o.requires
CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/iomanager.cc.o.requires

.PHONY : CMakeFiles/test_thread.dir/requires

CMakeFiles/test_thread.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_thread.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_thread.dir/clean

CMakeFiles/test_thread.dir/depend:
	cd /root/WorkSpace/sylar/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/WorkSpace/sylar /root/WorkSpace/sylar /root/WorkSpace/sylar/build /root/WorkSpace/sylar/build /root/WorkSpace/sylar/build/CMakeFiles/test_thread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_thread.dir/depend
