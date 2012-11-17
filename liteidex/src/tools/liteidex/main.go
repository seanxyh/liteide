package main

import (
	"os"
	"strings"
)

func main() {
	liteide(os.Args)
}

var (
	Env = os.Environ()
)

func init() {
	RegCmd("version", func(args []byte) (error, []byte) {
		return nil, []byte("liteidex(go) version 15.0")
	})
	RegCmd("setenv", func(args []byte) (error, []byte) {
		Env = strings.Split(string(args), "\n")
		return nil, nil
	})
}
