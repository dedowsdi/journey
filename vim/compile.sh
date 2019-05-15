#!/bin/sh

sudo apt install libncurses5-dev libgnome2-dev libgnomeui-dev \
  libgtk2.0-dev libatk1.0-dev libbonoboui2-dev \
  libcairo2-dev libx11-dev libxpm-dev libxt-dev python-dev \
  python3-dev ruby-dev lua5.1 liblua5.1-dev libperl-dev git

# pay attention to python config dir
#  --with-x \ # cause always has('gui')
./configure \
  --with-features=huge \
  --with-compiledby="dedowsdi <peanutandchestnut@gmail.com>" \
  --enable-gui=auto \
  --enable-multibyte \
  --enable-rubyinterp=yes \
  --enable-pythoninterp=yes \
  --with-python-config-dir=/usr/lib/python2.7/config-x86_64-linux-gnu \
  --enable-python3interp=yes \
  --with-python3-config-dir=/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu \
  --enable-perlinterp=yes \
  --enable-luainterp=yes \
  --enable-cscope \
  --prefix=/usr/local

make VIMRUNTIMEDIR=/usr/local/share/vim/vim81

sudo make install

sudo update-alternatives --install /usr/bin/editor   editor   /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/vi       vi       /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/vim      vim      /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/vimdiff  vimdiff  /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/rvim     rvim     /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/rview    rview    /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/view     view     /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/ex       ex       /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/editor   editor   /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/gvim     gvim     /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/gview    gview    /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/rgview   rgview   /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/rgvim    rgvim    /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/evim     evim     /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/eview    eview    /usr/local/bin/vim 100
sudo update-alternatives --install /usr/bin/gvimdiff gvimdiff /usr/local/bin/vim 100
