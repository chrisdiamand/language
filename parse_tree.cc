#include "gc.h"
#include "parse_tree.h"

static struct pt *pt_new(enum pt_t type) {
    struct pt *ret = GC_malloc(sizeof(*ret));
    ret->type = type;
    return ret;
}

struct pt *pt_class_def(void) {
    struct pt *ret = pt_new(PT_CLASS_DEF);
    return ret;
}
