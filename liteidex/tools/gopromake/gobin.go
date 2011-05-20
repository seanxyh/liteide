// Copyright 2011 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"runtime"
	"os"
	"exec"
	"path"
)

type GoBin struct {
	compiler string
	link     string
	pack     string
	cgo      string
	objext   string
	exeext   string
	pakext   string
	rm       string
}

func defGoroot() string {
	var curos = runtime.GOOS
	if curos == "windows" {
		return "c:/go" 
	}
	return os.Getenv("HOME")+"/go"
}

func NewGoBin(defgoroot string) (p *GoBin, err os.Error) {
	goroot := os.Getenv("GOROOT")
	if goroot == "" {
		goroot = defgoroot
		os.Setenv("GOROOT",goroot)
	}

	gobin := goroot+"/bin"

	goos := os.Getenv("GOOS")
	if goos == "" {
		goos = runtime.GOOS
		os.Setenv("GOOS", goos)
	}

	var exeext string
	var rm string = "rm"

	switch goos {
	case "windows":
		exeext = ".exe"
		rm = "del"
	}

	var goarch string
	goarch = os.Getenv("GOARCH")
	if goarch == "" {
		goarch = runtime.GOARCH
		os.Setenv("GOARCH", goarch)
	}
	var o string
	switch goarch {
	case "amd64":
		o = "6"
	case "386":
		o = "8"
	case "arm":
		o = "5"
	default:
		err = os.NewError("Unsupported arch: " + goarch)
		return
	}

	p = new(GoBin)
	p.compiler = o + "g"
	p.link = o + "l"
	p.pack = "gopack"
	p.cgo = "cgo"
	p.objext = "." + o
	p.exeext = exeext
	p.pakext = ".a"
	p.rm = rm

	p.compiler, err = exec.LookPath(path.Join(gobin,p.compiler))
	if err != nil {
		return
	}
	p.link, err = exec.LookPath(path.Join(gobin,p.link))
	if err != nil {
		return
	}
	p.pack, err = exec.LookPath(path.Join(gobin,p.pack))
	if err != nil {
		return
	}
	p.cgo, err = exec.LookPath(path.Join(gobin,p.cgo))
	if err != nil {
		return
	}

	return
}
