import argparse
import sys
import os
import subprocess
import multiprocessing
#import codecs

subcommands = ['build']
modules     = ['multi_core','bmp','ann']
def add_cmake_args(cmake_cmd, args):
    if args.release != None:
        if not os.path.exists(args.release):
            os.makedirs(args.release)
        #invoke cmake with args
        print("Got release")

    if args.static == True:
        cmake_cmd.append("-DBuildStatic=ON")
        print ("Got static")
    if args.shared == True:
        cmake_cmd.append("-DBuildShared=ON")
        print ("Got shared")
    if args.bench_marks == True:
        cmake_cmd.append("-DBuildBenchmarks=ON")
    else:
        cmake_cmd.append("-DBuildBenchmarks=OFF")
    if args.cuda == True:
        cmake_cmd.append("-DBuildCuda=ON")
        print ("Got cuda")
    else:
        print ("Not cuda")
        cmake_cmd.append("-DBuildCuda=OFF")
    if args.code_coverage == True:
        cmake_cmd.append("-DZinhartUseCodeCoverage=ON")
        print("Got code_coverage")
    else:
        print("Not code_coverage")
        cmake_cmd.append("-DZinhartUseCodeCoverage=OFF")

def handle_build(args):
    print(args)
    if args.list == True:
        print (modules)
        sys.exit()
    
    cmake = None
    cmake_cmd = []
    make_cmd = []
    shell = None

    if args.debug != None:
        if not os.path.exists(args.debug):
            os.makedirs(args.debug)
        shell = None
        if sys.platform.startswith('win'):
            # to do windows specific code
            cmake = "cmake.exe"
            shell = True
        else:
            cwd   = os.getcwd()
            build_dir = cwd + "/" + args.debug
            cmake_cmd.append("cmake")
            cmake_cmd.append("-B" + build_dir)
            cmake_cmd.append("-H" + cwd)
            cmake_cmd.append("-DCMAKE_BUILD_TYPE=DEBUG")
            shell = False
            make_cmd.append("make")
            make_cmd.append("-C" + build_dir)
            if args.threads != None:
                make_cmd.append("-j" + str(args.threads))

        print(cmake_cmd)
        add_cmake_args(cmake_cmd, args)
        ret = subprocess.check_call(cmake_cmd, shell, stderr=subprocess.STDOUT)
        make = subprocess.check_call(make_cmd, shell, stderr=subprocess.STDOUT)

    if args.clean == True:
        print ("Got clean")




if __name__ == '__main__':
    main_parser = argparse.ArgumentParser(prog = 'ZINHART')
    main_parser.add_argument('-l', '--list', help ='list subcommands')
    subparsers = main_parser.add_subparsers(help='sub-command help')
    
    # build subcommand
    build_parser = subparsers.add_parser('build', help='build help')
    build_parser.add_argument('-l', '--list',              action = 'store_true', help = 'list submodules of zinhart')
    build_parser.add_argument('-d', '--debug',   type=str,                        help = 'build zinhart in debug mode')
    build_parser.add_argument('-r', '--release', type=str,                        help = 'build zinhart in release mode')
    build_parser.add_argument('-st', '--static',           action = 'store_true', help = 'build zinhart as a static library')
    build_parser.add_argument('-sh', '--shared',           action = 'store_true', help = 'build zinhart as a shared library')
    build_parser.add_argument('-b', '--bench_marks',       action = 'store_true', help = 'build zinhart with with benchmarks')
    build_parser.add_argument('-cu', '--cuda',             action = 'store_true', help = 'build zinhart with cuda enabled')
    build_parser.add_argument('-cc', '--code_coverage',    action = 'store_true', help = 'clean zinhart build directories')
    build_parser.add_argument('-t', '--threads', type=int, choices = range(1, multiprocessing.cpu_count() + 1), help = 'number of threads to use during make')

    build_parser.add_argument('-c', '--clean',             action = 'store_true', help = 'clean zinhart build directories')
    
    if len(sys.argv) == 1:
        #main_parser.print_help()
        #build_parser.print_help()
        sys.exit()

    args = main_parser.parse_args()
   # args = build_parser.parse_args()

  #  if args.command == 'build':
   #     print ("Gotcha")
    #    sys.exit()
    handle_build(args)
