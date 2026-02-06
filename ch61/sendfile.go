package main

import (
	"io"
	"log"
	"net"
	"os"
)

func main() {
	file, err := os.Open("./sendfile.go")
	panicErr(err)

	lis, err := net.Listen("tcp", ":")
	panicErr(err)

	log.Printf("Listening on %s", lis.Addr())
	for {
		conn, err := lis.Accept()
		panicErr(err)
		go handleConn(conn, file)
	}
}

func handleConn(conn net.Conn, file *os.File) {
	_, err := io.Copy(conn, file)
	if err != nil {
		log.Printf("error occured when copying: %v", err)
	}
}

func panicErr(err error) {
	if err != nil {
		panic(err.Error())
	}
}
