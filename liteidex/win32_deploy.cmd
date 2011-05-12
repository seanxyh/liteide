@echo on
copy LICENSE.LGPL build
copy LGPL_EXCEPTION.TXT build
copy README.TXT build
copy CHANGES.TXT build 
xcopy deploy build  /e /y /i
xcopy os_deploy\win32 build /e /y  /i
pause