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
	Dirs *DirList
}

func (dir *Directory) lookupPkg(pkgname string) string {
	p := []string{pkgname}
	if strings.Contains(pkgname, `\`) {
		p = strings.Split(pkgname, `\`)
	} else if strings.Contains(pkgname, `/`) {
		p = strings.Split(pkgname, `/`)
	}
	fmt.Println(p)
	for i := 0; i < len(p); i++ {
		for j := 0; j < len(dir.Dirs); j++ {
			if dir.Dirs[j].Name == p[i] {
				dir = dir.Dirs[j]
				fmt.Println(dir.Path)
				break
			}
		}
	}
	return dir.Name
}

func FindPkgList(root string, pkgname string) (best string, matchs []string) {
	dir := newDirectory(root, nil, -1)
	if dir == nil {
		return
	}
	info := Info{Dirs: dir.listing(true)}
	max := len(info.Dirs.List)
	for i := 0; i < max; i++ {
		name := info.Dirs.List[i].Name
		path := filepath.ToSlash(info.Dirs.List[i].Path)

		if name == pkgname || path == pkgname {
			best = path
		} else if strings.HasSuffix(path, "/"+pkgname) {
			best = path
		} else if strings.Contains(path, pkgname) {
			matchs = append(matchs, path)
		}
	}
	return
}

func GetPkgList(root string, templateData string) []byte {
	data := readTemplateData(templateData)
	if data == nil {
		return nil
	}
	dir := newDirectory(root, nil, -1)
	if dir == nil {
		return nil
	}
	info := Info{Dirs: dir.listing(true)}
	return applyTemplate(data, "package", info)
}
