
#ifndef GC_H
#define GC_H

#include <stdio.h>
#include <string.h>

void *GC_malloc_actual(size_t);
void *GC_malloc_atomic_actual(size_t);
void *GC_realloc_actual(void *, size_t);
char *GC_strdup_actual(const char *);

#define GC_malloc(SIZE) GC_malloc_actual(SIZE)
#define GC_malloc_atomic(SIZE) GC_malloc_atomic_actual(SIZE)
#define GC_strdup(STR) GC_strdup_actual(STR)
#define GC_realloc(PTR, SIZE) GC_realloc_actual(PTR, SIZE)

void GC_ref(void *);
void GC_unref(void *);

#endif

