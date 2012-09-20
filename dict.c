
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

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
};

static void insert_at_pos(struct sorted_list *L, struct keyvalue kv, int pos)
{
    int i;
    if (L->len >= L->size) /* >= as len is going to increase by one */
    {
        L->size += 4; /* Leave a bit of room */
        L->list = realloc(L->list, L->size * sizeof(struct keyvalue));
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
    return calloc(1, sizeof(struct dict));
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

void dict_print(struct dict *D)
{
    int i, j;
    for (i = 0; i < N_BUCKETS; i++)
    {
        struct sorted_list *L = D->tbl + i;
        printf("  Bucket %d: (", i);
        for (j = 0; j < L->len; j++)
        {
            printf("\'%s\':%d", L->list[j].key, L->list[j].value);
            if (j < L->len - 1)
                printf(", ");
        }
        printf(")\n");
    }
}

