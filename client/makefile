all:
	cd ../ ; protoc --c_out=client/src/ dso.proto
	gcc -Wall src/*.c -Iinc/ -Isrc/ -I/usr/local/include -L/usr/local/lib -lprotobuf-c -lssl -lcrypto -g -o client && ./client first second
