#ifndef PTREE_H
#define PTREE_H

// Parse tree structure

enum pt_t {
    PT_CLASS_DEF,
    PT_VAR_DEF,
    PT_FUNC_DEF
};

struct pt {
    enum pt_t       type;
    union {
        int         n;
    } v;
};

struct pt *pt_class_def(void);

#endif
