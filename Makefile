
OBJ=dict.o gc.o main.o parser.o scanner.o
OUT=lang
LIBS=
CC=gcc
CFLAGS=-Wall -g -pedantic -ansi

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

clean:
	rm -f $(OUT) $(OBJ)

