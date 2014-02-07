#ifndef PARSE_TREE_H
#define PARSE_TREE_H

// Parse tree structure

enum pt_t {
    PT_CLASS_DEF,
    PT_VAR_DEF,
    PT_FUNC_DEF
};

class ParseTree { };

class ParseTree_ClassDef : ParseTree {
public:
    ParseTree(char *);
    char *name;
};

#endif /* PARSE_TREE_H */
