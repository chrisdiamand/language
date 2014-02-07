src = ["builtin_types.c",
       "class.c",
       "dict.c",
       "gc.c",
       "main.c",
       "method.c",
       "object.c",
       "ptree.c",
       "run.c",
       "scanner.c",
       "src_to_ptree.c",
       "stack.c",
       "state.c",
       "type.c"]
lang = CProg("lang", src)

makefile = Makefile([lang])
