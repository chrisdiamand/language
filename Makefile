
OBJ=builtin_types.o \
    class.o 		\
    compiler.o      \
    dict.o          \
    gc.o            \
    main.o          \
    method.o        \
	object.o        \
    scanner.o       \
    stack.o
OUT=lang
LIBS=
CC=gcc
CFLAGS=-Wall -g -pedantic -ansi

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

clean:
	rm -f $(OUT) $(OBJ)

