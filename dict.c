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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "gc.h"

#define N_BUCKETS_POWER 2
#define N_BUCKETS (1 << N_BUCKETS_POWER)
#define N_BUCKETS_MASK (N_BUCKETS - 1)

struct sorted_list
{
    int                 len;
    int                 size;
    struct dict_pair     *list;
};

struct dict
{
    struct sorted_list  tbl[N_BUCKETS];
    /* For iterating */
    unsigned int        bucket, pos;
};

static void insert_at_pos(struct sorted_list *L, struct dict_pair kv, int pos)
{
    int i;

    if (L->len >= L->size) /* >= as len is going to increase by one */
    {
        L->size += 4; /* Leave a bit of room */
        L->list = GC_realloc(L->list, L->size * sizeof(struct dict_pair));
        assert(L->list != NULL);
    }

    /* Move everything up one position */
    for (i = L->len - 1; i >= pos; i--)
        L->list[i + 1] = L->list[i];

    L->list[pos] = kv;
    L->len++;
}

static unsigned int hash(char *key)
{
    unsigned int n = 0;
    assert(key != NULL);
    while (*key)
        n += *(key++);

    return n & N_BUCKETS_MASK; /* = n % N_BUCKETS but quicker */
}

/* Binary search through the list. If it's not found
 * then return the position it should go in, i.e. searching 
 * for 'c' in 'abdef' should return 2. Obviously if it
 * is found, return its index. */
static int search(struct sorted_list *L, char *key)
{
    size_t min = 0, max = L->len, midpoint = 0;
    int res = 0;

    while (min < max)
    {
        /* Mean average of min and max */
        midpoint = min + ( (max - min) >> 1 );

        res = strcmp(key, L->list[midpoint].key);

        /* If it should go before the midpoint, decrease the max index */
        if (res < 0)
            max = midpoint;
        /* It should be after the midpoint so increase the minimum index */
        else if (res > 0)
            min = midpoint + 1;
        else
            break;
    }

    /* If we didn't find it and it should go later in the list,
     * increase the returned index */
    if (res > 0)
        midpoint++;
    
    return midpoint;
}

struct dict *dict_new(void)
{
    /* Assume the memory is cleared */
    return GC_malloc(sizeof(struct dict));
}

static void copy_sorted_list(struct sorted_list *dest, struct sorted_list *src)
{
    printf("***COPY***\n");
    dest->size = dest->len = src->len;
    dest->list = GC_malloc(sizeof(struct dict_pair) * dest->len);

    memcpy(dest->list, src->list, sizeof(struct dict_pair) * dest->len);
}

/* Copy the elements into a new dictionary */
struct dict *dict_copy(struct dict *D)
{
    int i;
    struct dict *ret = dict_new();
    for (i = 0; i < N_BUCKETS; i++)
    {
        struct sorted_list *src = D->tbl + i;
        struct sorted_list *dst = ret->tbl + i;
        copy_sorted_list(dst, src);
    }
    return ret;
}

void dict_set(struct dict *D, char *key, dict_value val)
{
    struct sorted_list *L = D->tbl + hash(key);
    int list_pos = search(L, key);
    struct dict_pair kv;

    if (L->list && L->size > 0 && list_pos < L->len)
    {
        if ( !strcmp(L->list[list_pos].key, key) )
        {
            L->list[list_pos].value = val;
            return;
        }
    }

    kv.key = key;
    kv.value = val;
    insert_at_pos(L, kv, list_pos);
}

dict_value dict_get(struct dict *D, char *key)
{
    unsigned int bucket = hash(key);
    struct sorted_list *L = D->tbl + bucket;
    int pos = search(L, key);

    if (L->list && L->size > 0 && pos < L->len)
    {
        if ( !strcmp(L->list[pos].key, key) )
            return L->list[pos].value;
    }
    return NULL;
}

void dict_print(struct dict *D, dict_print_function_t printfunc)
{
    int i, j;
    assert(D != NULL);
    for (i = 0; i < N_BUCKETS; i++)
    {
        struct sorted_list *L = D->tbl + i;
        printf("  Bucket %d: (", i);
        for (j = 0; j < L->len; j++)
        {
            printf("\'%s\':", L->list[j].key);
            if (printfunc)
                printfunc(L->list[j].value);
            else
                printf("%p", (void *)(L->list[j].value));
            if (j < L->len - 1)
                printf(", ");
        }
        printf(")\n");
    }
}

struct dict_pair *dict_next(struct dict *D)
{
    assert(D);

    /* Go through the buckets until will find one with
     * something in it (which could be the current one) */
    while (D->bucket < N_BUCKETS)
    {
        if (D->pos < D->tbl[D->bucket].len)
            return D->tbl[D->bucket].list + (D->pos++);
        D->bucket++;
        D->pos = 0;
    }

    return NULL;
}

/* Set all iterator counters to 0 so iteration can begin */
struct dict_pair *dict_begin(struct dict *D)
{
    assert(D);
    D->bucket = D->pos = 0;
    return dict_next(D);
}
