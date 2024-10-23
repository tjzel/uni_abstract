#!/bin/sh
CLANG_PATH=/opt/homebrew/Cellar/llvm/19.1.1/bin
export PATH=$CLANG_PATH:$PATH
export CC=$CLANG_PATH/clang
export CXX=$CLANG_PATH/clang++

export PS1="(figures) $PS1"
