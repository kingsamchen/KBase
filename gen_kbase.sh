#!/bin/bash

build_type="Debug"

if [ $# -eq 0 ]; then
    echo "No build type specified. Use Debug as default."
else
    if [ ${1,,} = "debug" ]; then
        echo 'Build in Debug mode.'
    elif [ ${1,,} = "release" ]; then
        build_type="Release"
        echo 'Build in Release mode.'
    else
        echo 'Error: Unidentified build type!'
        exit
    fi
fi

output_path="build/"${build_type}
readonly output_path
phantom_path="build/"${build_type}"/phantom"
readonly phantom_path

kbase_obj_dir=${phantom_path}"/kbase"
if [ ! -d $kbase_obj_dir ]; then
    mkdir -p $kbase_obj_dir
fi

sh_root=`pwd`
cmake_file_path=$sh_root"/src"
cd $kbase_obj_dir
cmake -D CMAKE_CXX_COMPILER="/usr/bin/clang++" CMAKE_BUILD_TYPE=$build_type $cmake_file_path
make
