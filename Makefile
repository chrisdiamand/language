SRC = builtin_types.c \
      class.c 		  \
      compiler.c      \
      dict.c          \
      gc.c            \
      main.c          \
      method.c        \
      object.c        \
      run.c           \
      scanner.c       \
      stack.c         \
      state.c         \
      type.c
OUT = lang
BUILDDIR = Build
LIBS =
CC = clang
CFLAGS = -Wall -g -pedantic -std=c99 -O0

OBJ = $(patsubst %.c,$(BUILDDIR)/%.o,$(SRC))

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(BUILDDIR)/%.o: $(1)%.c $(INCLUDES) $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OUT) $(OBJ)
	rmdir --ignore-fail-on-non-empty $(BUILDDIR)
