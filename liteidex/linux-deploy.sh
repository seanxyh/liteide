#!/bin/sh
cp -v LICENSE.LGPL liteide
cp -v LGPL_EXCEPTION.TXT liteide
cp -v README.TXT liteide
cp -v CHANGES.TXT liteide
cp -v GOCODE.TXT liteide
cp -r -v deploy/* liteide/
cp -r -v os_deploy/linux/* liteide/

