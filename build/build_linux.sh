#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

echo build liteide 
echo QTDIR=$QTDIR
echo GOROOT=$GOROOT
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $QTDIR ]; then
	echo 'error, QTDIR is null'
	exit 1
fi

echo qmake liteide ...
echo .
qmake $LITEIDE_ROOT

if [ $? -ge 1 ]; then
	echo 'error, qmake fail'
	exit 1
fi

echo make liteide ...
echo .
make

if [ $? -ge 1 ]; then
	echo 'error, make fail'
	exit 1
fi


echo build liteide tools ...
cd $LITEIDE_ROOT
export GOPATH=$PWD
go install -v -ldflags -s tools/goastview
go install -v -ldflags -s tools/godocview
go install -v -ldflags -s tools/goexec
go install -v -ldflags -s tools/gopromake

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

echo deploy ...

cd $BUILD_ROOT
mkdir -p liteide
mkdir -p liteide/bin
mkdir -p liteide/share/liteide
mkdir -p liteide/lib/liteide/plugins

cp -v $LITEIDE_ROOT/LICENSE.LGPL liteide
cp -v $LITEIDE_ROOT/LGPL_EXCEPTION.TXT liteide
cp -v $LITEIDE_ROOT/README.TXT liteide

cp $LITEIDE_ROOT/liteide/bin/* liteide/bin
cp $LITEIDE_ROOT/bin/* liteide/bin
cp $LITEIDE_ROOT/liteide/lib/liteide/plugins/*.so liteide/lib/liteide/plugins

cp -r -v $LITEIDE_ROOT/deploy/* liteide/share/liteide/
cp -r -v $LITEIDE_ROOT/os_deploy/linux/* liteide/share/liteide/

cp $QTDIR/lib/libQtCore.so.* liteide/lib/liteide
cp $QTDIR/lib/libQtGui.so.* liteide/lib/liteide
cp $QTDIR/lib/libQtXml.so.* liteide/lib/liteide
