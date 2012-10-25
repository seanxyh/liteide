// Copyright 2011 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Api computes the exported API of a set of Go packages.
//
// BUG(bradfitz): Note that this tool is only currently suitable
// for use on the Go standard library, not arbitrary packages.
// Once the Go AST has type information, this tool will be more
// reliable without hard-coded hacks throughout.

// 2012.10.17 fixed for any package
// visualfc

package main

import (
	//	"bufio"
	"bytes"
	"errors"
	"flag"
	"fmt"
	"go/ast"
	"go/build"
	"go/doc"
	"go/parser"
	"go/printer"
	"go/token"
	"log"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	//	"runtime"
	"sort"
	"strconv"
	"strings"
)

var TT = make([]uint, 10)
var TT2 = string("hello")

type T []string

var TT3 = T([]string{})

var out = os.Stderr
var out2 = os.ModeAppend
var out3 = os.Args

var MyArgs = flag.Args
var MyArgs2 = flag.Args()

type MyType build.Context

var MyArgs4 = func() {}
var MyArgs5 MyType
var MyArgs6 = ast.NewIdent("ok")
var MyArgs7 = MyArgs5.CgoEnabled
var MyArgs8 = s0.test1()
var MyArgs9 = s0.tag
var MyArgs10 = build.Default.SrcDirs()[0]

var MyArray = []build.Context{build.Default}
var MyArgs11 = MyArray[0].SrcDirs()[0]
var MyArgs12 = s0.con.Compiler

var MyCh1 = make(chan int)
var MyCh2 = make(chan<- int)
var MyCh3 = make(<-chan int)

var (
	MyCh6        = "hello"
	MyCh4, MyCh5 = func(int) (<-chan int, error) {
		return make(chan int), errors.New("error")
	}(10)
	MyCh7 = MyCh5.Error()
)

//var MyArgs9 = build.Default.CgoEnabled

const intSize = 32 << uint(^uint(0)>>63)
const IntSize = intSize // number of bits in int, uint (32 or 64)
type ss struct {
	tag string
	con build.Context
}

func (p *ss) test1() string {
	return "hello"
}

func (p *ss) test2() (b []byte) {
	return
}

func (p *ss) test3() (map[int]string, *ss) {
	return nil, newss()
}

func (p *ss) test4() func(x int) int {
	return func(x int) int {
		return x
	}
}

func (p *ss) test5() func(x int) *ss {
	return func(x int) *ss {
		return p
	}
}

func (p *ss) test6() string {
	return "hello"
}

func newss() *ss {
	return &ss{}
}

var (
	s0     ss
	s1     = &ss{}
	s2     = newss()
	V1     = s0.test1()
	V2     = s1.test1()
	V3     = s2.test2()
	V4, V5 = s2.test3()
	V6     = V5.test1()
	//V7     = s2.test4()(100)
	//V7 = s2.test5()(100).con.BuildTags
	V8  = s2.test5()(100).test5()(100)
	V9  = append([]int{}, 10)
	V10 = append(V9, 10)
	V11 = cap(V10)
	V12 = new(ss).con.BuildTags
	V13 = errors.New("hello").Error()
)

func init() {
	fmt.Println(V8)
}

// Flags
var (
	// TODO(bradfitz): once Go 1.1 comes out, allow the -c flag to take a comma-separated
	// list of files, rather than just one.
	verbose    = flag.Bool("v", false, "verbose debugging")
	allmethods = flag.Bool("e", false, "show all embedded methods")
	alldecls   = flag.Bool("a", false, "extract documentation for all package-level declarations")
	showpos    = flag.Bool("p", false, "show token pos tag")
	separate   = flag.String("sep", ",", "set token separate string")
	dep_parser = flag.Bool("dep", true, "check package import")
	nooutput   = flag.Bool("n", false, "no output, check only")
)

func usage() {

	fmt.Fprintf(os.Stderr, `usage: api [std|all|package...|local-dir]
    local-dir : . or ./goapi`)
	flag.PrintDefaults()
}

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() == 0 {
		flag.Usage()
		return
	}

	var pkgs []string

	if flag.Arg(0) == "std" || flag.Arg(0) == "all" {
		out, err := exec.Command("go", "list", flag.Arg(0)).Output()
		if err != nil {
			log.Fatal(err)
		}
		pkgs = strings.Fields(string(out))
	} else {
		pkgs = flag.Args()
	}

	w := NewWalker()
	w.sep = *separate

	for _, pkg := range pkgs {
		w.wantedPkg[pkg] = true
	}

	for _, pkg := range pkgs {
		if build.IsLocalImport(pkg) {
			wd, err := os.Getwd()
			if err != nil {
				log.Fatalln(err)
				continue
			}
			dir := path.Clean(path.Join(wd, pkg))
			bp, err := build.ImportDir(dir, 0)
			if err != nil {
				if *verbose {
					log.Fatalln(err)
				}
				continue
			}
			w.wantedPkg[bp.Name] = true
			w.WalkPackage(bp.Name, bp.Dir)
		} else {
			bp, err := build.Import(pkg, "", build.FindOnly)
			if err != nil {
				if *verbose {
					log.Println(err)
				}
				continue
			}
			w.WalkPackage(pkg, bp.Dir)
		}
	}

	if *nooutput {
		return
	}

	for _, feature := range w.Features() {
		fmt.Println(feature)
	}
}

func IsExported(name string) bool {
	if *alldecls {
		return true
	}
	return ast.IsExported(name)
}

// pkgSymbol represents a symbol in a package
type pkgSymbol struct {
	pkg    string // "net/http"
	symbol string // "RoundTripper"
}

type constInfo struct {
	typ string
	pos token.Pos
}

type Package struct {
	dpkg             *doc.Package
	apkg             *ast.Package
	interfaceMethods map[string]([]method)
	interfaces       map[string]*ast.InterfaceType //interface
	structs          map[string]*ast.StructType    //struct
	types            map[string]ast.Expr           //type
	functions        map[string]method             //function 
	consts           map[string]string             //const => type
	vars             map[string]string             //var => type
}

func NewPackage() *Package {
	return &Package{
		interfaceMethods: make(map[string]([]method)),
		interfaces:       make(map[string]*ast.InterfaceType),
		structs:          make(map[string]*ast.StructType),
		types:            make(map[string]ast.Expr),
		functions:        make(map[string]method),
		consts:           make(map[string]string),
		vars:             make(map[string]string),
	}
}

