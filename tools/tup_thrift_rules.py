#!/usr/bin/env python

import optparse
import os
import sys

# Tup complains about .pyc files created during parsing
sys.dont_write_bytecode = True

from thrift_common import *

usage = '''\
Writes out tup rules for processing .thrift files, optionally
using an external wrapper around the thrift compiler (to do
additional generation steps).

   %prog [options] input.thrift\
'''

options = None
args = None

def parse_args():
    parser = optparse.OptionParser(usage=usage)
    
    # C++ options
    parser.add_option('--cpp_cmd', default='thrift --gen cpp --out . %f',
                      help='thrift command for C++ generation')
    parser.add_option('--cpp_obj_bin', default='',
                      help='tup bin for C++ .o files')
    parser.add_option('--cpp_macro', default='!cxx',
                      help='tup macro for compiling C++ files to .o files')
    parser.add_option('--link_macro', default='!ld',
                      help='tup macro for linking .o files into executables')
    
    # Java options
    parser.add_option('--java_cmd', default='thrift --gen java --out . %f',
                      help='thrift command for Java generation')
    
    global options, args
    (options, args) = parser.parse_args()
    
    if len(args) != 1:
        parser.error('incorrect number of arguments')

def tup_rule(inputs, order_inputs, cmd, outputs):
    if not isinstance(inputs, basestring):
        inputs = ' '.join(inputs)
    if not isinstance(order_inputs, basestring):
        order_inputs = ' '.join(order_inputs)
    if not isinstance(cmd, basestring):
        cmd = ' '.join(cmd)
    if not isinstance(outputs, basestring):
        outputs = ' '.join(outputs)
    print ': %s | %s |> %s |> %s' % (inputs, order_inputs, cmd, outputs)

def cpp_rules(thrift_file, basename, structs, services):
    outputs = join_outputs(cpp_output_files(basename, structs, services))
    tup_rule(thrift_file, '', options.cpp_cmd, outputs)
    
    # compile all but the server skeleton
    to_compile = [ o for o in outputs if (o.endswith('.cpp') and not o.endswith('_server.skeleton.cpp')) ]
    tup_rule(['foreach'] + to_compile, outputs, options.cpp_macro, options.cpp_obj_bin)
    
    common_objs = [ os.path.splitext(o)[0] + '.o' for o in to_compile ]
    
    # compile the server skeleton on its own, so it can be used for testing
    for skeleton in [ o for o in outputs if o.endswith('_server.skeleton.cpp') ]:
        skeleton_base, ext = os.path.splitext(skeleton)
        obj_file = skeleton_base + '.o'
        tup_rule(skeleton, outputs, options.cpp_macro, obj_file)
        tup_rule(common_objs + [obj_file], '', options.link_macro, skeleton_base)

def java_rules(thrift_file, basename, structs, services):
    outputs = join_outputs(java_output_files(basename, structs, services))
    tup_rule(thrift_file, '', options.java_cmd, outputs)
    
    # don't compile java files; leave that up to the Tupfile so that
    # they all get put into one .jar

def main():
    thrift_file = args[0]
    path, filename = os.path.split(thrift_file)
    basename, extension = os.path.splitext(filename)
    
    structs, services = parse_thrift_file(thrift_file)
    
    cpp_rules(thrift_file, basename, structs, services)
    java_rules(thrift_file, basename, structs, services)

if __name__ == '__main__':
    parse_args()
    main()

