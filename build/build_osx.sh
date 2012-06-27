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

cp -r -v $LITEIDE_ROOT/liteide/bin/LiteIDE.app liteide

cp -v $LITEIDE_ROOT/LICENSE.LGPL liteide
cp -v $LITEIDE_ROOT/LGPL_EXCEPTION.TXT liteide
cp -v $LITEIDE_ROOT/README.TXT liteide

cp $LITEIDE_ROOT/bin/* liteide/LiteIDE.app/Contents/MacOSX
cp -r -v $LITEIDE_ROOT/deploy/* liteide/LiteIDE.app/Contents/Resources
cp -r -v $LITEIDE_ROOT/os_deploy/macosx/* liteide/LiteIDE.app/Contents/Resources


cp $QTDIR/lib/libQtCore.dylib.* liteide/lib/liteide
cp $QTDIR/lib/libQtGui.so.* liteide/lib/liteide
cp $QTDIR/lib/libQtXml.so.* liteide/lib/liteide


export QTLIBPATH=$QTDIR/lib

function process_file()
{
 echo "install_name_tool" $deploy_file
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/PlugIns/$deploy_file  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/PlugIns/$deploy_file   
}

function process_file2()
{
  echo "install_name_tool" $deploy_file
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/PlugIns/$deploy_file  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/PlugIns/$deploy_file   
  
install_name_tool -change \
 $QTLIBPATH/QtXml.framework/Versions/4/QtXml \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
  liteide/LiteIDE.app/Contents/PlugIns/$deploy_file   
}

export deploy_file=libgolangast.dylib
process_file

export deploy_file=libgolangfmt.dylib
process_file

export deploy_file=liblitebuild.dylib
process_file

export deploy_file=libliteeditor.dylib
process_file2

export deploy_file=libmakefileproject.dylib
process_file

export deploy_file=libprofileproject.dylib
process_file

export deploy_file=libwelcome.dylib
process_file2

export deploy_file=libgolangcode.dylib
process_file

export deploy_file=libfilebrowser.dylib
process_file

export deploy_file=liblitefind.dylib
process_file

export deploy_file=libgolangdoc.dylib
process_file2

export deploy_file=libliteenv.dylib
process_file

export deploy_file=liblitedebug.dylib
process_file

export deploy_file=libgdbdebugger.dylib
process_file

export deploy_file=libgolangplay.dylib
process_file

export deploy_file=libgolangpackage.dylib
process_file

