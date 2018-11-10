import argparse
import sys
import os
import subprocess
#import codecs

subcommands = ['build']
modules     = ['ann', 'multi_core']
def handle_build(args):
    print(args)
    if args.list == True:
        print (modules)
        sys.exit()
    
    cmake = None
    cmake_cmd = []
    shell = None


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

    if args.clean == True:
        print ("Got clean")

    if args.debug != None:
        if not os.path.exists(args.debug):
            os.makedirs(args.debug)
        shell = None
        if sys.platform.startswith('win'):
            cmake = "cmake.exe"
            shell = True
        else:
            cwd   = os.getcwd()
            cmake_cmd.insert(0, "cmake")
            cmake_cmd.insert(1, "-B" + cwd + "/" + args.debug)
            cmake_cmd.insert(2, "-H" + cwd)
            cmake_cmd.insert(3, "-DCMAKE_BUILD_TYPE=DEBUG")
            shell = False
        print(cmake_cmd)
        ret = subprocess.check_call(cmake_cmd, shell, stderr=subprocess.STDOUT)
    if args.release != None:
        if not os.path.exists(args.release):
            os.makedirs(args.release)
        #invoke cmake with args
        print("Got release")



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
