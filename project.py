src = ["builtin_types.cc",
       "class.cc",
       "gc.cc",
       "main.cc",
       "method.cc",
       "object.cc",
       "parse_tree.cc",
       "run.cc",
       "scanner.cc",
       "src_to_pt.cc",
       "state.cc",
       "type.cc"]
lang = CProg("lang", src)

makefile = Makefile([lang])