func (p *Package) findType(name string) ast.Expr {
	for k, v := range p.interfaces {
		if k == name {
			return v
		}
	}
	for k, v := range p.structs {
		if k == name {
			return v
		}
	}
	for k, v := range p.types {
		if k == name {
			return v
		}
	}
	return nil
}

func funcRetType(ft *ast.FuncType, index int) ast.Expr {
	if ft.Results != nil && len(ft.Results.List) >= index+1 {
		return ft.Results.List[index].Type
	}
	return nil
}

func findFunction(funcs []*doc.Func, name string) *ast.FuncType {
	for _, f := range funcs {
		if f.Name == name {
			return f.Decl.Type
		}
	}
	return nil
}

func (p *Package) findSelectorType(name string) ast.Expr {
	if t, ok := p.vars[name]; ok {
		return &ast.Ident{
			Name: t,
		}
	}
	if t, ok := p.consts[name]; ok {
		return &ast.Ident{
			Name: t,
		}
	}
	if t, ok := p.functions[name]; ok {
		return t.ft
	}
	for k, v := range p.structs {
		if k == name {
			return &ast.Ident{
				NamePos: v.Pos(),
				Name:    name,
			}
		}
	}
	for k, v := range p.types {
		if k == name {
			return v
		}
	}
	return nil
}

func (p *Package) findCallType(name string, index int) ast.Expr {
	if fn, ok := p.functions[name]; ok {
		return funcRetType(fn.ft, index)
	}
	for k, v := range p.structs {
		if k == name {
			return &ast.Ident{
				NamePos: v.Pos(),
				Name:    name,
			}
		}
	}
	for k, v := range p.types {
		if k == name {
			return v
			return &ast.Ident{
				NamePos: v.Pos(),
				Name:    name,
			}
		}
	}
	return nil
}

func (p *Package) findMethod(typ, name string) *ast.FuncType {
	for k, v := range p.interfaceMethods {
		if k == typ {
			for _, m := range v {
				if m.name == name {
					return m.ft
				}
			}
		}
	}
	for _, t := range p.dpkg.Types {
		if t.Name == typ {
			return findFunction(t.Methods, name)
		}
	}
	return nil
}

func (p *Package) resolveName(name string) (ok bool) {
	for _, v := range p.dpkg.Vars {
		for _, vname := range v.Names {
			if vname == name {
				return true
			}
		}
	}
	return
}

type Walker struct {
	context *build.Context
	//	root            string
	fset            *token.FileSet
	scope           []string
	features        map[string]([]token.Pos) // set
	lastConstType   string
	curPackageName  string
	sep             string
	curPackage      *Package
	constDep        map[string]*constInfo // key's const identifier has type of future value const identifier
	packageState    map[string]loadState
	packageMap      map[string]*Package
	interfaces      map[pkgSymbol]*ast.InterfaceType
	selectorFullPkg map[string]string // "http" => "net/http", updated by imports
	wantedPkg       map[string]bool   // packages requested on the command line
}

func NewWalker() *Walker {
	return &Walker{
		fset:            token.NewFileSet(),
		features:        make(map[string]([]token.Pos)),
		packageState:    make(map[string]loadState),
		interfaces:      make(map[pkgSymbol]*ast.InterfaceType),
		packageMap:      make(map[string]*Package),
		selectorFullPkg: make(map[string]string),
		wantedPkg:       make(map[string]bool),
		//		root:            filepath.Join(build.Default.GOROOT, "src/pkg"),
	}
}

// loadState is the state of a package's parsing.
type loadState int

const (
	notLoaded loadState = iota
	loading
	loaded
)

func postos(ps []token.Pos) (s []string) {
	for _, p := range ps {
		s = append(s, strconv.Itoa(int(p)))
	}
	return
}

func (w *Walker) Features() (fs []string) {
	for f, ps := range w.features {
		if *showpos {
			fs = append(fs, f+w.sep+strings.Join(postos(ps), ","))
		} else {
			fs = append(fs, f)
		}
	}
	sort.Strings(fs)
	return
}

// fileDeps returns the imports in a file.
func fileDeps(f *ast.File) (pkgs []string) {
	for _, is := range f.Imports {
		fpkg, err := strconv.Unquote(is.Path.Value)
		if err != nil {
			log.Fatalf("error unquoting import string %q: %v", is.Path.Value, err)
		}
		if fpkg != "C" {
			pkgs = append(pkgs, fpkg)
		}
	}
	return
}

