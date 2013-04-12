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

#ifndef GC_H
#define GC_H

#include <stdlib.h>

void GC_idle(void);
void GC_addroot(void *);
void GC_delroot(void *);

#ifdef GC_DEBUG
    void *GC_malloc_dbg(size_t, const char *, int);
    void *GC_malloc_atomic_dbg(size_t, const char *, int);
    char *GC_strdup_dbg(char *, const char *, int);
    void *GC_realloc_dbg(void *, size_t, const char *, int);
#else
    void *GC_malloc(size_t);
    void *GC_malloc_atomic(size_t);
    char *GC_strdup(char *);
    void *GC_realloc(void *, size_t);
#endif

#ifdef GC_DEBUG
    #define GC_MALLOC(size) GC_malloc_dbg(size, __FUNCTION__, __LINE__)
    #define GC_MALLOC_ATOMIC(size) GC_malloc_atomic_dbg(size, __FUNCTION__, __LINE__)
    #define GC_STRDUP(s) GC_strdup_dbg(s, __FUNCTION__, __LINE__)
    #define GC_REALLOC(ptr, size) GC_realloc_dbg(ptr, size, __FUNCTION__, __LINE__)
#else
    #define GC_MALLOC(size) GC_malloc(size)
    #define GC_MALLOC_ATOMIC(size) GC_malloc_atomic(size)
    #define GC_STRDUP(s) GC_strdup(s)
    #define GC_REALLOC(ptr, size) GC_realloc(ptr, size)
#endif


#endif

