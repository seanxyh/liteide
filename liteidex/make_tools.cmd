@echo off

set GOPATH=%CD%

go install -v -ldflags "-s -H windowsgui" liteidex
go install -v -ldflags -s tools/goastview
go install -v -ldflags -s tools/godocview
go install -v -ldflags -s tools/goexec
go install -v -ldflags -s tools/goapi
