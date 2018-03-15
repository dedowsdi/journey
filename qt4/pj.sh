#!/bin/bash
vim=nvim
session=
pjvim=

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
$vim --cmd "source ./.vim/pj.vim"
