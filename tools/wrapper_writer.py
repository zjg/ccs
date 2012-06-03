#!/usr/bin/env python

import optparse
import os
import textwrap
import sys

usage = textwrap.dedent('''\
    Creates a wrapper script for an executable, that
    sets environment variables before running the executable.
    
       %prog [options] exe_to_wrap wrapper_output\
    ''')

options = None
args = None

def parse_args():
    parser = optparse.OptionParser(usage=usage)
    
    parser.add_option('-L', action='append', dest='lib_paths',
                      help='paths for LD_LIBRARY_PATH')
    parser.add_option('-l', action='append',
                      help='ignored; provided for compatibility with LDFLAGS')
    
    global options, args
    (options, args) = parser.parse_args()
    
    if len(args) != 2:
        parser.error('incorrect number of arguments')

wrapper_base = textwrap.dedent('''\
    #!/bin/bash
    %(env_vars)s
    %(cd)s
    ./%(exe_file)s $*
    ''')

def main():
    exe_to_wrap = args[0]
    wrapper_output = args[1]
    
    wrapper_dir, wrapper_file = os.path.split(wrapper_output)
    
    exe_relative_to_wrapper = os.path.relpath(exe_to_wrap, wrapper_dir)
    exe_dir, exe_file = os.path.split(exe_relative_to_wrapper)
    
    env_vars = ['DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"']
    
    if options.lib_paths:
       added_lib_paths = set()
       for lib_dir in options.lib_paths:
           abs_path = os.path.abspath(lib_dir)
           if abs_path not in added_lib_paths:
               added_lib_paths.add(abs_path)
               env_vars.append('LD_LIBRARY_PATH="%s:$LD_LIBRARY_PATH"' % (abs_path,))
    
    cd_cmd = 'cd "$DIR"'
    if exe_dir:
        cd_cmd = '%s && cd %s' % (cd_cmd, exe_dir,)
    
    out_file = open(wrapper_output, 'w')
    out_file.write(wrapper_base %
                    {'env_vars':'\n'.join(env_vars),
                     'cd':cd_cmd,
                     'exe_file':exe_file,
                    })
    os.chmod(wrapper_output, 0755)

if __name__ == '__main__':
    parse_args()
    main()

