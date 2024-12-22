#!/bin/fish

if test (count $argv) -eq 0
  echo "Error: no arguments passed." ^&2
  echo "Syntax: ./build.sh <program_name> <build_mode>." ^&2
  exit 1
end

set program_name $argv[1]
set default_build_mode "debug"
if test (count $argv) -ge 2
  set build_mode $argv[2]
else
  set build_mode $default_build_mode
end

set flags "-std=c99" "-D_POSIX_C_SOURCE=200112L" "-DPROJECT_DIR=\"/home/qamosu/dev/c/x11_wm_from_scratch\"" "-Wall" "-Wextra"
if test "$build_mode" = "debug"
  set debug_flags "-DDEBUG_BUILD" "-O0" "-g" "-DDEBUG" "-fsanitize=address" #address,memory,thread
  set flags $flags $debug_flags
else if test "$build_mode" = "release"
  set release_flags "-DRELEASE_BUILD" "-O3" "-DNDEBUG" "-flto" "-fwhole-program-vtables" "-march=native" "-s"
  set flags $flags $release_flags
else
  echo "Error: build mode is invalid." ^&2
  exit 1
end

set sources ""
if test "$program_name" = "wm"
  set sources "wm/main.c"
else
  echo "Error: program name is invalid." ^&2
  exit 1
end

set build_folder "build"
if not test -d $build_folder
    mkdir $build_folder
    echo "Folder '$build_folder' created."
end

echo "Compiling $program_name..."
time clang $flags $sources -o "build/$program_name" || exit
