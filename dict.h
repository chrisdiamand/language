
#ifndef DICT_H
#define DICT_H

typedef void *dict_value;

struct dict;

typedef void (*dict_print_function_t)(dict_value);

struct dict *dict_new(void);
struct dict *dict_copy(struct dict *);
dict_value *dict_get(struct dict *, char *);
void dict_set(struct dict *, char *, dict_value);
void dict_print(struct dict *, dict_print_function_t);

/* For iterating over a dictionary */
void dict_begin(struct dict *);
dict_value dict_next(struct dict *, char **);

#endif

