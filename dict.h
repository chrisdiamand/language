
#ifndef DICT_H
#define DICT_H

typedef int dict_value;

struct dict;

struct dict *dict_new(void);
dict_value *dict_get(struct dict *, char *);
void dict_set(struct dict *, char *, dict_value);
void dict_print(struct dict *);

#endif

