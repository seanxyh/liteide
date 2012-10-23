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

// Flags
var (
	// TODO(bradfitz): once Go 1.1 comes out, allow the -c flag to take a comma-separated
	// list of files, rather than just one.
	verbose    = flag.Bool("v", false, "verbose debugging")
	allmethods = flag.Bool("e", false, "show all embedded methods")
	alldecls   = flag.Bool("a", false, "extract documentation for all package-level declarations")
	showpos    = flag.Bool("p", false, "show token pos tag")
	separate   = flag.String("sep", ",", "set token separate string")
)

type ss struct {
}

func (p *ss) test() (b map[int]string) {
	return
}

func newss() *ss {
	return &ss{}
}

var (
	s0 ss
	m0 = s0.test()
	s1 = newss()
	M1 = s1.test()
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
}

func NewPackage() *Package {
	return &Package{
		interfaceMethods: make(map[string]([]method)),
		interfaces:       make(map[string]*ast.InterfaceType),
		structs:          make(map[string]*ast.StructType),
		types:            make(map[string]ast.Expr),
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

func (p *Package) findFunction(name string) *ast.FuncType {
	return findFunction(p.dpkg.Funcs, name)
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
	prevConstType   map[pkgSymbol]string
	constDep        map[string]*constInfo // key's const identifier has type of future value const identifier
	packageState    map[string]loadState
	packageMap      map[string]*Package
	interfaces      map[pkgSymbol]*ast.InterfaceType
	functionTypes   map[pkgSymbol]string // symbol => return type
	selectorFullPkg map[string]string    // "http" => "net/http", updated by imports
	wantedPkg       map[string]bool      // packages requested on the command line
}

func NewWalker() *Walker {
	return &Walker{
		fset:            token.NewFileSet(),
		features:        make(map[string]([]token.Pos)),
		packageState:    make(map[string]loadState),
		interfaces:      make(map[pkgSymbol]*ast.InterfaceType),
		functionTypes:   make(map[pkgSymbol]string),
		packageMap:      make(map[string]*Package),
		selectorFullPkg: make(map[string]string),
		wantedPkg:       make(map[string]bool),
		prevConstType:   make(map[pkgSymbol]string),
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

//fix_code
// hardCodedConstantType is a hack until the type checker is sufficient for our needs.
// Rather than litter the code with unnecessary type annotations, we'll hard-code
// the cases we can't handle yet.
//func (w *Walker) hardCodedConstantType(name string) (typ string, ok bool) {
//	switch w.scope[0] {
//	case "pkg syscall":
//		switch name {
//		case "darwinAMD64":
//			return "bool", true
//		}
//	}
//	return "", false
//}

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
				if *verbose {
					log.Printf("not current package %s, file %s: %s", name, file, f.Name.Name)
				}
				continue
			}
			apkg.Files[file] = f
			for _, dep := range fileDeps(f) {
				bp, err := build.Import(dep, "", build.FindOnly)
				if err == nil {
					if dep != name {
						w.WalkPackage(dep, bp.Dir)
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
						w.functionTypes[pkgSymbol{w.curPackageName, name}] = name
					default:
						w.curPackage.types[name] = ts.Type
						w.functionTypes[pkgSymbol{w.curPackageName, name}] = w.nodeString(ts.Type)
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

var errTODO = errors.New("TODO")

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
		if t, ok := w.prevConstType[pkgSymbol{pkg, rhs}]; ok {
			return t, nil
		}
		return "", fmt.Errorf("unknown constant reference to %s.%s", lhs, rhs)
	case *ast.Ident:
		if v.Name == "iota" {
			return "ideal-int", nil // hack.
		}
		if v.Name == "false" || v.Name == "true" {
			return "bool", nil
		}
		if v.Obj != nil {
			if vs, ok := v.Obj.Decl.(*ast.ValueSpec); ok {
				if len(vs.Values) >= 1 {
					typ, err := w.constValueType(vs.Values[0])
					if err == nil {
						return typ, err
					}
				}
			}
		}
		if t, ok := w.prevConstType[pkgSymbol{w.curPackageName, v.Name}]; ok {
			return t, nil
		}
		return constDepPrefix + v.Name, nil
	case *ast.BinaryExpr:
		//fix_code
		if v.Op == token.EQL {
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
		if left != right {
			// TODO(bradfitz): encode the real rules here,
			// rather than this mess.
			if left == "ideal-int" && right == "ideal-float" {
				return "ideal-float", nil // math.Log2E
			}
			if left == "ideal-char" && right == "ideal-int" {
				return "ideal-int", nil // math/big.MaxBase
			}
			if left == "ideal-int" && right == "ideal-char" {
				return "ideal-int", nil // text/scanner.GoWhitespace
			}
			if left == "ideal-int" && right == "Duration" {
				// Hack, for package time.
				return "Duration", nil
			}
			if left == "ideal-int" && !strings.HasPrefix(right, "ideal-") {
				return right, nil
			}
			if right == "ideal-int" && !strings.HasPrefix(left, "ideal-") {
				return left, nil
			}
			if strings.HasPrefix(left, constDepPrefix) && strings.HasPrefix(right, constDepPrefix) {
				// Just pick one.
				// e.g. text/scanner GoTokens const-dependency:ScanIdents, const-dependency:ScanFloats
				return left, nil
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

//	bool byte complex64 complex128 error float32 float64
//	int int8 int16 int32 int64 rune string
//	uint uint8 uint16 uint32 uint64 uintptr

func (w *Walker) varValueType(vi interface{}) (string, error) {
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
			typ, err := w.varValueType(v.X)
			return "*" + typ, err
		}
		return "", fmt.Errorf("unknown unary expr: %#v", v)
	case *ast.SelectorExpr:
		//fix_code
		//return "", errTODO
		// assume it is not a method.
		pkg, ok := w.selectorFullPkg[w.nodeString(v.X)]
		if ok {
			funSym := pkgSymbol{pkg, v.Sel.Name}
			if retType, ok := w.functionTypes[funSym]; ok {
				if !ast.IsExported(retType) || pkg == w.curPackageName {
					return retType, nil
				}
				return pkg + "." + retType, nil
			}
			if retType, ok := w.prevConstType[funSym]; ok {
				if !ast.IsExported(retType) || pkg == w.curPackageName {
					return retType, nil
				} else {
					return pkg + "." + retType, nil
				}
			}
		} else {
			//fix_code
			typ, err := w.varValueType(v.X)
			if strings.HasPrefix(typ, "*") {
				typ = typ[1:]
			}
			isLocal := w.curPackage.findType(typ)

			if isLocal != nil {
				//find local fuction
				funSym := pkgSymbol{w.curPackageName, typ + "." + v.Sel.Name}
				if retType, ok := w.functionTypes[funSym]; ok {
					return retType, nil
				}
				if t, ok := isLocal.(*ast.StructType); ok {
					for _, fi := range t.Fields.List {
						for _, n := range fi.Names {
							if n.Name == v.Sel.Name {
								t, err := w.varValueType(fi.Type)
								if err == nil {
									return t, nil
								}

							}
						}
					}
				}
			} else {
				//find other method			
				pos := strings.LastIndex(typ, ".")
				if err == nil && pos != -1 {
					pkg := typ[:pos]
					_, ok := w.packageState[pkg]
					if ok == false {
						pkg, ok = w.selectorFullPkg[typ[:pos]]
					}
					if ok {
						p, ok := w.packageMap[pkg]
						if ok {
							fn := p.findMethod(typ[pos+1:], v.Sel.Name)
							if fn != nil {
								ret := funcRetType(fn, 0)
								if ret != nil {
									return w.nodeString(w.namelessType(ret)), nil
								}
							}
						}
					}
				}
			}
		}
		return "", fmt.Errorf("not a package: %s", w.nodeString(v.X))
	case *ast.Ident:
		switch v.Name {
		case "true", "false":
			return "bool", nil
		case "byte", "int":
			return v.Name, nil
		}
		if v.Obj != nil {
			if vs, ok := v.Obj.Decl.(*ast.ValueSpec); ok {
				if id, ok := vs.Type.(*ast.Ident); ok {
					return id.Name, nil
				}
			}
		}
		vt := w.curPackage.findType(v.Name)
		if vt != nil {
			return w.nodeString(vt), nil
		}
		node, _, ok := w.resolveName(v.Name)
		if !ok {
			return "", fmt.Errorf("unresolved identifier: %q", v.Name)
		}
		return w.varValueType(node)
	case *ast.BinaryExpr:
		//fix_code
		if v.Op == token.EQL {
			return "bool", nil
		}
		left, err := w.varValueType(v.X)
		if err != nil {
			return "", err
		}
		right, err := w.varValueType(v.Y)
		if err != nil {
			return "", err
		}
		if left != right {
			return "", fmt.Errorf("in BinaryExpr, unhandled type mismatch; left=%q, right=%q", left, right)
		}
		return left, nil
	case *ast.ParenExpr:
		return w.varValueType(v.X)
	case *ast.CallExpr:
		var funSym pkgSymbol
		if selnode, ok := v.Fun.(*ast.SelectorExpr); ok {
			return w.varValueType(selnode)
		} else {
			funSym = pkgSymbol{w.curPackageName, w.nodeString(v.Fun)}
			if retType, ok := w.functionTypes[funSym]; ok {
				return retType, nil
			}
		}
		// maybe a function call; maybe a conversion.  Need to lookup type.
		// TODO(bradfitz): this is a hack, but arguably most of this tool is,
		// until the Go AST has type information.
		nodeStr := w.nodeString(v.Fun)
		switch nodeStr {
		case "string", "[]byte":
			return nodeStr, nil
		//fix_code
		case "make":
			if len(v.Args) >= 1 {
				return w.varValueType(v.Args[0])
			}
		case "new":
			if len(v.Args) >= 1 {
				return w.varValueType(v.Args[0])
			}
		}
		return "", fmt.Errorf("not a known function %q", nodeStr)
	//fix_code
	case *ast.MapType:
		return fmt.Sprintf("map[%s](%s)", w.nodeString(w.namelessType(v.Key)), w.nodeString(w.namelessType(v.Value))), nil
	//fix_code
	case *ast.ArrayType:
		t, err := w.varValueType(v.Elt)
		if err != nil {
			return "", err
		}
		return "[]" + t, nil
	//fix_code return functype
	case *ast.FuncType:
		var retType = "func"
		retType += "("
		var in []string
		if v.Params != nil {
			for _, t := range v.Params.List {
				in = append(in, w.nodeString(w.namelessType(t.Type)))
			}
		}
		var out []string
		if v.Results != nil {
			for _, t := range v.Results.List {
				out = append(out, w.nodeString(w.namelessType(t.Type)))
			}
		}
		return fmt.Sprintf("func(%s) (%s)", strings.Join(in, ","), strings.Join(out, ",")), nil
	default:
		return "", fmt.Errorf("unknown value type %T", vi)
	}
	panic("unreachable")
}

// resolveName finds a top-level node named name and returns the node
// v and its type t, if known.
func (w *Walker) resolveName(name string) (v interface{}, t interface{}, ok bool) {
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
								if len(vs.Values) > i {
									return vs.Values[i], vs.Type, true
								}
								return nil, vs.Type, true
							}
						}
					}
				}
			}
		}
	}
	return nil, nil, false
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
					//					if t, ok := w.hardCodedConstantType(ident.Name); ok {
					//						litType = t
					//						err = nil
					//					} else {
					//						//fix_code
					//						litType = "unknown-type"
					//						if *verbose {
					//							log.Printf("unknown kind in const %q (%T): %v", ident.Name, vs.Values[0], err)
					//						}
					//						//log.Fatalf("unknown kind in const %q (%T): %v", ident.Name, vs.Values[0], err)
					//					}
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

		w.prevConstType[pkgSymbol{w.curPackageName, ident.Name}] = litType

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
		if t, ok := w.prevConstType[pkgSymbol{w.curPackageName, ident}]; ok {
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
	for _, ident := range vs.Names {
		if !IsExported(ident.Name) {
			continue
		}

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
			typ, err = w.varValueType(vs.Values[0])
			//fix_code fix error.New()
			if strings.HasSuffix(typ, ".error") {
				typ = "error"
			}
			if err != nil {
				//fix_code
				if *verbose {
					log.Printf("unknown type of variable %q, type %T, error = %v, pos=%s",
						ident.Name, vs.Values, err, w.fset.Position(vs.Pos()))
				}
				typ = "unknown-type"
				//log.Fatalf("unknown type of variable %q, type %T, error = %v\ncode: %s",
				//	ident.Name, vs.Values[i], err, w.nodeString(vs.Values[i]))
			}
		}
		w.emitFeature(fmt.Sprintf("var %s %s", ident, typ), ident.Pos())
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
	if f.Recv != nil {
		recvTypeName, imp := baseTypeName(f.Recv.List[0].Type)
		if imp {
			return
		}
		fname = recvTypeName + "." + f.Name.Name
	}
	// Record return type for later use.
	//fix_code len >= 1
	if f.Type.Results != nil && len(f.Type.Results.List) >= 1 {
		retType := w.nodeString(w.namelessType(f.Type.Results.List[0].Type))
		w.functionTypes[pkgSymbol{w.curPackageName, fname}] = retType
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
		if *verbose {
			log.Printf("feature contains newlines: %v, %s", feature, w.fset.Position(pos))
		}
		if len(feature) <= 1024 {
			feature = strings.Replace(feature, "\n", " ", 1)
			feature = strings.Replace(feature, "\n", ";", -1)
			feature = strings.Replace(feature, "\t", " ", -1)
		} else {
			feature = feature[:more] + " ...more"
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
