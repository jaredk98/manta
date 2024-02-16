#!/bin/bash

# usage: boot.bat -project=<project> -os=<os> -architecture=<architecture> -toolchain=<toolchain> -config=<config> -gfx=<api> -clean=<0/1> -run=<0/1>
	# -os=             windows, linux, macos
	# -architecture=   x64, arm32, arm64
	# -toolchain=      msvc, llvm, gnu
	# -config=         release, debug

args="$@"
toolchain="llvm"
project=""

parse_args()
{
    # Parse command-line arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -project=*)
                project="${1#*=}"
                ;;
            -toolchain=*)
                toolchain="${1#*=}"
                ;;
            *)
                # Skip unneeded args
                ;;
        esac
        shift
    done

    # Goto build
    build
}

build()
{
    # Clear Terminal
    clear
    echo -e "\033c"

    # Require a project
    if [ -z "$project" ]; then
        echo "Bootstrap failed: No -project specified"
        return
    fi

    # Ensure project exists
    if [ ! -d "projects/$project" ]; then
        echo "Bootstrap failed: project '$project' does not exist!"
        return
    fi

    # Skip this step if boot executable already exists
    if [ -f "projects/$project/output/boot/boot" ]; then
        strap
        return
    fi

    # Make sure the projects/$project/output/boot/objects directory exists
    mkdir -p "projects/$project/output/boot/objects"

    # MSVC isn't supported on macOS & linux
    if [ "$toolchain" == "msvc" ]; then
        echo "Bootstrap failed: msvc toolchain not supported on this platform"
        return
    fi

    # Call the compiler to make boot executable
    echo "Boot: $project, $toolchain [$args]"
    if [ "$toolchain" == "llvm" ]; then
        clang "source/boot/boot.cpp" -std=c++20 -fno-exceptions -I"source" -I"source/boot" -o "projects/$project/output/boot/boot"
    elif [ "$toolchain" == "gnu" ]; then
        g++ "source/boot/boot.cpp" -std=c++20 -fno-exceptions -I"source" -I"source/boot" -o "projects/$project/output/boot/boot"
    fi

    # Verify that the executable was created successfully
    if [ $? -eq 0 ]; then
        echo "Boot: compile successful"
        chmod +x "projects/$project/output/boot/boot"  # Make the executable executable
        strap
    else
        echo "Boot: compile failed"
        return
    fi
}

strap()
{
    "./projects/$project/output/boot/boot" $args
}

# Parse Args (entry point)
parse_args "$@"