// gopkgview project main.go
package main

import (
	"fmt"
	"path/filepath"
	"flag"
	"runtime"
)

var (
	goroot = flag.String("goroot", runtime.GOROOT(), "Go root directory")
	html    = flag.Bool("html", false, "print HTML in command-line mode")
)

func main() {
	flag.Parse()	
	var template string
	if *html {
		template = packageHTML
	} else {
		template = packageText
	}
	contents := GetPkgList(filepath.Join(*goroot,"src","pkg"),template)
	fmt.Println(string(contents))
}
