ccs - clang completion server

Goal is to have a code-completion engine powered by clang
that isn't tied to any editor/IDE. Idealy it will work over
the network, such that the editor using the code-completion
doesn't need to be the same one that is running ccs (but ccs
should/would need to be on the same machine as the code itself).


Dependencies:
   - tup (latest git)
   - llvm/clang v3.0 (or latest svn)
   - Qt 4.6 or 4.7
   - thrift 0.8
   - inotify-tools
      - https://github.com/zjg/inotify-tools
   - clucene (latest git)



