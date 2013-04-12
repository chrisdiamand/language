
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "gc.h"

#define N_BUCKETS_POWER 2
#define N_BUCKETS (1 << N_BUCKETS_POWER)
#define N_BUCKETS_MASK (N_BUCKETS - 1)

struct keyvalue
{
    char                *key;
    dict_value          value;
};

struct sorted_list
{
    int                 len;
    int                 size;
    struct keyvalue     *list;
};

struct dict
{
    struct sorted_list  tbl[N_BUCKETS];
    /* For iterating */
    unsigned int        bucket, pos;
};

static void insert_at_pos(struct sorted_list *L, struct keyvalue kv, int pos)
{
    int i;

    if (L->len >= L->size) /* >= as len is going to increase by one */
    {
        L->size += 4; /* Leave a bit of room */
        L->list = GC_realloc(L->list, L->size * sizeof(struct keyvalue));
        assert(L->list);
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
    dest->list = GC_malloc(sizeof(struct keyvalue) * dest->len);

    memcpy(dest->list, src->list, sizeof(struct keyvalue) * dest->len);
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
    struct keyvalue kv;

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

dict_value *dict_get(struct dict *D, char *key)
{
    unsigned int bucket = hash(key);
    struct sorted_list *L = D->tbl + bucket;
    int pos = search(L, key);

    if (L->list && L->size > 0 && pos < L->len)
    {
        if ( !strcmp(L->list[pos].key, key) )
            return &(L->list[pos].value);
    }
    return NULL;
}

void dict_print(struct dict *D, dict_print_function_t printfunc)
{
    int i, j;
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

/* Set all iterator counters to 0 so iteration can begin */
void dict_begin(struct dict *D)
{
    if (D)
        D->bucket = D->pos = 0;
}

dict_value dict_next(struct dict *D, char **namep)
{
    struct keyvalue *item;
    if  (!D)
        return NULL;

    /* See if it is finished */
    if (D->bucket >= N_BUCKETS - 1
        && D->pos >= D->tbl[N_BUCKETS - 1].len)
        return NULL;

    /* Do we need to move on to the next bucket? */
    if (D->pos >= D->tbl[D->bucket].len)
    {
        D->bucket++;
        D->pos = 0;
    }

    item = D->tbl[D->bucket].list + D->pos;

    D->pos++;

    if (namep)
        *namep = item->key;
    return item->value;
}
