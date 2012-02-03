#!/usr/bin/env python

import atexit
import fnmatch
import optparse
import os
import shutil
import subprocess
import sys
import tempfile

usage = '''\
Converts a Protocol Buffers .proto file to a Java .jar file.

   %prog [options] input.proto [input2.proto ...]\
'''

options = None
args = None

def find_files(root_dir, file_glob):
    files = []
    for root, dirnames, filenames in os.walk(root_dir):
        for filename in fnmatch.filter(filenames, file_glob):
            files.append(os.path.join(root, filename))
    return files

def parse_args():
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('-o', '--output', dest='jarfile', default='proto.jar',
                      help='output .jar file')
    parser.add_option('-c', '--classpath', dest='classpath',
                      help='java classpath')
    parser.add_option('--protoc', dest='protoc_bin', default='protoc',
                      help='protoc path')
    parser.add_option('--javac', dest='javac_bin', default='javac',
                      help='javac path')
    parser.add_option('--jar', dest='jar_bin', default='jar',
                      help='jar path')
    
    global options, args
    (options, args) = parser.parse_args()
    
    if len(args) < 1:
        parser.error('need at least one .proto file as input')

def main():
    proto_files = [ os.path.abspath(a) for a in args ]
    
    # make temp dir & register cleanup
    tmp_dir = tempfile.mkdtemp()
    def clean_tmp():
        shutil.rmtree(tmp_dir)
    atexit.register(clean_tmp)
    
    # run protoc
    protoc_includes = [ '-I' + os.path.dirname(f) for f in proto_files ]
    protoc_includes = list(set(protoc_includes))    # uniqify
    protoc_cmd = [ options.protoc_bin, '--java_out=' + tmp_dir ]
    protoc_cmd.extend(protoc_includes)
    protoc_cmd.extend(proto_files)
    subprocess.check_call(protoc_cmd)
    
    # compile java files
    java_files = find_files(tmp_dir, '*.java')
    javac_cmd = [ options.javac_bin, '-cp', options.classpath ]
    javac_cmd.extend(java_files)
    subprocess.check_call(javac_cmd)
    
    # build jar
    class_files = find_files(tmp_dir, '*.class')
    class_files = [ os.path.relpath(f, tmp_dir) for f in class_files ]
    jar_cmd = [ options.jar_bin, 'cf', os.path.abspath(options.jarfile) ]
    jar_cmd.extend(class_files)
    subprocess.check_call(jar_cmd, cwd=tmp_dir)

if __name__ == '__main__':
    parse_args()
    main()

