#!/usr/bin/python3
# -*- coding: utf-8 -*-
# 0xCCCCCCCC

import argparse
import os
import shlex
import shutil
import subprocess

from distutils.util import strtobool
from os import path

PROJECT_ROOT = path.dirname(path.abspath(__file__))
CMAKE_ROOT = path.dirname(path.abspath(__file__))

# Ordered in precedence.
# And we assume make is always available on any *nix system.
BUILD_SYSTEMS = (
    ('ninja', 'Ninja'),
    ('make', 'Unix Makefiles'),
)

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

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--build-type', dest='build_type', type=str, default='Debug')
    parser.add_argument('--out-dir', dest='out_dir', type=str)
    parser.add_argument('--generator', dest='generator', type=str)
    parser.add_argument('--rebuild', dest='rebuild', action='store_true')

    parser.add_argument('--no-test', dest='build_no_test', action='store_true')

    args = parser.parse_args()

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
        ' -DBUILD_NO_UNITTEST={}'
        ' -G "{}"'
        ' {}'
        .format(build_type,
                args.build_no_test,
                generator,
                CMAKE_ROOT))

    building_thread_count = os.cpu_count() + 2

    run('cmake'
        ' --build {}'
        ' -- -j {}'
        .format(out_dir, building_thread_count))


if __name__ == '__main__':
    main()