// WalkPackage walks all files in package `name'.
// WalkPackage does nothing if the package has already been loaded.
func (w *Walker) WalkPackage(name string, dir string) {
	//log.Println("walk", name, dir)
	switch w.packageState[name] {
	case loading:
		log.Fatalf("import cycle loading package %q?", name)
		return
	case loaded:
		return
	}
	w.packageState[name] = loading
	defer func() {
		w.packageState[name] = loaded
	}()

	sname := name[strings.LastIndex(name, "/")+1:]

	apkg := &ast.Package{
		Files: make(map[string]*ast.File),
	}

	//	bp, err := build.Default.ImportDir(dir, 0)
	//	if err != nil {
	//		log.Fatalln(err)
	//		//fmt.Println(err)
	//	}

	//	files := append(append([]string{}, bp.GoFiles...), bp.CgoFiles...)
	//	for _, file := range files {
	//		f, err := parser.ParseFile(w.fset, filepath.Join(dir, file), nil, 0)
	//		if err != nil {
	//			log.Fatalf("error parsing package %s, file %s: %v", name, file, err)
	//		}
	//		if sname != f.Name.Name {
	//			continue
	//		}
	//		apkg.Files[file] = f
	//		for _, dep := range fileDeps(f) {
	//			bp, err := build.Import(dep, "", build.FindOnly)
	//			if err == nil {
	//				if dep != name {
	//					w.WalkPackage(dep, bp.Dir)
	//				}
	//			}
	//		}
	//		if *showpos && w.wantedPkg[name] {
	//			tf := w.fset.File(f.Pos())
	//			if tf != nil {
	//				fmt.Printf("pos %s,%s,%d,%d\n", name, filepath.Join(dir, file), tf.Base(), tf.Size())
	//			}
	//		}
	//	}
	fdir, err := os.Open(dir)
	if err != nil {
		log.Fatalln(err)
	}
	infos, err := fdir.Readdir(-1)
	fdir.Close()
	if err != nil {
		log.Fatalln(err)
	}

	for _, info := range infos {
		if info.IsDir() {
			continue
		}
		file := info.Name()
		if strings.HasPrefix(file, "_") || strings.HasSuffix(file, "_test.go") {
			continue
		}
		if strings.HasSuffix(file, ".go") {
			f, err := parser.ParseFile(w.fset, filepath.Join(dir, file), nil, 0)
			if err != nil {
				//fix_code
				if *verbose {
					log.Printf("error parsing package %s, file %s: %v", name, file, err)
				}
				continue
			}
			if f.Name.Name != sname {
				continue
			}

			apkg.Files[file] = f
			if *dep_parser {
				for _, dep := range fileDeps(f) {
					bp, err := build.Import(dep, "", build.FindOnly)
					if err == nil {
						if dep != name {
							w.WalkPackage(dep, bp.Dir)
						}
					}
				}
			}
			if *showpos && w.wantedPkg[name] {
				tf := w.fset.File(f.Pos())
				if tf != nil {
					fmt.Printf("pos %s%s%s%s%d:%d\n", name, w.sep, filepath.Join(dir, file), w.sep, tf.Base(), tf.Base()+tf.Size())
				}
			}
		}
	}

	if *verbose {
		log.Printf("package %s", name)
	}
	pop := w.pushScope("pkg " + name)
	defer pop()

	w.curPackageName = name
	w.constDep = map[string]*constInfo{}
	w.curPackage = NewPackage()
	w.curPackage.apkg = apkg
	w.packageMap[name] = w.curPackage

	for _, afile := range apkg.Files {
		w.recordTypes(afile)
	}

	// Register all function declarations first.
	for _, afile := range apkg.Files {
		for _, di := range afile.Decls {
			if d, ok := di.(*ast.FuncDecl); ok {
				w.peekFuncDecl(d)
			}
		}
	}

	for _, afile := range apkg.Files {
		w.walkFile(afile)
	}

	w.resolveConstantDeps()

	// Now that we're done walking types, vars and consts
	// in the *ast.Package, use go/doc to do the rest
	// (functions and methods). This is done here because
	// go/doc is destructive.  We can't use the
	// *ast.Package after this.
	var mode doc.Mode
	if *allmethods {
		mode |= doc.AllMethods
	}
	if *alldecls {
		mode |= doc.AllDecls
	}

	dpkg := doc.New(apkg, name, mode)
	w.curPackage.dpkg = dpkg

	for _, t := range dpkg.Types {
		// Move funcs up to the top-level, not hiding in the Types.
		dpkg.Funcs = append(dpkg.Funcs, t.Funcs...)

		for _, m := range t.Methods {
			w.walkFuncDecl(m.Decl)
		}
	}

	for _, f := range dpkg.Funcs {
		w.walkFuncDecl(f.Decl)
	}
}

// pushScope enters a new scope (walking a package, type, node, etc)
// and returns a function that will leave the scope (with sanity checking
// for mismatched pushes & pops)
func (w *Walker) pushScope(name string) (popFunc func()) {
	w.scope = append(w.scope, name)
	return func() {
		if len(w.scope) == 0 {
			log.Fatalf("attempt to leave scope %q with empty scope list", name)
		}
		if w.scope[len(w.scope)-1] != name {
			log.Fatalf("attempt to leave scope %q, but scope is currently %#v", name, w.scope)
		}
		w.scope = w.scope[:len(w.scope)-1]
	}
}

func (w *Walker) recordTypes(file *ast.File) {
	for _, di := range file.Decls {
		switch d := di.(type) {
		case *ast.GenDecl:
			switch d.Tok {
			case token.TYPE:
				for _, sp := range d.Specs {
					ts := sp.(*ast.TypeSpec)
					name := ts.Name.Name
					switch t := ts.Type.(type) {
					case *ast.InterfaceType:
						if IsExported(name) {
							w.noteInterface(name, t)
						}
						w.curPackage.interfaces[name] = t
					case *ast.StructType:
						w.curPackage.structs[name] = t
					default:
						w.curPackage.types[name] = ts.Type
					}
				}
			}
		}
	}
}

func (w *Walker) walkFile(file *ast.File) {
	// Not entering a scope here; file boundaries aren't interesting.
	for _, di := range file.Decls {
		switch d := di.(type) {
		case *ast.GenDecl:
			switch d.Tok {
			case token.IMPORT:
				for _, sp := range d.Specs {
					is := sp.(*ast.ImportSpec)
					fpath, err := strconv.Unquote(is.Path.Value)
					if err != nil {
						log.Fatal(err)
					}
					name := path.Base(fpath)
					if is.Name != nil {
						name = is.Name.Name
					}
					w.selectorFullPkg[name] = fpath
				}
			case token.CONST:
				for _, sp := range d.Specs {
					w.walkConst(sp.(*ast.ValueSpec))
				}
			case token.TYPE:
				for _, sp := range d.Specs {
					w.walkTypeSpec(sp.(*ast.TypeSpec))
				}
			case token.VAR:
				for _, sp := range d.Specs {
					w.walkVar(sp.(*ast.ValueSpec))
				}
			default:
				log.Fatalf("unknown token type %d in GenDecl", d.Tok)
			}
		case *ast.FuncDecl:
			// Ignore. Handled in subsequent pass, by go/doc.
		default:
			log.Printf("unhandled %T, %#v\n", di, di)
			printer.Fprint(os.Stderr, w.fset, di)
			os.Stderr.Write([]byte("\n"))
		}
	}
}

var constType = map[token.Token]string{
	token.INT:    "ideal-int",
	token.FLOAT:  "ideal-float",
	token.STRING: "ideal-string",
	token.CHAR:   "ideal-char",
	token.IMAG:   "ideal-imag",
}

var varType = map[token.Token]string{
	token.INT:    "int",
	token.FLOAT:  "float64",
	token.STRING: "string",
	token.CHAR:   "rune",
	token.IMAG:   "complex128",
}

var builtinTypes = []string{
	"bool", "byte", "complex64", "complex128", "error", "float32", "float64",
	"int", "int8", "int16", "int32", "int64", "rune", "string",
	"uint", "uint8", "uint16", "uint32", "uint64", "uintptr",
}

func isBuiltinType(typ string) bool {
	for _, v := range builtinTypes {
		if v == typ {
			return true
		}
	}
	return false
}

