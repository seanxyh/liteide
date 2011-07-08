Welcome to LiteIDE X!

Introduction:
    LiteIDE is a simple, open source, cross-platform IDE. 

Project:	LiteIDE
Author:	visualfc (earthworm)
Version:	x5 (v0.78125)
Website:	http://code.google.com/liteide

Features:
    Plugins Extension System
    File Based Build System
    Multi Build Environment System
    Kate Syntax And Style Scheme
    WordApi Completer Helper
    Golang astview gocode and godoc. 

System:
    WIN32
    Linux
    MacOSX 


FAQ:

1. FileBrowser  and Build
   FileBrowser load file and auto use Makefile to Build.

2. Go Project (*.e64)
TARGET=hello
DESTDIR=./bin
GCOPT=
GLOPT=
GOFILES = \
	hello.go \
	user32.go

3. Build
Edit liteide/data/build/* config file and reload LiteIDE

4.Find/Replace Regex
Search and Replace: (Colo)(u)(r) -> \1\3