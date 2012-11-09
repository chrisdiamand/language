
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gc.h"

typedef struct
{
    unsigned int    refcount    : sizeof(unsigned int) * 8 - 2;
    unsigned int    atomic      : 1;
    unsigned int    locked      : 1;
} Header;

static Header *alloc_header(size_t size)
{
    Header *H = malloc(size + sizeof(Header));
    H->refcount = 1;
    H->atomic = H->locked = 0;
    return H;
}

void *GC_malloc_actual(size_t size)
{
    return alloc_header(size) + 1;
}

void *GC_malloc_atomic_actual(size_t size)
{
    Header *H = alloc_header(size);
    H->atomic = 1;
    return H + 1;
}

char *GC_strdup_actual(const char *str)
{
    size_t len = strlen(str);
    char *ret = GC_malloc_atomic(len + 1);
    memcpy(ret, str, len + 1);
    return ret;
}

void *GC_realloc_actual(void *ptr, size_t size)
{
    if (ptr)
    {
        Header *H = ((Header *) ptr) - 1;
        H = realloc(H, size);
        return H + 1;
    }
    /* If ptr == NULL */
    return GC_malloc_actual(size);
}

void GC_ref(void *ptr)
{
    Header *H = ((Header *) ptr) - 1;
    H->refcount++;
}

void GC_unref(void *ptr)
{
    Header *H = ((Header *) ptr) - 1;
    H->refcount--;
    if (H->refcount == 0) /* FIXME: Needs to do more */
        free(H);
}