func constTypePriority(typ string) int {
	switch typ {
	case "complex128":
		return 100
	case "ideal-imag":
		return 99
	case "complex64":
		return 98
	case "float64":
		return 97
	case "ideal-float":
		return 96
	case "float32":
		return 95
	case "int64":
		return 94
	case "int", "uint", "uintptr":
		return 93
	case "ideal-int":
		return 92
	case "int16", "uint16", "int8", "uint8", "byte":
		return 91
	case "ideal-char":
		return 90
	}
	return 101
}

var errTODO = errors.New("TODO")

func (w *Walker) constRealType(typ string) string {
	pos := strings.Index(typ, ".")
	if pos >= 0 {
		pkg := typ[:pos]
		if pkg == "C" {
			return "int"
		}
		typ = typ[pos+1:]
		if name, ok := w.selectorFullPkg[pkg]; ok {
			if p, ok := w.packageMap[name]; ok {
				ret := p.findType(typ)
				if ret != nil {
					return w.nodeString(w.namelessType(ret))
				}
			}
		}
	} else {
		ret := w.curPackage.findType(typ)
		if ret != nil {
			return w.nodeString(w.namelessType(ret))
		}
	}
	return typ
}

func (w *Walker) constValueType(vi interface{}) (string, error) {
	switch v := vi.(type) {
	case *ast.BasicLit:
		litType, ok := constType[v.Kind]
		if !ok {
			return "", fmt.Errorf("unknown basic literal kind %#v", v)
		}
		return litType, nil
	case *ast.UnaryExpr:
		return w.constValueType(v.X)
	case *ast.SelectorExpr:
		lhs := w.nodeString(v.X)
		rhs := w.nodeString(v.Sel)
		//fix_code example C.PROT_NONE
		if lhs == "C" {
			return lhs + "." + rhs, nil
		}
		pkg, ok := w.selectorFullPkg[lhs]
		if !ok {
			return "", fmt.Errorf("unknown constant reference; unknown package in expression %s.%s", lhs, rhs)
		}
		if p, ok := w.packageMap[pkg]; ok {
			if ret, ok := p.consts[rhs]; ok {
				return pkgRetType(lhs, ret), nil
			}
		}
		return "", fmt.Errorf("unknown constant reference to %s.%s", lhs, rhs)
	case *ast.Ident:
		if v.Name == "iota" {
			return "ideal-int", nil // hack.
		}
		if v.Name == "false" || v.Name == "true" {
			return "bool", nil
		}
		if t, ok := w.curPackage.consts[v.Name]; ok {
			return t, nil
		}
		return constDepPrefix + v.Name, nil
	case *ast.BinaryExpr:
		//fix_code
		//== > < ! != >= <=
		if v.Op == token.EQL || v.Op == token.LSS || v.Op == token.GTR || v.Op == token.NOT ||
			v.Op == token.NEQ || v.Op == token.LEQ || v.Op == token.GEQ {
			return "bool", nil
		}
		left, err := w.constValueType(v.X)
		if err != nil {
			return "", err
		}
		if v.Op == token.SHL || v.Op == token.SHR {
			return left, err
		}
		right, err := w.constValueType(v.Y)
		if err != nil {
			return "", err
		}
		//const left != right , one or two is ideal-
		if left != right {
			if strings.HasPrefix(left, constDepPrefix) && strings.HasPrefix(right, constDepPrefix) {
				// Just pick one.
				// e.g. text/scanner GoTokens const-dependency:ScanIdents, const-dependency:ScanFloats
				return left, nil
			}
			lp := constTypePriority(w.constRealType(left))
			rp := constTypePriority(w.constRealType(right))
			if lp >= rp {
				return left, nil
			} else {
				return right, nil
			}
			return "", fmt.Errorf("in BinaryExpr, unhandled type mismatch; left=%q, right=%q", left, right)
		}
		return left, nil
	case *ast.CallExpr:
		// Not a call, but a type conversion.
		return w.nodeString(v.Fun), nil
	case *ast.ParenExpr:
		return w.constValueType(v.X)
	}
	return "", fmt.Errorf("unknown const value type %T", vi)
}

func pkgRetType(pkg, ret string) string {
	start := strings.HasPrefix(ret, "*")
	var name = ret
	if start {
		name = ret[1:]
	}
	if ast.IsExported(name) {
		if start {
			return "*" + pkg + "." + name
		}
		return pkg + "." + name
	}
	return ret
}

func (w *Walker) findStructFieldType(st ast.Expr, name string) ast.Expr {
	if s, ok := st.(*ast.StructType); ok {
		for _, fi := range s.Fields.List {
			for _, n := range fi.Names {
				if n.Name == name {
					return fi.Type
				}
			}
		}
	}
	return nil
}

func (w *Walker) varFunctionType(name, sel string, index int) (string, error) {
	pos := strings.Index(name, ".")
	if pos != -1 {
		pkg := name[:pos]
		typ := name[pos+1:]
		if full, ok := w.selectorFullPkg[pkg]; ok {
			if p, ok := w.packageMap[full]; ok {
				fn := p.findMethod(typ, sel)
				if fn != nil {
					ret := funcRetType(fn, index)
					if ret != nil {
						return pkgRetType(pkg, w.nodeString(w.namelessType(ret))), nil
					}
				}
			}
		}
		return "", fmt.Errorf("unknown pkg type function pkg: %s.%s.%s", pkg, typ, sel)
	}
	//find local var.func()
	if ns, nt, n := w.resolveName(name); n >= 0 {
		var vt string
		if nt != nil {
			vt = w.nodeString(w.namelessType(nt))
		} else if ns != nil {
			typ, err := w.varValueType(ns, n)
			if err == nil {
				vt = typ
			}
		} else {
			typ := w.curPackage.findSelectorType(name)
			if typ != nil {
				vt = w.nodeString(w.namelessType(typ))
			}
		}
		if strings.HasPrefix(vt, "*") {
			vt = vt[1:]
		}
		if vt == "error" && sel == "Error" {
			return "string", nil
		}
		if fn, ok := w.curPackage.functions[vt+"."+sel]; ok {
			return w.nodeString(w.namelessType(funcRetType(fn.ft, index))), nil
		}
	}
	//find pkg.func()
	if pkg, ok := w.selectorFullPkg[name]; ok {
		if p, ok := w.packageMap[pkg]; ok {
			typ := p.findCallType(sel, index)
			if typ != nil {
				//log.Println(st.Name, typ)
				return pkgRetType(name, w.nodeString(w.namelessType(typ))), nil
			}
			return "", fmt.Errorf("not find pkg func %v.%v", pkg, sel)
		}
	}
	return "", fmt.Errorf("not find func %v.%v", name, sel)

}

