@echo off

set BUILD_ROOT=%CD%
if not x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex
set OLD_GOPATH=%GOPATH%

echo build liteide 
echo QTDIR=%QTDIR%
echo GOROOT=%GOROOT%
echo BUILD_ROOT=%BUILD_ROOT%
echo LITEIDE_ROOT=%LITEIDE_ROOT%
echo .

if x%QTDIR%==x goto fail

echo qmake liteide ...
echo .
call qmake ../liteidex

echo make liteide ...
echo .
call make release

echo build liteide tools
echo .

cd ../liteidex
set GOPATH=%CD%
go install -v tools/goastview
go install -v tools/godocview
go install -v tools/goexec
go install -v tools/gopromake

cd %BUILD_ROOT%

echo deploy liteide ...
echo .

if not exist liteide mkdir liteide
if not exist liteide\bin mkdir liteide\bin
if not exist liteide\lib mkdir liteide\lib
if not exist liteide\lib\liteide mkdir liteide\lib\liteide
if not exist liteide\lib\liteide\plugins mkdir liteide\lib\liteide\plugins

xcopy /y /i %LITEIDE_ROOT%\LICENSE.LGPL liteide
xcopy /y /i %LITEIDE_ROOT%\LGPL_EXCEPTION.TXT liteide
xcopy /y /i %LITEIDE_ROOT%\README.TXT liteide

xcopy /y /i %LITEIDE_ROOT%\liteide\bin\* liteide\bin
xcopy /y /i %LITEIDE_ROOT%\bin\* liteide\bin
xcopy /y /i %LITEIDE_ROOT%\liteide\lib\liteide\plugins\*.dll liteide\lib\liteide\plugins

xcopy /e /y /i %LITEIDE_ROOT%\deploy liteide\share\liteide
xcopy /e /y /i %LITEIDE_ROOT%\os_deploy\windows liteide\share\liteide

xcopy /y /i %QTDIR%\bin\QtCore4.dll liteide\bin
xcopy /y /i %QTDIR%\bin\QtGui4.dll liteide\bin
xcopy /y /i %QTDIR%\bin\QtXml4.dll liteide\bin
xcopy /y /i %QTDIR%\bin\mingwm10.dll liteide\bin
xcopy /y /i %QTDIR%\bin\libgcc_s_dw2-1.dll liteide\bin

goto end

:fail
echo build fail

:end
set GOPATH=%OLD_GOPATH%