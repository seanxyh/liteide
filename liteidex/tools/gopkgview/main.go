// gopkgview project main.go
package main

import (
	"fmt"
	"path/filepath"
	"flag"
	"runtime"
	"bytes"
)

var (
	goroot = flag.String("goroot", runtime.GOROOT(), "Go root directory")
	fs     FileSystem // the underlying file system
)

// Fake package file and name for commands. Contains the command documentation.
const fakePkgFile = "doc.go"
const fakePkgName = "documentation"

func (dir *Directory) writeLeafsName1(path string, buf *bytes.Buffer) {
	if dir != nil {
		var npath string
		if len(path) == 0 {
			npath = dir.Name
		} else {
			npath = path + "/" + dir.Name
		}
		if len(dir.Text) != 0 {
			buf.WriteString(npath)
			buf.WriteByte('\n')
		}
		for _, d := range dir.Dirs {
			d.writeLeafsName1(npath, buf)
		}
	}
}

func (dir *Directory) writeLeafsName(buf *bytes.Buffer) {
	if dir != nil {
		for _, d := range dir.Dirs {
			d.writeLeafsName1("", buf)
		}
	}
}

func loadPkg() []byte{
	dir := newDirectory(filepath.Join(*goroot, "src", "pkg"), nil, -1)
	var b bytes.Buffer
	dir.writeLeafsName(&b)
	return b.Bytes()
}

func main() {
	flag.Parse()
	fs = OS
	buf := loadPkg()
	fmt.Println(string(buf))
}