func (w *Walker) varSelectorType(name string, sel string) (string, error) {
	pos := strings.Index(name, ".")
	if pos != -1 {
		pkg := name[:pos]
		typ := name[pos+1:]
		if full, ok := w.selectorFullPkg[pkg]; ok {
			if p, ok := w.packageMap[full]; ok {
				t := p.findType(typ)
				if t != nil {
					typ := w.findStructFieldType(t, sel)
					if typ != nil {
						return w.nodeString(w.namelessType(typ)), nil
					}
				}
			}
		}
		return "", fmt.Errorf("unknown pkg type selector pkg: %s.%s.%s", pkg, typ, sel)
	}
	vs, vt, n := w.resolveName(name)
	if n >= 0 {
		var typ string
		if vt != nil {
			typ = w.nodeString(w.namelessType(vt))
		} else {
			typ, _ = w.varValueType(vs, n)
		}
		if strings.HasPrefix(typ, "*") {
			typ = typ[1:]
		}
		//typ is type, find real type
		for k, v := range w.curPackage.types {
			if k == typ {
				typ = w.nodeString(w.namelessType(v))
			}
		}
		pos := strings.Index(typ, ".")
		if pos == -1 {
			t := w.curPackage.findType(typ)
			if t != nil {
				typ := w.findStructFieldType(t, sel)
				if typ != nil {
					return w.nodeString(w.namelessType(typ)), nil
				}
			}
		} else {
			name := typ[:pos]
			typ = typ[pos+1:]
			if pkg, ok := w.selectorFullPkg[name]; ok {
				if p, ok := w.packageMap[pkg]; ok {
					t := p.findType(typ)
					if t != nil {
						typ := w.findStructFieldType(t, sel)
						if typ != nil {
							return w.nodeString(w.namelessType(typ)), nil
						}
					}
				}
			}
		}
	}
	if pkg, ok := w.selectorFullPkg[name]; ok {
		if p, ok := w.packageMap[pkg]; ok {
			typ := p.findSelectorType(sel)
			if typ != nil {
				return pkgRetType(name, w.nodeString(w.namelessType(typ))), nil
			}
		}
	}
	return "", fmt.Errorf("unknown selector expr ident: %s.%s", name, sel)
}

func (w *Walker) varValueType(vi interface{}, index int) (string, error) {
	switch v := vi.(type) {
	case *ast.BasicLit:
		litType, ok := varType[v.Kind]
		if !ok {
			return "", fmt.Errorf("unknown basic literal kind %#v", v)
		}
		return litType, nil
	case *ast.CompositeLit:
		return w.nodeString(v.Type), nil
	case *ast.FuncLit:
		return w.nodeString(w.namelessType(v.Type)), nil
	case *ast.UnaryExpr:
		if v.Op == token.AND {
			typ, err := w.varValueType(v.X, index)
			return "*" + typ, err
		}
		return "", fmt.Errorf("unknown unary expr: %#v", v)
	case *ast.SelectorExpr:
		switch st := v.X.(type) {
		case *ast.Ident:
			return w.varSelectorType(st.Name, v.Sel.Name)
		case *ast.CallExpr:
			typ, err := w.varValueType(v.X, index)
			if err == nil {
				if strings.HasPrefix(typ, "*") {
					typ = typ[1:]
				}
				t := w.curPackage.findType(typ)
				if st, ok := t.(*ast.StructType); ok {
					for _, fi := range st.Fields.List {
						for _, n := range fi.Names {
							if n.Name == v.Sel.Name {
								return w.varValueType(fi.Type, index)
							}
						}
					}
				}
			}
		case *ast.SelectorExpr:
			typ, err := w.varValueType(v.X, index)
			if err == nil {
				return w.varSelectorType(typ, v.Sel.Name)
			}
		case *ast.IndexExpr:
			//log.Fatalln(st.X)
			typ, err := w.varValueType(st.X, index)
			//log.Fatalln(typ, err)
			if err == nil {
				if strings.HasPrefix(typ, "[]") {
					return w.varSelectorType(typ[2:], v.Sel.Name)
				}
			}
		}
		return "", fmt.Errorf("unknown selector expr: %T %s.%s", v.X, w.nodeString(v.X), v.Sel)
	case *ast.Ident:
		if v.Name == "true" || v.Name == "false" {
			return "bool", nil
		}
		if isBuiltinType(v.Name) {
			return v.Name, nil
		}
		vt := w.curPackage.findType(v.Name)
		if vt != nil {
			return w.nodeString(vt), nil
		}
		vs, _, n := w.resolveName(v.Name)
		if n >= 0 {
			return w.varValueType(vs, n)
		}
		return "", fmt.Errorf("unresolved identifier: %q", v.Name)
	case *ast.BinaryExpr:
		//fix_code
		if v.Op == token.EQL {
			return "bool", nil
		}
		left, err := w.varValueType(v.X, index)
		if err != nil {
			return "", err
		}
		right, err := w.varValueType(v.Y, index)
		if err != nil {
			return "", err
		}
		if left != right {
			return "", fmt.Errorf("in BinaryExpr, unhandled type mismatch; left=%q, right=%q", left, right)
		}
		return left, nil
	case *ast.ParenExpr:
		return w.varValueType(v.X, index)
	case *ast.CallExpr:
		switch ft := v.Fun.(type) {
		case *ast.ArrayType:
			return w.nodeString(v.Fun), nil
		case *ast.Ident:
			switch ft.Name {
			case "make":
				return w.nodeString(w.namelessType(v.Args[0])), nil
			case "new":
				return "*" + w.nodeString(w.namelessType(v.Args[0])), nil
			case "append":
				return w.varValueType(v.Args[0], 0)
			case "recover":
				return "interface{}", nil
			case "len", "cap", "copy":
				return "int", nil
			case "complex":
				return "complex128", nil
			case "real":
				return "float64", nil
			case "imag":
				return "float64", nil
			}
			if isBuiltinType(ft.Name) {
				return ft.Name, nil
			}
			typ := w.curPackage.findCallType(ft.Name, index)
			if typ != nil {
				return w.nodeString(w.namelessType(typ)), nil
			}
			//if var is func() type
			vs, _, n := w.resolveName(ft.Name)
			if n >= 0 {
				if vs != nil {
					typ, err := w.varValueType(vs, n)
					if err == nil {
						if strings.HasPrefix(typ, "func(") {
							expr, err := parser.ParseExpr(typ + "{}")
							if err == nil {
								if fl, ok := expr.(*ast.FuncLit); ok {
									retType := funcRetType(fl.Type, index)
									if retType != nil {
										return w.nodeString(w.namelessType(retType)), nil
									}
								}
							}
						}
					}
				}
			}
			return "", fmt.Errorf("unknown funcion %s %s", w.curPackageName, ft.Name)
		case *ast.SelectorExpr:
			typ, err := w.varValueType(ft.X, index)
			if err == nil {
				if strings.HasPrefix(typ, "*") {
					typ = typ[1:]
				}
				retType := w.curPackage.findCallType(typ+"."+ft.Sel.Name, index)
				if retType != nil {
					return w.nodeString(w.namelessType(retType)), nil
				}
				if ft.Sel.Name == "test6" {
					retType := w.curPackage.findCallType(typ[1:]+"."+ft.Sel.Name, index)
					log.Fatalln(retType)
					log.Fatalln(w.varFunctionType(typ, ft.Sel.Name, index))
					log.Fatalln(typ, ft.Sel.Name)
				}
			}
			switch st := ft.X.(type) {
			case *ast.Ident:
				return w.varFunctionType(st.Name, ft.Sel.Name, index)
			case *ast.CallExpr:
				return w.varValueType(st, index)
			case *ast.SelectorExpr:
				typ, err := w.varValueType(st, index)
				if err == nil {
					return w.varFunctionType(typ, ft.Sel.Name, index)
				}
			case *ast.IndexExpr:
				typ, err := w.varValueType(st.X, index)
				if err == nil {
					if strings.HasPrefix(typ, "[]") {
						return w.varFunctionType(typ[2:], ft.Sel.Name, index)
					}
				}
			}
		case *ast.FuncLit:
			retType := funcRetType(ft.Type, index)
			if retType != nil {
				return w.nodeString(w.namelessType(retType)), nil
			}
		case *ast.CallExpr:
			typ, err := w.varValueType(v.Fun, 0)
			if err == nil && strings.HasPrefix(typ, "func(") {
				expr, err := parser.ParseExpr(typ + "{}")
				if err == nil {
					if fl, ok := expr.(*ast.FuncLit); ok {
						retType := funcRetType(fl.Type, index)
						if retType != nil {
							return w.nodeString(w.namelessType(retType)), nil
						}
					}
				}
			}
		}
		return "", fmt.Errorf("not a known function %T %v", v.Fun, w.nodeString(v.Fun))
	//fix_code
	case *ast.MapType:
		return fmt.Sprintf("map[%s](%s)", w.nodeString(w.namelessType(v.Key)), w.nodeString(w.namelessType(v.Value))), nil
	//fix_code
	case *ast.ArrayType:
		return fmt.Sprintf("[]%s", w.nodeString(w.namelessType(v.Elt))), nil
	//fix_code return functype
	case *ast.FuncType:
		return w.nodeString(w.namelessType(v)), nil
	case *ast.IndexExpr:
		typ, err := w.varValueType(v.X, index)
		if err == nil {
			if strings.HasPrefix(typ, "[]") {
				return typ[2:], nil
			}
		}
	case *ast.ChanType:
		typ, err := w.varValueType(v.Value, index)
		if err == nil {
			if v.Dir == ast.RECV {
				return "<-chan " + typ, nil
			} else if v.Dir == ast.SEND {
				return "chan<- " + typ, nil
			}
			return "chan " + typ, nil
		}
	default:
		return "", fmt.Errorf("unknown value type %T", vi)
	}
	panic("unreachable")
}

