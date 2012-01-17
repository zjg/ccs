#!/usr/bin/env python

import socket

import ccs_pb2 as ccs

def main():
    
    port = input('Enter port:')
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', port))
    
    print 'Connected on', port
    
    while 1:
        filename = raw_input('Enter filename for completion:')
        line = input('Line:')
        col = input('Column:')
        
        request = ccs.CCSRequest()
        request.completionRequest.filename = filename
        request.completionRequest.line = line
        request.completionRequest.column = col
        
        s.send(request.SerializeToString())
        


if __name__ == '__main__':
    main()


