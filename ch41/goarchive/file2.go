// Package go_archive tests how go compiles .go files into archives
package goarchive

import "os"

func F2() {
	println("f2")
	println(os.Chdir)
}