// resolveName finds a top-level node named name and returns the node
// v and its type t, if known.
func (w *Walker) resolveName(name string) (v interface{}, t interface{}, n int) {
	for _, file := range w.curPackage.apkg.Files {
		for _, di := range file.Decls {
			switch d := di.(type) {
			case *ast.GenDecl:
				switch d.Tok {
				case token.VAR:
					for _, sp := range d.Specs {
						vs := sp.(*ast.ValueSpec)
						for i, vname := range vs.Names {
							if vname.Name == name {
								if len(vs.Values) == 1 {
									return vs.Values[0], vs.Type, i
								}
								return nil, vs.Type, i
							}
						}
					}
				}
			}
		}
	}
	return nil, nil, -1
}

// constDepPrefix is a magic prefix that is used by constValueType
// and walkConst to signal that a type isn't known yet. These are
// resolved at the end of walking of a package's files.
const constDepPrefix = "const-dependency:"

func (w *Walker) walkConst(vs *ast.ValueSpec) {
	for _, ident := range vs.Names {
		litType := ""
		if vs.Type != nil {
			litType = w.nodeString(vs.Type)
		} else {
			litType = w.lastConstType
			if vs.Values != nil {
				if len(vs.Values) != 1 {
					log.Fatalf("const %q, values: %#v", ident.Name, vs.Values)
				}
				var err error
				litType, err = w.constValueType(vs.Values[0])
				if err != nil {
					//fix_code
					if *verbose {
						log.Printf("unknown kind in const %q (%T): %v", ident.Name, vs.Values[0], err)
					}
					litType = "unknown-type"
				}
			}
		}
		if strings.HasPrefix(litType, constDepPrefix) {
			dep := litType[len(constDepPrefix):]
			w.constDep[ident.Name] = &constInfo{dep, ident.Pos()}
			continue
		}
		if litType == "" {
			//fix_code 			
			//log.Fatalf("unknown kind in const %q", ident.Name)
			if *verbose {
				log.Printf("unknown kind in const %q", ident.Name)
			}
			continue
		}
		w.lastConstType = litType

		w.curPackage.consts[ident.Name] = litType

		if IsExported(ident.Name) {
			w.emitFeature(fmt.Sprintf("const %s %s", ident, litType), ident.Pos())
		}
	}
}

func (w *Walker) resolveConstantDeps() {
	var findConstType func(string) string
	findConstType = func(ident string) string {
		if dep, ok := w.constDep[ident]; ok {
			return findConstType(dep.typ)
		}
		if t, ok := w.curPackage.consts[ident]; ok {
			return t
		}
		return ""
	}
	for ident, info := range w.constDep {
		if !IsExported(ident) {
			continue
		}
		t := findConstType(ident)
		if t == "" {
			//fix_code
			if *verbose {
				log.Printf("failed to resolve constant %q", ident)
			}
			continue
			//log.Fatalf("failed to resolve constant %q", ident)
		}
		w.emitFeature(fmt.Sprintf("const %s %s", ident, t), info.pos)
	}
}

