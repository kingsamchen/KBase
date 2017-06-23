#!/usr/bin/python
# -*- coding: utf-8 -*-

import argparse
import os
import shlex
import subprocess

def str2args(cmd):
    return shlex.split(cmd)


def build_component(build_type, build_dir, cmake_file_path, target):
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    os.chdir(build_dir)

    subprocess.call(str2args(
        'cmake -DCMAKE_BUILD_TYPE={0} -G "Unix Makefiles" {1}'.format(build_type, cmake_file_path)
    ))
    subprocess.call(str2args(
        'cmake --build {0} --target {1} -- -j 4'.format(build_dir, target)
    ))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-type', dest='build_type', type=str,
                        default='Debug')
    parser.add_argument('--build-gtest-only', dest='build_gtest_only', type=bool,
                        default=False)
    args = parser.parse_args()

    build_type = args.build_type.capitalize()

    if build_type.lower() == 'debug':
        print('Build in Debug mode')
    elif build_type.lower() == 'release':
        print('Build in Release mode')
    else:
        print("Error: unrecognized build mode " + build_type)
        return

    project_root = os.path.dirname(os.path.abspath(__file__))

    gtest_build_dir = os.path.join(project_root, 'build', build_type, 'gtest')
    gtest_cmake_path = os.path.join(project_root, 'test', 'third-party')

    build_component(build_type, gtest_build_dir, gtest_cmake_path, 'gtest')

    if args.build_gtest_only:
        return

    if not os.path.exists(os.path.join(project_root, 'build', build_type, 'libkbase.a')):
        print('No lib kbase found! Please build libkbase first!')
        return

    test_build_dir = os.path.join(project_root, 'build', build_type, 'test')
    test_cmake_path = os.path.join(project_root, 'test')

    build_component(build_type, test_build_dir, test_cmake_path, 'kbase_test')


if __name__ == '__main__':
    main()
