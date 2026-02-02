package main

import (
	"fmt"
	"log"
	"net"
	"sync/atomic"
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

var i int64

func handle(conn net.Conn) {
	defer conn.Close()

	old := atomic.AddInt64(&i, 1)

	fmt.Fprintf(conn, "Hello %d\n", old)
}
