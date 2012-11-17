package main

import (
	"os"
)

func main() {
	liteide(os.Args)
}

func init() {
	RegFunc("version", func(args []byte) []byte {
		return []byte("liteide x15.0")
	})
}
