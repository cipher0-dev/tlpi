package main

import (
	"io"
	"log"
	"net"
)

func main() {
	ln, err := net.Listen("tcp", "127.0.0.1:12345")
	if err != nil {
		log.Fatal(err)
	}
	defer ln.Close()

	log.Println("listening on 127.0.0.1:12345")

	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Println("accept error:", err)
			continue
		}

		go handle(conn)
	}
}

func handle(conn net.Conn) {
	defer conn.Close()

	// Echo everything back to the client
	if _, err := io.Copy(conn, conn); err != nil {
		log.Println("conn error:", err)
	}
}
