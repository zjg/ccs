#!/usr/bin/env python

import optparse
import os
import re

usage = '''\
Writes out tup rules for processing .thrift files.

   %prog [options] input.thrift\
'''

options = None
args = None

def parse_args():
    parser = optparse.OptionParser(usage=usage)
    
    global options, args
    (options, args) = parser.parse_args()
    
    if len(args) != 1:
        parser.error('incorrect number of arguments')

def tup_rule(inputs, order_inputs, cmd, outputs):
    print ': %s | %s |> %s |> %s' % (inputs, order_inputs, cmd, outputs)

def main():
    thrift_file = args[0]
    path, filename = os.path.split(thrift_file)
    basename, extension = os.path.splitext(filename)
    
    structs = set()
    services = set()
    
    # parse thrift file to find structs & services
    struct_re = re.compile(r'^struct (\w+)')
    service_re = re.compile(r'^service (\w+)')
    
    for line in open(thrift_file):
        struct_match = struct_re.match(line)
        if struct_match:
            structs.add(struct_match.group(1))
            continue
        
        service_match = service_re.match(line)
        if service_match:
            services.add(service_match.group(1))
            continue
    
    
    # cpp output
    cpp_standard_outputs = [ '%s_constants.cpp', '%s_constants.h',
                              '%s_types.cpp', '%s_types.h' ]
    cpp_service_outputs = [ '%s.cpp', '%s.h', '%s_server.skeleton.cpp' ]
    cpp_struct_outputs = []
    
    outputs = [ o % (basename,) for o in cpp_standard_outputs ]
    for s in services:
        outputs.extend([ o % (s,) for o in cpp_service_outputs])
    
    tup_rule(thrift_file, '', 'thrift --gen cpp --out . %f', ' '.join(outputs))
    
    
    # java output
    java_standard_outputs = []
    java_service_outputs = [ '%s.java' ]
    java_struct_outputs = [ '%s.java' ]
    
    outputs = [ o % (basename,) for o in java_standard_outputs ]
    for s in services:
        outputs.extend([ o % (s,) for o in java_service_outputs])
    for s in structs:
        outputs.extend([ o % (s,) for o in java_struct_outputs])
    
    tup_rule(thrift_file, '', 'thrift --gen java --out . %f', ' '.join(outputs))
    

if __name__ == '__main__':
    parse_args()
    main()

