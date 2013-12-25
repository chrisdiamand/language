src = ["builtin_types.c",
       "class.c",
       "compiler.c",
       "dict.c",
       "gc.c",
       "main.c",
       "method.c",
       "object.c",
       "run.c",
       "scanner.c",
       "stack.c",
       "state.c",
       "type.c"]
lang = CProg("lang", src)

makefile = Makefile([lang])
