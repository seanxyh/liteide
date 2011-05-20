// Copyright 2011 visualfc <visualfc@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"fmt"
	"os"
	"io/ioutil"
	"bytes"
	"regexp"
	"path"
	"exec"
	"syscall"
	"path/filepath"
)

type Gopro struct {
	Name   string
	Values map[string][]string
	array  *PackageArray
}

func NewGopro(name string) (pro *Gopro, err os.Error) {
	buf, e := ioutil.ReadFile(name)
	if e != nil {
		err = e
		return
	}
	pro = new(Gopro)
	pro.Name = name
	pro.Values = make(map[string][]string)

	pre, e := regexp.Compile("\\\\[\t| ]*[\r|\n]+[\t| ]*")//("\\\\[^a-z|A-Z|0-9|_|\r|\n]*[\r|\n]+[^a-z|A-Z|0-9|_|\r|\n]*")
	if e != nil {
		err = e
		return
	}
	all := pre.ReplaceAll(buf[:], []byte(" "))
	all = bytes.Replace(all, []byte("\r\n"), []byte("\n"), -1)
	lines := bytes.Split(all, []byte("\n"), -1)

	for _, line := range lines {
		offset := 2
		line = bytes.Replace(line, []byte("\t"), []byte(" "), -1)
		if len(line) >= 1 && line[0] == '#' {
			continue
		}
		find := bytes.Index(line, []byte("+="))
		if find == -1 {
			offset = 1
			find = bytes.Index(line, []byte("="))
		}
		if find != -1 {
			k := bytes.TrimSpace(line[0:find])
			v := bytes.SplitAfter(line[find+offset:], []byte(" "), -1)
			var vall []string
			if offset == 2 {
				vall = pro.Values[string(k)]
			}
			for _, nv := range v {
				nv2 := bytes.TrimSpace(nv)
				if len(nv2) != 0 && string(nv2) != "\\"{
					vall = append(vall, string(nv2))
				}
			}
			pro.Values[string(k)] = vall
		}
	}
	return
}

func (file *Gopro) Gofiles() []string {
	return file.Values["GOFILES"]
}

func (file *Gopro) AllPackage() (paks []string) {
	for i := 0; i < len(file.array.Data); i++ {
		paks = append(paks, file.array.Data[i].pakname)
	}
	return
}

func (file *Gopro) PackageFilesString(pakname string) []string {
	return file.Values[pakname]
}

func (file *Gopro) PackageFiles(pakname string) []string {
	return file.array.index(pakname).files
}

func (file *Gopro) TargetName() string {
	t := file.Target()
	_,name := path.Split(t)
	return name
}

func (file *Gopro) Target() string {
	v := file.Values["TARGET"]
	if len(v) >= 1 && len(v[0]) > 0 {
		return v[0]
	}
	if len(file.Name) == 0 {
		return "main"
	}
	return path.Base(file.Name)	
}

func (file *Gopro) DestDir() (dir string) {
	v := file.Values["DESTDIR"]
	if len(v) >= 1 {
		dir = string(v[0])
	}
	t := file.Target()
	p,_ := path.Split(t)	
	if len(p) > 0 {
		dir = path.Join(dir,p)
	}
	return
}

func (file *Gopro) ProjectDir() (dir string) {
	dir, _ = path.Split(file.Name)
	return
}

func build(gcfile string, opts []string, proFileName string, files []string, envv []string, dir string) (status syscall.WaitStatus, err os.Error) {
	arg := []string{gcfile, "-o", proFileName}
	for _,v := range opts {
		arg = append(arg,v)
	}
	for _, v := range files {
		arg = append(arg, string(v))
	}
	fmt.Println("\t", arg)
	var cmd *exec.Cmd
	cmd, err = exec.Run(gcfile, arg[:], envv[:], dir, 0, 1, 2)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	defer cmd.Close()
	var wait *os.Waitmsg
	wait, err = cmd.Wait(0)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	status = wait.WaitStatus
	return
}

func link(glfile string, opts []string, target string, ofile string, envv []string, dir string) (status syscall.WaitStatus, err os.Error) {
	arg := []string{glfile, "-o", target}
	for _,v := range opts {
		arg = append(arg,v)
	}	
	arg = append(arg,ofile)
	fmt.Println("\t", arg)
	var cmd *exec.Cmd
	cmd, err = exec.Run(glfile, arg[:], envv[:], dir, 0, 1, 2)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	defer cmd.Close()
	var wait *os.Waitmsg
	wait, err = cmd.Wait(0)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	status = wait.WaitStatus
	return
}

func pack(pkfile string, target string, ofile string, envv []string, dir string) (status syscall.WaitStatus, err os.Error) {
	arg := []string{pkfile, "grc", target, ofile}
	fmt.Println("\t", arg)
	var cmd *exec.Cmd
	cmd, err = exec.Run(pkfile, arg[:], envv[:], dir, 0, 1, 2)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	defer cmd.Close()
	var wait *os.Waitmsg
	wait, err = cmd.Wait(0)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	status = wait.WaitStatus
	return
}

func (file *Gopro) IsEmpty() bool {
	return len(file.Values) == 0
}

func (file *Gopro) Clean(clean string) (os.Error){
	dir := file.ProjectDir()
	if dir == "" {
		dir = "./"
	}	
	files,err := ioutil.ReadDir(dir)
	var rfiles []string
	if err != nil {
		return err
	}
	for _,file := range files {
		ext := filepath.Ext(file.Name)
		if ext == ".8" || ext == ".5" || ext == ".6" {
			err := os.Remove(file.Name)
			if err == nil {
				rfiles = append(rfiles,file.Name)
			}	
		}	
	}		
	if clean == "all" {
		target := file.Target()
	    dest := file.DestDir()		
		if len(dest) > 0 {
			target = path.Join(dest, target)
		}
		targets := []string{target,target+".a",target+".exe"}
		for _,v := range targets {
			err := os.Remove(v)
			if err == nil {
				rfiles = append(rfiles,v)
			} 
		}
	}
	if len(rfiles) == 0 {
		fmt.Println("clean nothing")
	} else {
		for _,v := range rfiles {
			fmt.Println("remove",v)
		}	
	}	
	return nil
}	

func (file *Gopro) MakeTarget(gobin *GoBin) (status syscall.WaitStatus, err os.Error) {
	all := file.AllPackage()
	for _, v := range all {
		if v == "documentation" {
			continue
		}			
		fmt.Printf("build package %s:\n", v)
		target := v
		ofile := target + gobin.objext
		if v == "main" {
			target = file.TargetName()
			ofile = target + "_go_" + gobin.objext
		} 
		status, err = build(gobin.compiler, file.Values["GCOPT"], ofile, file.PackageFiles(v), os.Environ(), file.ProjectDir())
		if err != nil || status.ExitStatus() != 0 {
			return
		}

		dest := file.DestDir()		
		if len(dest) > 0 {
			//dest = path.Join(file.ProjectDir(), dest)
			os.MkdirAll(dest, 0777)
			target = path.Join(dest, target)
		}
		if string(v) == "main" {
			target = target + gobin.exeext
			status, err = link(gobin.link, file.Values["GLOPT"],target, ofile, os.Environ(), file.ProjectDir())
			if err != nil || status.ExitStatus() != 0 {
				return
			}
			fmt.Printf("link target : %s\n", target)
		} else if *buildLib {
			target = target + gobin.pakext
			status, err = pack(gobin.pack, target, ofile, os.Environ(), file.ProjectDir())
			if err != nil || status.ExitStatus() != 0 {
				return
			}
			fmt.Printf("pack library : %s\n", target)
		}
	}
	return
}

