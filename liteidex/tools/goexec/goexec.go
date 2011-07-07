package main

import (
	"exec"
	"os"
	"fmt"
	"path"
)

func main() {	
	args := os.Args[1:]

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
	
	fmt.Printf("Starting Process %s ...\n\n",path.Clean(filePath))
	
	cmd := exec.Command(filePath,args[1:]...)
	cmd.Dir = workPath
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	
	err = cmd.Run()
	
	if err != nil {
		fmt.Println("\nEnd Process",err)
	} else {
		fmt.Println("\nEnd Process","exit status 0")
	}

	wait_exit()
}

func wait_exit() {
	fmt.Println("\nPress enter key to continue")
	var s = [256]byte{}
	os.Stdin.Read(s[:])		
	os.Exit(0)	
}