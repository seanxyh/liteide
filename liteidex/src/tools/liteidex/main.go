package main

import (
	"os"
)

func main() {
	liteide(os.Args)
}

func init() {
	RegCmd("version", func(args []byte) []byte {
		return []byte("liteidex(go) version 15.0")
	})
}
