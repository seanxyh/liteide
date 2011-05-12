// Copyright 2011 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"fmt"
	"flag"
	"os"
	"path"
	"bytes"
)	

var (
	proFileName  *string = flag.String("gopro", "", "make go project")
	goFileName   *string = flag.String("gofiles", "", "make go sources")
	goTargetName *string = flag.String("o", "", "file specify output file")
	printDep     *bool   = flag.Bool("dep", false, "print packages depends ")
	showVer      *bool   = flag.Bool("ver", true, "print version ")
	buildLib     *bool   = flag.Bool("lib", false, "build packages as librarys outside main")
	goroot		 *string = flag.String("goroot",defGoroot(),"default go root")	
	clean		 *string = flag.String("clean","","clean project [obj|all]")
)

var Usage = func() {
	_, name := path.Split(os.Args[0])
	fmt.Fprintf(os.Stderr, "usage: %s -gopro   example.pro\n", name)
	fmt.Fprintf(os.Stderr, "       %s -gofiles \"go1.go go2.go\"\n", name)
	flag.PrintDefaults()
}

func exitln(err os.Error) {
	fmt.Println(err)
	os.Exit(1)
}

func main() {
	flag.Parse()
	if *showVer == true {
		fmt.Println("golang project make tools. v1.0 by visualfc.")
	}

	gobin, err := NewGoBin(*goroot)
	if err != nil {
		exitln(err)
	}

	var pro *Gopro

	if len(*proFileName) > 0 {
		pro, err = NewGopro(*proFileName)
		if err != nil {
			exitln(err)
		}
	} else if len(*goFileName) > 0 {
		var input []byte = []byte(*goFileName)
		all := bytes.SplitAfter(input, []byte(" "), -1)
		pro = new(Gopro)
		pro.Values = make(map[string][]string)

		for _, v := range all {
			pro.Values["GOFILES"] = append(pro.Values["GOFILES"], string(v))
		}
	}
	if pro == nil || err != nil {
		Usage()
		os.Exit(1)
	}

	if len(*goTargetName) > 0 {
		pro.Values["TARGET"] = []string{*goTargetName}
	}
	fmt.Println("gopromake parser files...")

	files := pro.Gofiles()
	pro.array = ParserFiles(files)

	if printDep != nil && *printDep == true {
		fmt.Printf("AllPackage:\n%s\n", pro.array)
	}

	if pro.array.HasMain == false {
		*buildLib = true
	}
	
	if len(*clean) > 0 {
		if *clean == "obj" || *clean == "all" {
			err := pro.Clean(*clean)
			if err != nil {
				exitln(err)
			}
		} else {
			Usage()
			os.Exit(1)
		}
		os.Exit(0)
	}		

	status, err := pro.MakeTarget(gobin)
	if err != nil {
		exitln(err)
	} else if status.ExitStatus() != 0 {
		exitln(os.NewError("Error"))
	}
	os.Exit(0)
}
