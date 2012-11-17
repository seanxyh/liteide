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
	RegCmd("version", func(args []byte) ([]byte, error) {
		return []byte("liteidex(go) version 15.0"), nil
	})
	RegCmd("setenv", func(args []byte) ([]byte, error) {
		Env = strings.Split(string(args), "\n")
		return nil, nil
	})
}
