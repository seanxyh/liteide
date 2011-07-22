// godocview project main.go
package main

import (
	"fmt"
	"path/filepath"
	"flag"
	"runtime"
	"os"
)

var (
	goroot = flag.String("goroot", runtime.GOROOT(), "Go root directory")
	subdir = flag.String("subdir", "src/pkg", "Go package sub directory")
	find   = flag.String("find", "", "find package list, :pkg flag is best match")
	mode   = flag.String("mode", "text", "Print mode [text|html|lite]")
)

func usage() {
	fmt.Fprintf(os.Stderr,
		"usage: godocview -find pkgname\n"+
			"       godocview -find *\t:list all package\n"+
			"       godocview -find build\t:find go/build package\n")
	flag.PrintDefaults()
	os.Exit(2)
}

func main() {
	flag.Usage = usage
	flag.Parse()

	if len(*find) == 0 {
		flag.Usage()
	}

	info := FindPkgInfo(filepath.Join(*goroot, *subdir), *find)
	if info == nil {
		os.Exit(2)
	}

	var template string

	if *find == "*" {
		switch *mode {
		case "html":
			template = listHTML
		case "lite":
			template = findLite
		case "text":
			template = listText
		default:
			template = listText
		}
	} else {
		switch *mode {
		case "html":
			template = findHTML
		case "lite":
			template = findLite
		case "text":
			template = findText
		default:
			template = findText
		}
	}

	contents := info.GetPkgList(template)
	fmt.Println(string(contents))
	
	os.Exit(0)
}
