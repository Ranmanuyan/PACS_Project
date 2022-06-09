#!/bin/sh

VERSION=$(grep AC_INIT ../configure.ac|sed -e "s/^[^\[]*\[[^\[]*\[//" -e "s/\].*//")
FILE=bobpp-$VERSION
PREFIX=$PWD/$FILE
W32PATH=$HOME/local/win32

export C_INCLUDE_PATH=$W32PATH/include
export CPLUS_INCLUDE_PATH=$W32PATH/include
export LD_LIBRARY_PATH=$W32PATH/lib
export LD_RUN_PATH=$W32PATH/lib

echo C_INCLUDE_PATH=$C_INCLUDE_PATH
echo CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo LD_RUN_PATH=$LD_RUN_PATH

cd ..
sh bootstrap || exit 1

./configure --prefix=$PREFIX --host=i586-mingw32msvc --disable-static \
	--with-glop=no --with-mpi=no --with-kaapi=no --with-giw=no || exit 1

exit 0

make || exit 1

make install || exit 1

cat << _EOF > $PREFIX/$FILE.DevPackage
[Setup]
Version=1
AppName=Bob++
AppVersion=$VERSION
AppVerName=Bob++
MenuName=bobpp
Description=The Bob++ Library is a set of C++ classes. Its goal is to allow easy implementation of sequential and parallel search algorithms (Branch and X, Dynamic programming, etc) to solve your own problems.
Url=https://software.prism.uvsq.fr/bobpp/
License=COPYING
Readme=README
Dependencies=pthread_w32

[Files]
include=<app>\include\
lib=<app>\lib\
Templates=<app>\Templates\
docs=<app>\docs\
bin=<app>\bin\
_EOF

cp README COPYING $PREFIX

tar cf - $FILE | bzip2 -c9 > $FILE.DevPak
