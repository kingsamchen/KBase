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

libgtest_path=${output_path}"/libgtest.a"
if [ ! -e $libgtest_path ]; then
    gtest_int_dir=${phantom_path}"/gtest"
    if [ ! -d $gtest_int_dir ]; then
        mkdir -p $gtest_int_dir
    fi

    sh_root=`pwd`
    cmake_file_path=$sh_root"/test/third-party"
    cd $gtest_int_dir
    cmake -D CMAKE_BUILD_TYPE=$build_type $cmake_file_path
    make
else
    echo "lib gtest is already updated; nothing to build"
fi