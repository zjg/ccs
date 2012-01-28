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

   %prog [options] input.proto output.jar\
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
    
    if len(args) != 2:
        parser.error('incorrect number of arguments')

def main():
    proto_file = os.path.abspath(args[0])
    jar_file = os.path.abspath(args[1])
    
    # make temp dir & register cleanup
    tmp_dir = tempfile.mkdtemp()
    def clean_tmp():
        shutil.rmtree(tmp_dir)
    atexit.register(clean_tmp)
    
    # run protoc
    protoc_cmd = [ options.protoc_bin,
                   '-I' + os.path.dirname(proto_file),
                   '--java_out=' + tmp_dir,
                   proto_file
                 ]
    subprocess.call(protoc_cmd)
    
    # compile java files
    java_files = find_files(tmp_dir, '*.java')
    javac_cmd = [ options.javac_bin, '-cp', options.classpath ]
    javac_cmd.extend(java_files)
    subprocess.call(javac_cmd)
    
    # build jar
    class_files = find_files(tmp_dir, '*.class')
    jar_cmd = [ options.jar_bin, 'cf', jar_file ]
    jar_cmd.extend(class_files)
    subprocess.call(jar_cmd)

if __name__ == '__main__':
    parse_args()
    main()

