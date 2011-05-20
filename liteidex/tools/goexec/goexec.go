package main

import (
	"flag"
	"exec"
	"os"
	"fmt"
)

func main() {	
	args := flag.Args()

	if len(args) < 1 {
		fmt.Println("Usage: goexec [-w work_path] <program_name> [arguments...]")
		os.Exit(0)
	}
	
	var workPath string
	var fileName string
	
	if (args[0] == "-w") {
		if (len(args) < 3) {
			fmt.Println("Usage: goexec [-w work_path] <program_name> [arguments...]")
			os.Exit(0)			
		}
		workPath = args[1]
		fileName = args[2]
		args = args[2:]
	} else {
		fileName = args[0]
	}

	filePath,err := exec.LookPath(fileName)
	if err != nil {
		fmt.Println(err)
		wait_exit()
	}
	
	fmt.Println("Starting ",args)
	fmt.Println()
		
	cmd,err := exec.Run(filePath,args,os.Environ(),workPath,exec.PassThrough,exec.PassThrough,exec.PassThrough)
	if err != nil {
		fmt.Println(err)
		os.Exit(-2)
	}
	msg,err := cmd.Wait(0)
	if err != nil {
		fmt.Println(err)
		os.Exit(-2)
	}
	fmt.Println()
	fmt.Println(msg)
	
	wait_exit()
}

func wait_exit() {
	fmt.Println("Press enter key to continue")
	var s = [256]byte{}
	os.Stdin.Read(s[:])		
	os.Exit(0)	
}