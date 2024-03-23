all:
	protoc --go_out=. dso.proto
	protoc --c_out=c dso.proto

c:
	protoc --c_out=c dso.proto
	gcc -Wall -Ic/ -I/usr/local/include c/*.c -o dso-c-test -L/usr/local/lib -lprotobuf-c
	./dso-c-test


.PHONY: c
