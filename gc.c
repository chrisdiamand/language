/*
 * Copyright (c) 2013 Chris Diamand
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc.h"

#define ROOT 3
#define BLACK 2
#define GREY 1
#define WHITE 0

#define HEADER(ptr) ( ((Header *)ptr) - 1 )
#define DATA(base) ( (void *)( ((Header *)base) + 1 ) )

static enum { MARK_PHASE, SWEEP_PHASE } curr_phase;

/* FIXME: Currently this means everything allocated has an *eight* byte overhead */
typedef struct
{
    unsigned int index  :  26;
    unsigned int atomic :   1;
    unsigned int col    :   2;
    size_t size;
    #ifdef GC_DEBUG
        int line;
        const char *fn;
    #endif
} Header;


static int has_done_init = 0;

static Header **heap = NULL;
static unsigned int heap_max = 0, heap_len = 0;

static void *lowest_value, *highest_value;

static unsigned int curr_pos;
static unsigned int n_greys;

static void mark_contents(Header *);
static int is_valid_pointer(void *);

#ifdef GC_DEBUG
static void gc_debug(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
}

static char *Colour(Header *b)
{
    switch (b->col)
    {
        case WHITE:     return "white";
        case GREY:      return "grey";
        case BLACK:     return "black";
        case ROOT:      return "root";
        default:        return "corrupt";
    }
}

static void print_header_info(Header *h)
{
    gc_debug("(%d): size %d (function %p[%d]): %s: %p", h->index, h->size, h->fn, h->line, Colour(h), DATA(h));
}

static void print_mem(void)
{
    int i;
    gc_debug("n_greys = %d\n", n_greys);
    for (i = 0; i < heap_len; i++)
    {
        Header *h = heap[i];
        if (h != NULL)
            print_header_info(h);
        else
            gc_debug("%d: (nil)\n", i);
    }
}
#endif

void GC_addroot(void *p)
{
    Header *h = HEADER(p);
    if (!is_valid_pointer(p))
    {
        fprintf(stderr, "GC_addroot: Invalid pointer! %p (header = %p)\n", (void *)p, (void *)h);
        exit(1);
    }
    if (h->col == GREY)
        n_greys--;
    h->col = ROOT;
    #ifdef GC_DEBUG
    gc_debug("addroot: ");
    print_header_info(h);
    #endif
}

void GC_delroot(void *p)
{
    HEADER(p)->col = BLACK;
}

static void start_mark_phase(void)
{
    int i;
    /* Mark everything white at the start of the marking phase */
    for (i = 0; i < heap_len; i++)
    {
        Header *h = heap[i];
        if (h != NULL)
        {
            if (h->col != ROOT)
                h->col = WHITE;
        }
    }
    n_greys = 0;
    curr_phase = MARK_PHASE;
    /* Then find anything marked as a root and grey its contents */
    for (i = 0; i < heap_len; i++)
    {
        Header *h = heap[i];
        if (h != NULL)
        {
            if (h->col == ROOT)
                mark_contents(h);
        }
    }
    curr_pos = 0;
}

static void start_sweep_phase(void)
{
    curr_pos = 0;
    curr_phase = SWEEP_PHASE;
}

static void do_init(void)
{
    heap_max = 1;
    heap = malloc(sizeof(Header *) * heap_max);
    heap_len = 0;

    has_done_init = 1;
    start_mark_phase();
}

static void add_header(Header *h)
{
    unsigned int i;
    void *actualptr = DATA(h);

    if (actualptr < lowest_value)
        lowest_value = actualptr;
    if (actualptr > highest_value)
        highest_value = actualptr;

    /* First try to find a free slot in the array; this is why we need the hash
     * (to reduce the search time) */
    for (i = 0; i < heap_len; i++)
    {
        if (heap[i] == NULL)
        {
            /*gc_debug("Allocating %p [%d][%d]\n", h, hash, i);*/
            heap[i] = h;
            h->index = i;
            return;
        }
    }

    /* If there were no free slots, add another one */
    if (heap_len >= heap_max)
    {
        Header **n;
        heap_max *= 2;
        n = realloc(heap, sizeof(Header *) * heap_max);
        if (n)
            heap = n;
        else
            fprintf(stderr, "realloc() failed - about to die.");
    }
    /*gc_debug("Allocating %p to %d\n", h, heap_len);*/
    h->index = heap_len;
    heap[heap_len] = h;
    heap_len++;
}

/* Find the next root that hasn't already been marked */
Header *find_next(int *pos)
{
    while (curr_pos < heap_len)
    {
        Header *h = heap[curr_pos];
        if (h != NULL)
        {
            if (  (curr_phase == MARK_PHASE && h->col == GREY)
               || (curr_phase == SWEEP_PHASE && h->col == WHITE) )
            {
                    *pos = curr_pos;
                    curr_pos++;
                    return h;
            }
        }
        curr_pos++;
    }
    return NULL;
}

/*
void sweep_some_things(void)
{
    int pos;
    Header *h = find_next(&pos);
    if (h == NULL)
    {
        start_mark_phase();
        return;
    }
    heap[pos] = NULL;
    free(h);
    gc_debug("FREE: %s[%d] type %s: %p, (header = %p)\n", h->fn, h->line, TYPENAME(h), DATA(h), h);
}
*/
void sweep_some_things(void)
{
    int i;
    for (i = 0; i < heap_len; i++)
    {
        Header *h = heap[i];
        if (h != NULL)
        {
            if (h->col == WHITE)
            {
                #ifdef GC_DEBUG
                gc_debug("FREE: %p", DATA(h));
                print_header_info(h);
                #endif
                heap[i] = NULL;
                free(h);
            }
        }
    }
    start_mark_phase();
}

