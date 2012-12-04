
OBJ=builtin_types.o compiler.o dict.o gc.o main.o method.o scanner.o stack.o type.o
OUT=lang
LIBS=
CC=gcc
CFLAGS=-Wall -g -pedantic -ansi

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

clean:
	rm -f $(OUT) $(OBJ)

