// godocview project main.go
package main

import (
	"fmt"
	"path/filepath"
	"flag"
	"runtime"
	"strings"
)

var (
	goroot = flag.String("goroot", runtime.GOROOT(), "Go root directory")
	subdir = flag.String("subdir", "src/pkg", "Go package subdir")
	find   = flag.String("find", "", "find package name")
	html   = flag.Bool("html", false, "print HTML in command-line mode")
)

func main() {
	flag.Parse()
	var template string
	if *html {
		template = packageHTML
	} else {
		template = packageText
	}
	if len(*find) > 0 {
		best, matchs := FindPkgList(filepath.Join(*goroot, *subdir), *find)
		fmt.Printf("%s:%s",best,strings.Join(matchs,","))
	} else {
		contents := GetPkgList(filepath.Join(*goroot, *subdir), template)
		fmt.Println(string(contents))
	}
}
