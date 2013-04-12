#include <assert.h>

#include "class.h"
#include "gc.h"
#include "type.h"

struct type_t *type_new(struct class_t *cl)
{
    struct type_t *T = GC_malloc(sizeof(struct type_t));
    assert(cl);
    if (cl->nparams > 0)
        T->params = GC_malloc(sizeof(struct type_t *) * cl->nparams);
    return T;
}

struct type_t *type_from_class(struct state *S, struct class_t *cl)
{
    struct type_t *T = type_new(S->class_class);
    T->cl = S->class_class;
    T->v.cl = cl;
    return T;
}

struct type_t *type_from_method(struct state *S, struct method_t *method)
{
    struct type_t *T = type_new(S->class_method);
    T->cl = S->class_method;
    T->v.method = method;
    return T;
}
