#!/bin/sh

export GOPATH=$PWD
go install -v tools/goastview
go install -v tools/godocview
go install -v tools/goexec
go install -v tools/gopromake
