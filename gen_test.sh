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

sh_root=`pwd`
readonly sh_root
output_path=${sh_root}"/build/"${build_type}
readonly output_path
phantom_path=${sh_root}"/build/"${build_type}"/phantom"
readonly phantom_path

# We automatically build gtest if necessary, since its content won't change.

libgtest_path=${output_path}"/libgtest.a"
if [ ! -e $libgtest_path ]; then
    gtest_obj_dir=${phantom_path}"/gtest"
    if [ ! -d $gtest_obj_dir ]; then
        mkdir -p $gtest_obj_dir
    fi

    gtest_cmake_file_path=$sh_root"/test/third-party"
    cd $gtest_obj_dir
    cmake -D CMAKE_BUILD_TYPE=$build_type $gtest_cmake_file_path
    make
else
    echo "lib gtest is already updated; nothing to build"
fi

libkbase_path=${output_path}"/libkbase.a"
if [ ! -e ${libkbase_path} ]; then
    echo "Error: No lib kbase found! Please build it first."
    exit
fi

test_obj_dir=${phantom_path}"/test"
if [ ! -d ${test_obj_dir} ]; then
    mkdir -p ${test_obj_dir}
fi

test_cmake_file_path=$sh_root"/test"
cd $test_obj_dir
cmake -D CMAKE_BUILD_TYPE=$build_type $test_cmake_file_path
make

