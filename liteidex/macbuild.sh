#!/bin/sh

function process_file()
{
install_name_tool -change \
 QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 LiteIDE.app/Contents/PlugIns/$deploy_file  

install_name_tool -change \
 QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  LiteIDE.app/Contents/PlugIns/$deploy_file   
}

function process_file2()
{
install_name_tool -change \
 QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 LiteIDE.app/Contents/PlugIns/$deploy_file  

install_name_tool -change \
 QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  LiteIDE.app/Contents/PlugIns/$deploy_file   
  
install_name_tool -change \
 QtXml.framework/Versions/4/QtXml \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
  LiteIDE.app/Contents/PlugIns/$deploy_file   
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
process_file

export deploy_file=libgolangcode.dylib
process_file
