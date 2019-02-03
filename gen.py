#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 0xCCCCCCCC

import argparse
import os
import platform
import shlex
import shutil
import subprocess

from os import path

PROJECT_ROOT = path.dirname(path.abspath(__file__))
CMAKE_ROOT = path.dirname(path.abspath(__file__))

# Ordered in precedence.
# And we assume make is always available on any *nix system.
BUILD_SYSTEMS = (
    ('ninja', 'Ninja'),
    ('make', 'Unix Makefiles'),
)


def is_on_windows():
    return platform.system() == 'Windows'


def run(cmd):
    subprocess.call(shlex.split(cmd))


def find_program(exe):
    return subprocess.call(['which', exe],
                           stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT) == 0


def search_preferred_build_system():
    print('* Auto-search building system...')
    for tool, gen_name in BUILD_SYSTEMS:
        if find_program(tool):
            print('* {} found and would be used'.format(tool))
            return gen_name


def boolean_to_option(value):
    return 'ON' if value else 'OFF'


def run_gen_on_posix(args):
    build_type = args.build_type.capitalize()

    our_dir_name = args.out_dir if args.out_dir else build_type
    out_dir = path.join(PROJECT_ROOT, 'build', our_dir_name)

    if args.rebuild:
        print('* Rebuild mode, clean first...')
        if path.exists(out_dir):
            shutil.rmtree(out_dir, True)

    if not path.exists(out_dir):
        os.makedirs(out_dir)

    os.chdir(out_dir)

    generator = args.generator if args.generator else search_preferred_build_system()

    run('cmake'
        ' -DCMAKE_BUILD_TYPE={}'
        ' -DKBASE_BUILD_UNITTESTS={}'
        ' -G "{}"'
        ' {}'
        .format(build_type,
                boolean_to_option(not args.build_no_test),
                generator,
                CMAKE_ROOT))

    building_thread_count = os.cpu_count() + 2

    run('cmake'
        ' --build {}'
        ' -- -j {}'
        .format(out_dir, building_thread_count))


def run_gen_on_windows(args):
    OUR_DIR_NAME = 'Windows'
    out_dir = path.join(PROJECT_ROOT, 'build', OUR_DIR_NAME)

    if not path.exists(out_dir):
        os.makedirs(out_dir)

    os.chdir(out_dir)

    MSVC_2017_X64 = 'Visual Studio 15 2017 Win64'
    generator = args.generator if args.generator else MSVC_2017_X64

    run('cmake'
        ' -DKBASE_BUILD_UNITTESTS={}'
        ' -G "{}"'
        ' "{}"'
        .format(boolean_to_option(not args.build_no_test),
                generator,
                CMAKE_ROOT))

    if args.skip_build:
        print('* Run configuration and generation only, skip building')
        return

    run('cmake'
        ' --build "{}"'
        ' --target ALL_BUILD'
        ' --config {}'
        .format(out_dir, args.build_type))


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('--build-type', dest='build_type', type=str, default='Debug')
    parser.add_argument('--out-dir', dest='out_dir', type=str)
    parser.add_argument('--generator', dest='generator', type=str)
    parser.add_argument('--rebuild', dest='rebuild', action='store_true')

    parser.add_argument('--no-test', dest='build_no_test', action='store_true')

    if is_on_windows():
        parser.add_argument('--no-build', dest='skip_build', action='store_true',
                            help='do not build target after generation')

    return parser.parse_args()


def main():
    args = parse_args()
    if is_on_windows():
        run_gen_on_windows(args)
    else:
        run_gen_on_posix(args)


if __name__ == '__main__':
    main()
