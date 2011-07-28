@echo on
copy LICENSE.LGPL liteide
copy LGPL_EXCEPTION.TXT liteide
xcopy deploy liteide  /e /y /i
xcopy os_deploy\windows liteide /e /y  /i