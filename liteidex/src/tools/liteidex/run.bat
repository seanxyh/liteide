@echo off
go build
copy liteidex.exe ..\..\..\liteide\bin
cd ..\..\..\liteide\bin
liteidex