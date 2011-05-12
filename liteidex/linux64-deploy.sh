#!/bin/sh
cp -v LICENSE.LGPL build
cp -v LGPL_EXCEPTION.TXT build
cp -r -v deploy/* build/
cp -r -v os_deploy/linux64/* build/