func (w *Walker) walkVar(vs *ast.ValueSpec) {
	for n, ident := range vs.Names {
		typ := ""
		if vs.Type != nil {
			typ = w.nodeString(vs.Type)
		} else {
			if len(vs.Values) == 0 {
				log.Fatalf("no values for var %q", ident.Name)
			}
			if len(vs.Values) > 1 {
				log.Fatalf("more than 1 values in ValueSpec not handled, var %q", ident.Name)
			}
			var err error
			typ, err = w.varValueType(vs.Values[0], n)
			if err != nil {
				//fix_code
				if *verbose {
					log.Printf("unknown type of variable %q, type %T, error = %v, pos=%s",
						ident.Name, vs.Values, err, w.fset.Position(vs.Pos()))
				}
				typ = "unknown-type"
			}
		}
		w.curPackage.vars[ident.Name] = typ
		if IsExported(ident.Name) {
			w.emitFeature(fmt.Sprintf("var %s %s", ident, typ), ident.Pos())
		}
	}
}

func (w *Walker) nodeString(node interface{}) string {
	if node == nil {
		return ""
	}
	var b bytes.Buffer
	printer.Fprint(&b, w.fset, node)
	return b.String()
}

func (w *Walker) nodeDebug(node interface{}) string {
	if node == nil {
		return ""
	}
	var b bytes.Buffer
	ast.Fprint(&b, w.fset, node, nil)
	return b.String()
}

func (w *Walker) noteInterface(name string, it *ast.InterfaceType) {
	w.interfaces[pkgSymbol{w.curPackageName, name}] = it
}

func (w *Walker) walkTypeSpec(ts *ast.TypeSpec) {
	name := ts.Name.Name
	if !IsExported(name) {
		return
	}
	switch t := ts.Type.(type) {
	case *ast.StructType:
		w.walkStructType(name, t)
	case *ast.InterfaceType:
		w.walkInterfaceType(name, t)
	default:
		w.emitFeature(fmt.Sprintf("type %s %s", name, w.nodeString(ts.Type)), t.Pos()-token.Pos(len(name)+1))
	}
}

func (w *Walker) walkStructType(name string, t *ast.StructType) {
	typeStruct := fmt.Sprintf("type %s struct", name)
	w.emitFeature(typeStruct, t.Pos()-token.Pos(len(name)+1))
	pop := w.pushScope(typeStruct)
	defer pop()
	for _, f := range t.Fields.List {
		typ := f.Type
		for _, name := range f.Names {
			if IsExported(name.Name) {
				w.emitFeature(fmt.Sprintf("%s %s", name, w.nodeString(w.namelessType(typ))), name.Pos())
			}
		}
		if f.Names == nil {
			switch v := typ.(type) {
			case *ast.Ident:
				if IsExported(v.Name) {
					w.emitFeature(fmt.Sprintf("embedded %s", v.Name), v.Pos())
				}
			case *ast.StarExpr:
				switch vv := v.X.(type) {
				case *ast.Ident:
					if IsExported(vv.Name) {
						w.emitFeature(fmt.Sprintf("embedded *%s", vv.Name), vv.Pos())
					}
				case *ast.SelectorExpr:
					w.emitFeature(fmt.Sprintf("embedded %s", w.nodeString(typ)), v.Pos())
				default:
					log.Fatalf("unable to handle embedded starexpr before %T", typ)
				}
			case *ast.SelectorExpr:
				w.emitFeature(fmt.Sprintf("embedded %s", w.nodeString(typ)), v.Pos())
			default:
				log.Fatalf("unable to handle embedded %T", typ)
			}
		}
	}
}

// method is a method of an interface.
type method struct {
	name string // "Read"
	sig  string // "([]byte) (int, error)", from funcSigString
	ft   *ast.FuncType
	pos  token.Pos
	recv ast.Expr
}

// interfaceMethods returns the expanded list of exported methods for an interface.
// The boolean complete reports whether the list contains all methods (that is, the
// interface has no unexported methods).
// pkg is the complete package name ("net/http")
// iname is the interface name.
func (w *Walker) interfaceMethods(pkg, iname string) (methods []method, complete bool) {
	t, ok := w.interfaces[pkgSymbol{pkg, iname}]
	if !ok {
		//fix_code
		if *verbose {
			log.Printf("failed to find interface %s.%s", pkg, iname)
		}
		//log.Fatalf("failed to find interface %s.%s", pkg, iname)
		return
	}

	complete = true
	for _, f := range t.Methods.List {
		typ := f.Type
		switch tv := typ.(type) {
		case *ast.FuncType:
			for _, mname := range f.Names {
				if IsExported(mname.Name) {
					ft := typ.(*ast.FuncType)
					methods = append(methods, method{
						name: mname.Name,
						sig:  w.funcSigString(ft),
						ft:   ft,
						pos:  f.Pos(),
					})
				} else {
					complete = false
				}
			}
		case *ast.Ident:
			embedded := typ.(*ast.Ident).Name
			if embedded == "error" {
				methods = append(methods, method{
					name: "Error",
					sig:  "() string",
					ft: &ast.FuncType{
						Params: nil,
						Results: &ast.FieldList{
							List: []*ast.Field{
								&ast.Field{
									Type: &ast.Ident{
										Name: "string",
									},
								},
							},
						},
					},
					pos: f.Pos(),
				})
				continue
			}
			if !IsExported(embedded) {
				log.Fatalf("unexported embedded interface %q in exported interface %s.%s; confused",
					embedded, pkg, iname)
			}
			m, c := w.interfaceMethods(pkg, embedded)
			methods = append(methods, m...)
			complete = complete && c
		case *ast.SelectorExpr:
			lhs := w.nodeString(tv.X)
			rhs := w.nodeString(tv.Sel)
			fpkg, ok := w.selectorFullPkg[lhs]
			if !ok {
				log.Fatalf("can't resolve selector %q in interface %s.%s", lhs, pkg, iname)
			}
			m, c := w.interfaceMethods(fpkg, rhs)
			methods = append(methods, m...)
			complete = complete && c
		default:
			log.Fatalf("unknown type %T in interface field", typ)
		}
	}
	return
}

