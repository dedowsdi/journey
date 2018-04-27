#!/bin/bash
vim=nvim

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
$vim --cmd "source ./.vim/init.vim"
