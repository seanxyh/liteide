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
)

var (
	fs     FileSystem = OS// the underlying file system
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
	"":             textFmt,
	"html-esc":     htmlEscFmt,
	"padding":      paddingFmt,	
	"time":         timeFmt,
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

type Info struct{
	Dirs *DirList
}	

func GetPkgList(root string,templateData string) []byte {	
	packageHTML := readTemplateData(templateData)
	if packageHTML == nil {
		return nil
	}		
	dir := newDirectory(root,nil,-1)
	if dir == nil {
		return nil
	}
	var info Info
	info.Dirs = dir.listing(true)
	return applyTemplate(packageHTML,"package",info)
}	