func (w *Walker) walkInterfaceType(name string, t *ast.InterfaceType) {
	methNames := []string{}
	pop := w.pushScope("type " + name + " interface")
	methods, complete := w.interfaceMethods(w.curPackageName, name)
	w.packageMap[w.curPackageName].interfaceMethods[name] = methods
	for _, m := range methods {
		methNames = append(methNames, m.name)
		w.emitFeature(fmt.Sprintf("%s%s", m.name, m.sig), m.pos)
	}
	if !complete {
		// The method set has unexported methods, so all the
		// implementations are provided by the same package,
		// so the method set can be extended. Instead of recording
		// the full set of names (below), record only that there were
		// unexported methods. (If the interface shrinks, we will notice
		// because a method signature emitted during the last loop,
		// will disappear.)
		w.emitFeature("unexported methods", 0)
	}
	pop()

	if !complete {
		return
	}

	sort.Strings(methNames)
	if len(methNames) == 0 {
		w.emitFeature(fmt.Sprintf("type %s interface {}", name), t.Pos()-token.Pos(len(name)+1))
	} else {
		w.emitFeature(fmt.Sprintf("type %s interface { %s }", name, strings.Join(methNames, ", ")), t.Pos()-token.Pos(len(name)+1))
	}
}

func baseTypeName(x ast.Expr) (name string, imported bool) {
	switch t := x.(type) {
	case *ast.Ident:
		return t.Name, false
	case *ast.SelectorExpr:
		if _, ok := t.X.(*ast.Ident); ok {
			// only possible for qualified type names;
			// assume type is imported
			return t.Sel.Name, true
		}
	case *ast.StarExpr:
		return baseTypeName(t.X)
	}
	return
}

func (w *Walker) peekFuncDecl(f *ast.FuncDecl) {
	//fix_code
	var fname = f.Name.Name
	var recv ast.Expr
	if f.Recv != nil {
		recvTypeName, imp := baseTypeName(f.Recv.List[0].Type)
		if imp {
			return
		}
		fname = recvTypeName + "." + f.Name.Name
		recv = f.Recv.List[0].Type
	}
	// Record return type for later use.
	if f.Type.Results != nil && len(f.Type.Results.List) >= 1 {
		w.curPackage.functions[fname] = method{
			name: fname,
			sig:  w.funcSigString(f.Type),
			ft:   f.Type,
			pos:  f.Pos(),
			recv: recv,
		}
	}
}

func (w *Walker) walkFuncDecl(f *ast.FuncDecl) {
	if !IsExported(f.Name.Name) {
		return
	}
	if f.Recv != nil {
		// Method.
		recvType := w.nodeString(f.Recv.List[0].Type)
		keep := IsExported(recvType) ||
			(strings.HasPrefix(recvType, "*") &&
				IsExported(recvType[1:]))
		if !keep {
			return
		}
		w.emitFeature(fmt.Sprintf("method (%s) %s%s", recvType, f.Name.Name, w.funcSigString(f.Type)), f.Name.Pos())
		return
	}
	// Else, a function
	w.emitFeature(fmt.Sprintf("func %s%s", f.Name.Name, w.funcSigString(f.Type)), f.Name.Pos())
}

func (w *Walker) funcSigString(ft *ast.FuncType) string {
	var b bytes.Buffer
	writeField := func(b *bytes.Buffer, f *ast.Field) {
		if n := len(f.Names); n > 1 {
			for i := 0; i < n; i++ {
				if i > 0 {
					b.WriteString(", ")
				}
				b.WriteString(w.nodeString(w.namelessType(f.Type)))
			}
		} else {
			b.WriteString(w.nodeString(w.namelessType(f.Type)))
		}
	}
	b.WriteByte('(')
	if ft.Params != nil {
		for i, f := range ft.Params.List {
			if i > 0 {
				b.WriteString(", ")
			}
			writeField(&b, f)
		}
	}
	b.WriteByte(')')
	if ft.Results != nil {
		nr := 0
		for _, f := range ft.Results.List {
			if n := len(f.Names); n > 1 {
				nr += n
			} else {
				nr++
			}
		}
		if nr > 0 {
			b.WriteByte(' ')
			if nr > 1 {
				b.WriteByte('(')
			}
			for i, f := range ft.Results.List {
				if i > 0 {
					b.WriteString(", ")
				}
				writeField(&b, f)
			}
			if nr > 1 {
				b.WriteByte(')')
			}
		}
	}
	return b.String()
}

// namelessType returns a type node that lacks any variable names.
func (w *Walker) namelessType(t interface{}) interface{} {
	ft, ok := t.(*ast.FuncType)
	if !ok {
		return t
	}
	return &ast.FuncType{
		Params:  w.namelessFieldList(ft.Params),
		Results: w.namelessFieldList(ft.Results),
	}
}

// namelessFieldList returns a deep clone of fl, with the cloned fields
// lacking names.
func (w *Walker) namelessFieldList(fl *ast.FieldList) *ast.FieldList {
	fl2 := &ast.FieldList{}
	if fl != nil {
		for _, f := range fl.List {
			fl2.List = append(fl2.List, w.namelessField(f))
		}
	}
	return fl2
}

// namelessField clones f, but not preserving the names of fields.
// (comments and tags are also ignored)
func (w *Walker) namelessField(f *ast.Field) *ast.Field {
	return &ast.Field{
		Type: f.Type,
	}
}

func (w *Walker) emitFeature(feature string, pos token.Pos) {
	if !w.wantedPkg[w.curPackageName] {
		return
	}
	more := strings.Index(feature, "\n")
	if more != -1 {
		if len(feature) <= 1024 {
			feature = strings.Replace(feature, "\n", " ", 1)
			feature = strings.Replace(feature, "\n", ";", -1)
			feature = strings.Replace(feature, "\t", " ", -1)
		} else {
			feature = feature[:more] + " ...more"
		}
		if *verbose {
			log.Printf("feature contains newlines: %v, %s", feature, w.fset.Position(pos))
		}
	}
	f := strings.Join(w.scope, w.sep) + w.sep + feature

	if _, dup := w.features[f]; dup {
		w.features[f] = append(w.features[f], pos)
		return
	}

	w.features[f] = append(w.features[f], pos)
	//	if *verbose {
	//		log.Printf("feature: %s", f)
	//	}
}

func strListContains(l []string, s string) bool {
	for _, v := range l {
		if v == s {
			return true
		}
	}
	return false
}