/* Check if a pointer (referring to actual data, not the header) is valid */
/* Return its position if it is, -1 if not */
int is_valid_pointer(void *p)
{
    Header *base = HEADER(p);
    if (p < lowest_value || p > highest_value)
        return 0;

    if (base->index < 0 || base->index >= heap_len)
    {
        fprintf(stderr, "GC: is_valid_pointer(%p): Invalid index: %d: ", p, base->index);
        #ifdef GC_DEBUG
        print_header_info(base);
        #endif
        fprintf(stderr, "\n");
        fflush(stderr);
        exit(1);
        return 0;
    }
    if (heap[base->index] == base)
        return 1;
    fprintf(stderr, "GC: is_valid_pointer: Pointer in range was not valid: %p\n", p);
    exit(1);
    return 0;
}

/* Make something grey if it has been referred to by something else
 * but still needs to be searched */
static void make_grey(void *data)
{
    Header *h = HEADER(data);
    if (!is_valid_pointer(data))
        return;

    if (h->col == WHITE)
    {
        h->col = GREY;
        n_greys++;
    }
    #ifdef GC_DEBUG
    gc_debug("  GREY: ");
    print_header_info(h);
    #endif

}

/* Mark something as black and make everything it refers to grey */
static void mark_contents(Header *base)
{
    int i;
    void **ptr = DATA(base);
    /* If it was grey we are about to grey all its roots, so mark it black */
    if (base->col == GREY)
        n_greys--;
    if (base->col != ROOT)
        base->col = BLACK;

    if (base->size % sizeof(void *) != 0)
        return;
    for (i = 0; i < base->size / sizeof(void *); i++)
    {
        if (ptr[i])
            make_grey(ptr[i]);
    }
}

static void mark_another_grey(void)
{
    int pos;
    Header *base = find_next(&pos);
    if (n_greys == 0)
    {
        start_sweep_phase();
        return;
    }
    /* If we have got to the end of the list but there are still greys left
     * go round again */
    if (base == NULL)
    {
        curr_pos = 0;
    }
    else
    {
        mark_contents(base);
    }
}

void GC_idle(void)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        switch (curr_phase)
        {
            case MARK_PHASE:
                mark_another_grey();
                break;
            case SWEEP_PHASE:
                sweep_some_things();
                break;
        }
    }
}

static Header *allocate(size_t size)
{
    Header *base = calloc(sizeof(Header) + size, 1);
    if (!has_done_init)
    {
        do_init();
        lowest_value = DATA(base);
        highest_value = lowest_value;
    }

    add_header(base);
    /* Make it black so that memory allocated during a mark phase is not freed */
    base->col = BLACK;
    base->size = size;
    return base;
}

void *GC_malloc(size_t size)
{
    Header *h;
    h = allocate(size);
    h->atomic = 0;
    return DATA(h);
}

void *GC_malloc_atomic(size_t size)
{
    Header *h = allocate(size);
    h->atomic = 1;
    return DATA(h);
}

void *GC_realloc(void *ptr, size_t size)
{
    Header *base, *nm;
    void *ret;

    if (ptr == NULL)
        return GC_malloc(size);

    if (!is_valid_pointer(ptr))
    {
        fprintf(stderr, "GC_realloc(): Invalid pointer %p)\n", (void *)ptr);
        exit(1);
        return NULL;
    }
    base = HEADER(ptr);
    nm = realloc(base, size + sizeof(Header));

    heap[nm->index] = nm;

    ret = DATA(nm);

    /* Change the range if it was outside it: */
    if (ret < lowest_value)
        lowest_value = ret;
    if (ret > highest_value)
        highest_value = ret;

    return ret;
}

char *GC_strdup(char *s)
{
    int len = strlen(s);
    char *ret = GC_malloc_atomic(len + 1);
    int i;
    for (i = 0; i <= len; i++)
        ret[i] = s[i];

    return ret;
}

#ifdef GC_DEBUG

void *GC_malloc_dbg(size_t size, const char *fn, int line)
{
    void *r = GC_malloc(size);
    HEADER(r)->fn = fn;
    HEADER(r)->line = line;
    gc_debug("MALLOC: ");
    print_header_info(HEADER(r));
    return r;
}

void *GC_malloc_atomic_dbg(size_t size, const char *fn, int line)
{
    void *r = GC_malloc_atomic(size);
    HEADER(r)->fn = fn;
    HEADER(r)->line = line;
    gc_debug("ATOMIC: ");
    print_header_info(HEADER(r));
    return r;
}

void *GC_realloc_dbg(void *ptr, size_t size, const char *fn, int line)
{
    void *r = GC_realloc(ptr, size);
    HEADER(r)->fn = fn;
    HEADER(r)->line = line;
    gc_debug("REALLOCing: %p -> ");
    print_header_info(HEADER(r));
    return r;
}

char *GC_strdup_dbg(char *s, const char *fn, int line)
{
    char *r = GC_strdup(s);
    HEADER(r)->fn = fn;
    HEADER(r)->line = line;
    gc_debug("STRDUP: ");
    print_header_info(HEADER(r));
    return r;
}

#endif /* GC_DEBUG */

