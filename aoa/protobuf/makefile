all:
	echo "Targets are c, go, and rust."
	echo "IE:"
	echo "make c"
	echo "make go"
	echo "make rust"

go:
	protoc --go_out=go dso.proto
	cd go ; go run .

c:
	protoc --c_out=c dso.proto
	gcc -Wall -Ic/ -I/usr/local/include c/*.c -o dso-c-test -L/usr/local/lib -lprotobuf-c
	./dso-c-test

rust:
	cd rust/src ; ln -sf ../../dso.proto
	cd rust ; cargo build
	rust/target/debug/rust


.PHONY: c rust go
