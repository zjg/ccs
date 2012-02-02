#!/usr/bin/env python

__all__ = ['parse_thrift_file', 'join_outputs',
           'cpp_output_files', 'java_output_files'
          ]

import re

def parse_thrift_file(f):
    ''' returns (structs, services) '''
    structs = set()
    services = set()
    
    struct_re = re.compile(r'^struct (\w+)')
    service_re = re.compile(r'^service (\w+)')
    
    for line in open(f):
        struct_match = struct_re.match(line)
        if struct_match:
            structs.add(struct_match.group(1))
            continue
        
        service_match = service_re.match(line)
        if service_match:
            services.add(service_match.group(1))
            continue
    
    return (structs, services)
    

def join_outputs(tupl):
    ''' joins the outputs in the given tuple into one big list '''
    out = []
    for t in tupl:
        out.extend(t)
    return out

def cpp_output_files(basename, structs, services):
    ''' returns (standard_files, struct_files, service_files) '''
    standard_outputs = [ '%s_constants.cpp', '%s_constants.h',
                         '%s_types.cpp', '%s_types.h' ]
    struct_outputs = []
    service_outputs = [ '%s.cpp', '%s.h', '%s_server.skeleton.cpp' ]
    
    return transform_output_files(basename, structs, services,
                                  standard_outputs, struct_outputs, service_outputs)

def java_output_files(basename, structs, services):
    ''' returns (standard_files, struct_files, service_files) '''
    standard_outputs = []
    struct_outputs = [ '%s.java' ]
    service_outputs = [ '%s.java' ]
    
    return transform_output_files(basename, structs, services,
                                  standard_outputs, struct_outputs, service_outputs)

def transform_output_files(basename, structs, services,
                           standard_outputs, struct_outputs, service_outputs):
    ''' returns (standard_files, struct_files, service_files) '''
    
    standard_files = [ o % (basename,) for o in standard_outputs ]
    
    struct_files = []
    for s in structs:
        struct_files.extend([ o % (s,) for o in struct_outputs ])
    
    service_files = []
    for s in services:
        service_files.extend([ o % (s,) for o in service_outputs ])
    
    return (standard_files, struct_files, service_files)

