ccs - clang completion server

Goal is to have a code-completion engine powered by clang
that isn't tied to any editor/IDE. Idealy it will work over
the network, such that the editor using the code-completion
doesn't need to be the same one that is running ccs (but ccs
should/would need to be on the same machine as the code itself).

