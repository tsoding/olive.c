// Copyright 2022 Alexey Kutepov <reximkut@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef ARENA_H_
#define ARENA_H_

#include <stddef.h>
#include <stdint.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif

#define ARENA_BACKEND_LIBC_MALLOC 0
#define ARENA_BACKEND_LINUX_MMAP 1
#define ARENA_BACKEND_WIN32_VIRTUALALLOC 2
#define ARENA_BACKEND_WASM_HEAPBASE 3

#ifndef ARENA_BACKEND
#define ARENA_BACKEND ARENA_BACKEND_LIBC_MALLOC
#endif // ARENA_BACKEND

typedef struct Region Region;

struct Region {
    Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
};

typedef struct {
    Region *begin, *end;
} Arena;

#define REGION_DEFAULT_CAPACITY (8*1024)

Region *new_region(size_t capacity);
void free_region(Region *r);

// TODO: snapshot/rewind capability for the arena
// - Snapshot should be combination of a->end and a->end->count. 
// - Rewinding should be restoring a->end and a->end->count from the snapshot and
// setting count-s of all the Region-s after the remembered a->end to 0.
void *arena_alloc(Arena *a, size_t size_bytes);
void arena_reset(Arena *a);
void arena_free(Arena *a);

#endif // ARENA_H_

#ifdef ARENA_IMPLEMENTATION

#if ARENA_BACKEND == ARENA_BACKEND_LIBC_MALLOC
#include <stdlib.h>

// TODO: instead of accepting specific capacity new_region() should accept the size of the object we want to fit into the region
// It should be up to new_region() to decide the actual capacity to allocate
Region *new_region(size_t capacity)
{
    size_t size_bytes = sizeof(Region) + sizeof(uintptr_t)*capacity;
    // TODO: it would be nice if we could guarantee that the regions are allocated by ARENA_BACKEND_LIBC_MALLOC are page aligned
    Region *r = malloc(size_bytes);
    ARENA_ASSERT(r);
    r->next = NULL;
    r->count = 0;
    r->capacity = capacity;
    return r;
}

void free_region(Region *r)
{
    free(r);
}
#elif ARENA_BACKEND == ARENA_BACKEND_LINUX_MMAP
#  error "TODO: Linux mmap backend is not implemented yet"
#elif ARENA_BACKEND == ARENA_BACKEND_WIN32_VIRTUALALLOC
#  error "TODO: Win32 VirtualAlloc backend is not implemented yet"
#elif ARENA_BACKEND == ARENA_BACKEND_WASM_HEAPBASE
#  error "TODO: WASM __heap_base backend is not implemented yet"
#else
#  error "Unknown Arena backend"
#endif

// TODO: add debug statistic collection mode for arena
// Should collect things like:
// - How many times new_region was called
// - How many times existing region was skipped
// - How many times allocation exceeded REGION_DEFAULT_CAPACITY

void *arena_alloc(Arena *a, size_t size_bytes)
{
    size_t size = (size_bytes + sizeof(uintptr_t) - 1)/sizeof(uintptr_t);

    if (a->end == NULL) {
        ARENA_ASSERT(a->begin == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end = new_region(capacity);
        a->begin = a->end;
    }

    while (a->end->count + size > a->end->capacity && a->end->next != NULL) {
        a->end = a->end->next;
    }

    if (a->end->count + size > a->end->capacity) {
        ARENA_ASSERT(a->end->next == NULL);
        size_t capacity = REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end->next = new_region(capacity);
        a->end = a->end->next;
    }

    void *result = &a->end->data[a->end->count];
    a->end->count += size;
    return result;
}

void arena_reset(Arena *a)
{
    for (Region *r = a->begin; r != NULL; r = r->next) {
        r->count = 0;
    }

    a->end = a->begin;
}

void arena_free(Arena *a)
{
    Region *r = a->begin;
    while (r) {
        Region *r0 = r;
        r = r->next;
        free_region(r0);
    }
    a->begin = NULL;
    a->end = NULL;
}

#endif // ARENA_IMPLEMENTATION
