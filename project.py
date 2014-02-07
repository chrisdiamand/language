src = ["builtin_types.c",
       "class.c",
       "dict.c",
       "gc.c",
       "main.c",
       "method.c",
       "object.c",
       "parse_tree.c",
       "run.c",
       "scanner.c",
       "src_to_pt.c",
       "stack.c",
       "state.c",
       "type.c"]
lang = CProg("lang", src)

makefile = Makefile([lang])
