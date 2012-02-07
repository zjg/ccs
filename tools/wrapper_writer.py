#!/usr/bin/env python

import optparse
import os
import textwrap

usage = textwrap.dedent('''\
    Creates a wrapper script for an executable, that
    sets environment variables before running the executable.
    
       %prog [options] exe_to_wrap wrapper_output\
    ''')

options = None
args = None

def parse_args():
    parser = optparse.OptionParser(usage=usage)
    
    global options, args
    (options, args) = parser.parse_args()
    
    if len(args) != 2:
        parser.error('incorrect number of arguments')

wrapper_base = textwrap.dedent('''\
    #!/bin/bash
    %(env_vars)s
    %(cd)s
    ./%(exe_file)s
    ''')

def main():
    exe_to_wrap = args[0]
    wrapper_output = args[1]
    
    wrapper_dir, wrapper_file = os.path.split(wrapper_output)
    
    exe_relative_to_wrapper = os.path.relpath(exe_to_wrap, wrapper_dir)
    exe_dir, exe_file = os.path.split(exe_relative_to_wrapper)
    
    env_vars = ['DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"']
    
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

