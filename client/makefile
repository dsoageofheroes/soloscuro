INCS = \
-I ../ext/orx/code/include/ \
-I ../ext/orx/code/include/core/ \
-I ../ext/libsoloscuro/ext/libgff/include \
-I /usr/local/include \
-I inc/ \
-I ../ext/libsoloscuro/inc \
-I "../ext/orx/code/build/template/include/[+nuklear nuklear]/" \
-I "code/build/template/include/" \
-I src/

LIBS = \
-L ../ext/orx/code/lib/dynamic/ \
-L ../ext/libsoloscuro/lib/ \
-lgff \
-lxmi2mid \
-lorxd \
-lssl \
-lcrypto \
-lprotobuf-c \
-lsoloscuro \
-lm

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=obj/%.o)

CFLAGS = $(INCS) -Wall -Werror -g -fsanitize=address

all: $(OBJS) ../ext/libsoloscuro/lib/libsoloscuro.so
	gcc $(CFLAGS) obj/*.o -L/usr/local/lib $(LIBS) -o client && LD_LIBRARY_PATH=../ext/orx/code/lib/dynamic/:../ext/libsoloscuro/lib/ ./client first second

../ext/libsoloscuro/lib/libsoloscuro.so:
	git submodule update --init
	cd ../ext/libsoloscuro ; make

src/dso.pb-c.c src/dso.pb-c.h:
	cd ../ ; protoc --c_out=client/src/ dso.proto

obj:
	mkdir -p obj

obj/%.o: src/%.c inc/*.h src/dso.pb-c.h | obj 
	echo $(OBJS)
	gcc $(CFLAGS) -c $< -o $@

vi:
	 #CPATH=`pwd`/inc/ vim src/main.c
	 CPATH=`pwd`/inc/:`pwd`/../ext/libgff/inc:`pwd`/../ext/libsoloscuro/inc vim src/main.c

ctags:
	ctags -Ru inc src ../ext/libsoloscuro/ext/libgff/include ../ext/libsoloscuro/ext/libgff/src ../ext/libsoloscuro/inc ../ext/libsoloscuro/src
