// this program just demonstrates that go's mutex can be unlocked by a goroutine
// that didn't originally lock it and thus doesn't have ownership over the lock

package main

import "sync"

func main() {
	var mu sync.Mutex
	unlockme := make(chan struct{})
	done := make(chan struct{})

	go func() {
		<-unlockme
		println("goroutine: calling Unlock()")
		mu.Unlock()
		println("goroutine: Unlock() called")
		close(done)
	}()

	println("main: calling Lock()")
	mu.Lock()
	close(unlockme)
	<-done
}
