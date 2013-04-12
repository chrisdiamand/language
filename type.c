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
