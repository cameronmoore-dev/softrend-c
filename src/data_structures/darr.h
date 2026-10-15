#pragma once

#include "typedefs.h"

typedef struct darr darr_;

darr_ *darr_create(u32 tsize, u32 capacity);
void darr_close(darr_ *arr);
void darr_append(darr_ *arr, void *elem);
void darr_remove_back(darr_ *arr);
void darr_remove_at(darr_ *arr, u32 index);
void darr_clear(darr_ *arr);
void darr_replace(darr_ *arr, u32 index, void *elem);
void darr_resize(darr_ *arr, u32 new_size);
void darr_reserve(darr_ *arr, u32 new_capacity);
void darr_free_excess(darr_ *arr);
void darr_swap(darr_ *arr, u32 a_index, u32 b_index);
void *darr_at(darr_ *arr, u32 index);
void *darr_get_last(darr_ *arr);

void darr_copy(darr_ *dest, darr_ *src, bool match);

bool darr_is_empty(darr_ *arr);
u32 darr_count(darr_ *arr);
u32 darr_capacity(darr_ *arr);