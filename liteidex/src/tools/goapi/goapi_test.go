// Copyright 2011 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"flag"
	"fmt"
	"go/build"
	"io/ioutil"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"testing"
)

var (
	updateGolden = flag.Bool("updategolden", false, "update golden files")
)

func TestGolden(t *testing.T) {
	td, err := os.Open("testdata/src/pkg")

	if err != nil {
		t.Fatal(err)
	}
	fis, err := td.Readdir(0)
	if err != nil {
		t.Fatal(err)
	}
	context := &build.Default
	context.GOROOT = "testdata"

	for _, fi := range fis {
		if !fi.IsDir() {
			continue
		}
		w := NewWalker()
		w.wantedPkg[fi.Name()] = true
		w.context = context

		goldenFile := filepath.Join("testdata", "src", "pkg", fi.Name(), "golden.txt")

		w.WalkPackage(fi.Name())

		if *updateGolden {
			os.Remove(goldenFile)
			f, err := os.Create(goldenFile)
			if err != nil {
				t.Fatal(err)
			}
			for _, feat := range w.Features("") {
				fmt.Fprintf(f, "%s\n", feat)
			}
			f.Close()
		}

		bs, err := ioutil.ReadFile(goldenFile)
		if err != nil {
			t.Fatalf("opening golden.txt for package %q: %v", fi.Name(), err)
		}
		wanted := strings.Split(string(bs), "\n")
		sort.Strings(wanted)

		features := make(map[string]bool)
		for _, p := range w.Features("") {
			features[p] = true
		}

		for _, feature := range wanted {
			if feature == "" {
				continue
			}
			_, ok := features[feature]
			if !ok {
				t.Errorf("package %s: missing feature %q", fi.Name(), feature)
			}
			delete(features, feature)
		}
		for _, feature := range features {
			t.Errorf("package %s: extra feature not in golden file: %q", fi.Name(), feature)
		}
	}
}
