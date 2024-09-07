// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>

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

#ifndef SV_H_
#define SV_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef SVDEF
#define SVDEF
#endif // SVDEF

typedef struct {
    size_t count;
    const char *data;
} String_View;

#define SV(cstr_lit) sv_from_parts(cstr_lit, sizeof(cstr_lit) - 1)
#define SV_STATIC(cstr_lit)   \
    {                         \
        sizeof(cstr_lit) - 1, \
        (cstr_lit)            \
    }

#define SV_NULL sv_from_parts(NULL, 0)

// printf macros for String_View
#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int) (sv).count, (sv).data
// USAGE:
//   String_View name = ...;
//   printf("Name: "SV_Fmt"\n", SV_Arg(name));

SVDEF String_View sv_from_parts(const char *data, size_t count);
SVDEF String_View sv_from_cstr(const char *cstr);
SVDEF String_View sv_trim_left(String_View sv);
SVDEF String_View sv_trim_right(String_View sv);
SVDEF String_View sv_trim(String_View sv);
SVDEF String_View sv_take_left_while(String_View sv, bool (*predicate)(char x));
SVDEF String_View sv_chop_by_delim(String_View *sv, char delim);
SVDEF String_View sv_chop_by_sv(String_View *sv, String_View thicc_delim);
SVDEF bool sv_try_chop_by_delim(String_View *sv, char delim, String_View *chunk);
SVDEF String_View sv_chop_left(String_View *sv, size_t n);
SVDEF String_View sv_chop_right(String_View *sv, size_t n);
SVDEF String_View sv_chop_left_while(String_View *sv, bool (*predicate)(char x));
SVDEF bool sv_index_of(String_View sv, char c, size_t *index);
SVDEF bool sv_eq(String_View a, String_View b);
SVDEF bool sv_eq_ignorecase(String_View a, String_View b);
SVDEF bool sv_starts_with(String_View sv, String_View prefix);
SVDEF bool sv_ends_with(String_View sv, String_View suffix);
SVDEF uint64_t sv_to_u64(String_View sv);

#endif  // SV_H_

#ifdef SV_IMPLEMENTATION

SVDEF String_View sv_from_parts(const char *data, size_t count)
{
    String_View sv;
    sv.count = count;
    sv.data = data;
    return sv;
}

SVDEF String_View sv_from_cstr(const char *cstr)
{
    return sv_from_parts(cstr, strlen(cstr));
}

SVDEF String_View sv_trim_left(String_View sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[i])) {
        i += 1;
    }

    return sv_from_parts(sv.data + i, sv.count - i);
}

SVDEF String_View sv_trim_right(String_View sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) {
        i += 1;
    }

    return sv_from_parts(sv.data, sv.count - i);
}

SVDEF String_View sv_trim(String_View sv)
{
    return sv_trim_right(sv_trim_left(sv));
}

SVDEF String_View sv_chop_left(String_View *sv, size_t n)
{
    if (n > sv->count) {
        n = sv->count;
    }

    String_View result = sv_from_parts(sv->data, n);

    sv->data  += n;
    sv->count -= n;

    return result;
}

SVDEF String_View sv_chop_right(String_View *sv, size_t n)
{
    if (n > sv->count) {
        n = sv->count;
    }

    String_View result = sv_from_parts(sv->data + sv->count - n, n);

    sv->count -= n;

    return result;
}

SVDEF bool sv_index_of(String_View sv, char c, size_t *index)
{
    size_t i = 0;
    while (i < sv.count && sv.data[i] != c) {
        i += 1;
    }

    if (i < sv.count) {
        if (index) {
            *index = i;
        }
        return true;
    } else {
        return false;
    }
}

SVDEF bool sv_try_chop_by_delim(String_View *sv, char delim, String_View *chunk)
{
    size_t i = 0;
    while (i < sv->count && sv->data[i] != delim) {
        i += 1;
    }

    String_View result = sv_from_parts(sv->data, i);

    if (i < sv->count) {
        sv->count -= i + 1;
        sv->data  += i + 1;
        if (chunk) {
            *chunk = result;
        }
        return true;
    }

    return false;
}

SVDEF String_View sv_chop_by_delim(String_View *sv, char delim)
{
    size_t i = 0;
    while (i < sv->count && sv->data[i] != delim) {
        i += 1;
    }

    String_View result = sv_from_parts(sv->data, i);

    if (i < sv->count) {
        sv->count -= i + 1;
        sv->data  += i + 1;
    } else {
        sv->count -= i;
        sv->data  += i;
    }

    return result;
}

SVDEF String_View sv_chop_by_sv(String_View *sv, String_View thicc_delim)
{
    String_View window = sv_from_parts(sv->data, thicc_delim.count);
    size_t i = 0;
    while (i + thicc_delim.count < sv->count 
        && !(sv_eq(window, thicc_delim))) 
    {
        i++;
        window.data++;
    }

    String_View result = sv_from_parts(sv->data, i);

    if (i + thicc_delim.count == sv->count) {
        // include last <thicc_delim.count> characters if they aren't 
        //  equal to thicc_delim
        result.count += thicc_delim.count; 
    }
    
    // Chop!
    sv->data  += i + thicc_delim.count;
    sv->count -= i + thicc_delim.count;

    return result;
}

SVDEF bool sv_starts_with(String_View sv, String_View expected_prefix)
{
    if (expected_prefix.count <= sv.count) {
        String_View actual_prefix = sv_from_parts(sv.data, expected_prefix.count);
        return sv_eq(expected_prefix, actual_prefix);
    }

    return false;
}

SVDEF bool sv_ends_with(String_View sv, String_View expected_suffix)
{
    if (expected_suffix.count <= sv.count) {
        String_View actual_suffix = sv_from_parts(sv.data + sv.count - expected_suffix.count, expected_suffix.count);
        return sv_eq(expected_suffix, actual_suffix);
    }

    return false;
}

SVDEF bool sv_eq(String_View a, String_View b)
{
    if (a.count != b.count) {
        return false;
    } else {
        return memcmp(a.data, b.data, a.count) == 0;
    }
}

SVDEF bool sv_eq_ignorecase(String_View a, String_View b)
{
    if (a.count != b.count) {
        return false;
    }
    
    char x, y;
    for (size_t i = 0; i < a.count; i++) {
        x = 'A' <= a.data[i] && a.data[i] <= 'Z'
              ? a.data[i] + 32
              : a.data[i];
        
        y = 'A' <= b.data[i] && b.data[i] <= 'Z'
              ? b.data[i] + 32
              : b.data[i];

        if (x != y) return false;
    } 
    return true;
}

SVDEF uint64_t sv_to_u64(String_View sv)
{
    uint64_t result = 0;

    for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); ++i) {
        result = result * 10 + (uint64_t) sv.data[i] - '0';
    }

    return result;
}

uint64_t sv_chop_u64(String_View *sv)
{
    uint64_t result = 0;
    while (sv->count > 0 && isdigit(*sv->data)) {
        result = result*10 + *sv->data - '0';
        sv->count -= 1;
        sv->data += 1;
    }
    return result;
}

SVDEF String_View sv_chop_left_while(String_View *sv, bool (*predicate)(char x))
{
    size_t i = 0;
    while (i < sv->count && predicate(sv->data[i])) {
        i += 1;
    }
    return sv_chop_left(sv, i);
}

SVDEF String_View sv_take_left_while(String_View sv, bool (*predicate)(char x))
{
    size_t i = 0;
    while (i < sv.count && predicate(sv.data[i])) {
        i += 1;
    }
    return sv_from_parts(sv.data, i);
}

#endif // SV_IMPLEMENTATION
