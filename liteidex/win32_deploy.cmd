@echo on
copy LICENSE.LGPL build
copy LGPL_EXCEPTION.TXT build
copy README.TXT build
copy CHANGES.TXT build 
copy GOCODE.TXT build
mkdir build\data\translations
xcopy liteide_zh.qm build\data\translations /y
xcopy deploy build  /e /y /i
xcopy os_deploy\win32 build /e /y  /i
pause