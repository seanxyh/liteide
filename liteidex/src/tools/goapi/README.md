goapi tool
=====

 list go package api information, clone of go/cmd/api

		usage: api [std|all|package...|local-dir]
		       api std
		       api -default_ctx=true fmt flag
		       api -default_ctx=true -a ./cmd/go
		       api -default_ctx=true -cursor_type main.go:121 f:/hello 
		  -a=false: extract for all declarations
		  -allow_new=true: allow API additions
		  -c="": optional filename to check API against
		  -cursor_type="": print cursor node type "file.go:pos"
		  -custom_ctx="": optional comma-separated list of <goos>-<goarch>[-cgo] to override default contexts.
		  -default_ctx=false: extract for default context
		  -dep=true: parser package imports
		  -e=true: extract for all embedded methods
		  -except="": optional filename of packages that are allowed to change without triggering a failure in the tool
		  -next="": optional filename of tentative upcoming API features for the next release. This file can be lazily maintained. It only affects the delta warnings from the -c file printed on success.
		  -pos=false: addition token position
		  -sep=",": setup separators
		  -v=false: verbose debugging