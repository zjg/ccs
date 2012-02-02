#!/usr/bin/env python

import atexit
import difflib
import fileinput
import optparse
import os
import re
import shutil
import subprocess
import sys
import tempfile

# Tup complains about .pyc files created during parsing
sys.dont_write_bytecode = True

from thrift_common import *

usage = '''\
Runs the thrift compiler to create C++ output, and then does some
additional processing of the output files.

   %prog [options] input.thrift\
'''

options = None
args = None

def parse_args():
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--out', dest='out_dir', default='.',
                      help='final output directory for generated files')
    
    global options, args
    (options, args) = parser.parse_args()
    
    if len(args) != 1:
        parser.error('incorrect number of arguments')

def add_to_files(files, insertion_re, to_add):
    insertion_re = re.compile(insertion_re)
    for line in fileinput.input(files, inplace=1):
        sys.stdout.write(line)
        if insertion_re.match(line):
            sys.stdout.write(to_add)

def main():
    thrift_file = args[0]
    path, filename = os.path.split(thrift_file)
    basename, extension = os.path.splitext(filename)
    
    structs, services = parse_thrift_file(thrift_file)
    std_files, struct_files, service_files = cpp_output_files(basename, structs, services)
    all_files = join_outputs((std_files, struct_files, service_files))
    
    # make temp dir & register cleanup
    tmp_dir = tempfile.mkdtemp()
    def clean_tmp():
        shutil.rmtree(tmp_dir)
    atexit.register(clean_tmp)
    
    # run thrift
    thrift_cmd = [ 'thrift', '--gen', 'cpp',
                   '--out', tmp_dir,
                   thrift_file
                 ]
    subprocess.check_call(thrift_cmd)
    
    # add code to some of the files
    
    # needed for int32_t, etc
    to_add = '#include <stdint.h>\n'
    files_to_modify = [ o for o in all_files if o.endswith('.h') ]
    files_to_modify = [ os.path.join(tmp_dir, o) for o in files_to_modify ]
    add_to_files(files_to_modify, r'^#define [\w_]+_H$', to_add)
    
    # needed for 'htons'
    to_add = '#include <arpa/inet.h>\n'
    files_to_modify = [ o for o in service_files if o.endswith('_server.skeleton.cpp') ]
    files_to_modify = [ os.path.join(tmp_dir, o) for o in files_to_modify ]
    add_to_files(files_to_modify, r'^#include ".*"$', to_add)
    
    
    # copy all the files to their final resting place
    # (can't do any check-if-changed optimization, since tup will hide any existing files)
    for f in all_files:
        src = os.path.join(tmp_dir, f)
        dst = os.path.join(options.out_dir, f)
        shutil.move(src, dst)
    
if __name__ == '__main__':
    parse_args()
    main()

