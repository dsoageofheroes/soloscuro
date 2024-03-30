INCS = \
-I ../orx/code/include/ \
-I ../orx/code/include/core/ \
-I ../libgff/include \
-I /usr/local/include \
-I inc/ \
-I "../orx/code/build/template/include/[+nuklear nuklear]/" \
-I "code/build/template/include/" \
-I src/

LIBS = \
-L ../orx/code/lib/dynamic/ \
-L ../libgff/build/src/ \
-L ../libgff/build/ext/ \
-lgff \
-lxmi2mid \
-lorxd \
-lssl \
-lcrypto \
-lprotobuf-c \
-lm

#SRCS = src/soloscuro.c  src/register_gff.c src/window_generator.c src/ui/button.c src/ui/window.c src/ui/font.c src/ui/ds1/*.c src/state.c src/sol_window.c src/eng/*.c

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=obj/%.o)

CFLAGS = $(INCS) -Wall -Werror -g -fsanitize=address

all: $(OBJS)
	cd ../ ; protoc --c_out=client/src/ dso.proto
	gcc $(CFLAGS) obj/*.o -L/usr/local/lib $(LIBS) -o client && LD_LIBRARY_PATH=../orx/code/lib/dynamic/:../libgff/build/src/:../libgff/build/ext/ ./client first second

obj:
	mkdir -p obj

obj/%.o: src/%.c inc/*.h | obj
	echo $(OBJS)
	gcc $(CFLAGS) -c $< -o $@

vi:
	 CPATH=`pwd`/inc/ vim src/main.c
