// gopkgview project pkglist.go
package main

import (
	"fmt"
	"bytes"
	"template"
	"log"
	"io"
	"time"
	"strconv"
	"strings"
	"path/filepath"
)

var (
	fs FileSystem = OS // the underlying file system
)

// Fake package file and name for commands. Contains the command documentation.
const fakePkgFile = "doc.go"
const fakePkgName = "documentation"

func textFmt(w io.Writer, format string, x ...interface{}) {
	var buf bytes.Buffer
	fmt.Fprint(&buf, x)
	template.HTMLEscape(w, buf.Bytes())
}

func pathEscFmt(w io.Writer, format string, x ...interface{}) {
	switch v := x[0].(type) {
	case []byte:
		template.HTMLEscape(w, v)
	case string:
		template.HTMLEscape(w, []byte(filepath.ToSlash(v)))
	default:
		var buf bytes.Buffer
		fmt.Fprint(&buf, x)
		template.HTMLEscape(w, buf.Bytes())
	}
}

func htmlEscFmt(w io.Writer, format string, x ...interface{}) {
	switch v := x[0].(type) {
	case int:
		template.HTMLEscape(w, []byte(strconv.Itoa(v)))
	case []byte:
		template.HTMLEscape(w, v)
	case string:
		template.HTMLEscape(w, []byte(v))
	default:
		var buf bytes.Buffer
		fmt.Fprint(&buf, x)
		template.HTMLEscape(w, buf.Bytes())
	}
}

// Template formatter for "padding" format.
func paddingFmt(w io.Writer, format string, x ...interface{}) {
	for i := x[0].(int); i > 0; i-- {
		fmt.Fprint(w, `<td width="25"></td>`)
	}
}

// Template formatter for "time" format.
func timeFmt(w io.Writer, format string, x ...interface{}) {
	template.HTMLEscape(w, []byte(time.SecondsToLocalTime(x[0].(int64)/1e9).String()))
}

var fmap = template.FormatterMap{
	"":         textFmt,
	"html-esc": htmlEscFmt,
	"path-esc": pathEscFmt,
	"padding":  paddingFmt,
	"time":     timeFmt,
}

func readTemplateData(data string) *template.Template {
	t, err := template.Parse(data, fmap)
	if err != nil {
		log.Fatalf("%s: %v", data, err)
	}
	return t
}

func readTemplateFile(path string) *template.Template {
	data, err := fs.ReadFile(path)
	if err != nil {
		log.Fatalf("ReadFile %s: %v", path, err)
	}
	t, err := template.Parse(string(data), fmap)
	if err != nil {
		log.Fatalf("%s: %v", path, err)
	}
	return t
}

func applyTemplate(t *template.Template, name string, data interface{}) []byte {
	var buf bytes.Buffer
	if err := t.Execute(&buf, data); err != nil {
		log.Printf("%s.Execute: %s", name, err)
	}
	return buf.Bytes()
}

type Info struct {
	Find string
	Best *DirEntry
	Dirs *DirList
}

func FindPkgInfo(root string, pkgname string) *Info {
	dir := newDirectory(root, nil, -1)
	if dir == nil {
		return nil
	}
	dirList := dir.listing(true)
	if pkgname == "*" {
		return &Info{pkgname, nil, dirList}
	}
	var best DirEntry
	var list []DirEntry
	max := len(dirList.List)
	for i := 0; i < max; i++ {
		name := dirList.List[i].Name
		path := filepath.ToSlash(dirList.List[i].Path)
		if name == pkgname || path == pkgname {
			best = dirList.List[i]
		} else if strings.Contains(path, pkgname) {
			list = append(list, dirList.List[i])
		}
	}
	return &Info{pkgname, &best, &DirList{dirList.MaxHeight, list}}
}

func (info *Info) GetPkgList(templateData string) []byte {
	data := readTemplateData(templateData)
	return applyTemplate(data, "pkglist", info)
}
