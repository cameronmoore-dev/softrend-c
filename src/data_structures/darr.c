#include "darr.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct darr
{
    u32 count;
    u32 capacity;
    u32 tsize;
    void *data;
} darr_;

void *_darr_get_address(darr_ *arr, u32 index);
void  _darr_realloc(darr_ *arr, u32 new_size);
void  _darr_resize_up(darr_ *arr, u32 new_size);
void  _darr_resize_down(darr_ *arr, u32 new_size);

darr_ *darr_create(u32 tsize, u32 new_size)
{
    darr_ *arr = (darr_ *)malloc(sizeof(darr_));
    arr->count = new_size;
    arr->capacity = (new_size > 0) ? new_size * 2 : 1;
    arr->tsize = tsize;
    arr->data = calloc(1, tsize * arr->capacity);

    assert(arr && "Darr -- Allocation Error");
    assert(arr->data && "Darr->Data -- Allocation Error");
    return arr;
}

void darr_close(darr_ *arr)
{
    free(arr->data);
    arr->data = NULL;
    free(arr);
    arr = NULL;
}

void darr_append(darr_ *arr, void *elem)
{
    if (arr->count == arr->capacity)
    {
        _darr_realloc(arr, arr->count + 1);
    }

    void *addr = _darr_get_address(arr, arr->count);
    memcpy(addr, elem, arr->tsize);
    arr->count++;
}

void darr_remove_back(darr_ *arr)
{
    void *addr = _darr_get_address(arr, arr->count);
    memset(addr, 0, arr->tsize);
    arr->count--;
}

void darr_remove_at(darr_ *arr, u32 index)
{
    if (index == (arr->count - 1))
    {
        darr_remove_back(arr);
        return;
    }

    u32 remaining_elements = arr->count - index;
    
    // Address of element to remove
    void *remove_addr = _darr_get_address(arr, index);

    // Address of the next element up from the one being removed
    void *remain_addr = _darr_get_address(arr, index + 1);

    // Keeps array packed by pulling each remaining element back by 1 index
    memcpy(remove_addr, remain_addr, (arr->tsize * remaining_elements));

    // Keep the count accurate
    darr_remove_back(arr);
}

void darr_clear(darr_ *arr)
{
    memset(arr->data, 0, (arr->tsize * arr->count));
    arr->count = 0;
}

void darr_replace(darr_ *arr, u32 index, void *elem)
{
    void *addr = _darr_get_address(arr, index);
    memcpy(addr, elem, arr->tsize);
}

void darr_resize(darr_ *arr, u32 new_size)
{
    if (new_size == arr->count)
        return;

    (new_size > arr->count)
        ? _darr_resize_up(arr, new_size)
        : _darr_resize_down(arr, new_size);
}

void darr_reserve(darr_ *arr, u32 new_capacity)
{
    if (new_capacity <= arr->capacity)
        return;

    void *tmp = realloc(arr->data, new_capacity);
    if (tmp)
    {
        arr->data = tmp;
    }
}

void darr_free_excess(darr_ *arr)
{
    assert(arr->capacity > 0);
    assert(arr->capacity >= arr->count);

    arr->capacity = arr->count;
    void *tmp = realloc(arr->data, (arr->tsize * arr->capacity));
    if (tmp)
    {
        arr->data = tmp;
    }
}

void darr_swap(darr_ *arr, u32 a_index, u32 b_index)
{
    void *a = _darr_get_address(arr, a_index);
    void *b = _darr_get_address(arr, b_index);

    void *tmp = malloc(arr->tsize);
    memcpy(tmp, a, arr->tsize);
    memcpy(a, b, arr->tsize);
    memcpy(b, tmp, arr->tsize);
    free(tmp);
}

void *darr_at(darr_ *arr, u32 index)
{
    return _darr_get_address(arr, index);
}

void *darr_get_last(darr_ *arr)
{
    return _darr_get_address(arr, arr->count - 1);
}

void darr_copy(darr_ *dest, darr_ *src, bool match)
{
    if (match)
    {
        dest->count = src->count;
        dest->capacity = src->capacity;
        void *tmp = realloc(dest->data, dest->capacity);
        if (tmp)
        {
            dest->data = tmp;
        }
    }

    memcpy(dest->data, src->data, (src->tsize * dest->count));
}

bool darr_is_empty(darr_ *arr)
{
    return arr->count == 0;
}

u32 darr_count(darr_ *arr)
{
    return arr->count;
}

u32 darr_capacity(darr_ *arr)
{
    return arr->capacity;
}

void *_darr_get_address(darr_ *arr, u32 index)
{
    assert(arr->count >= index && "Trying to access out of bounds index!");
    return (void *)((char *)arr->data + (arr->tsize * index));
}

void _darr_realloc(darr_ *arr, u32 new_size)
{
    arr->capacity = new_size * 2;
    void *tmp = realloc(arr->data, (arr->tsize * arr->capacity));
    if (tmp)
    {
        arr->data = tmp;
    }
}

void _darr_resize_up(darr_ *arr, u32 new_size)
{
    if (new_size > arr->capacity)
    {
        _darr_realloc(arr, new_size);
    }

    void *start_added_addr = _darr_get_address(arr, arr->count + 1);
    arr->count = new_size;
    memset(start_added_addr, 0, (arr->tsize * arr->count));
}

void _darr_resize_down(darr_ *arr, u32 new_size)
{
    u32 to_remove = arr->count - new_size;
    void *new_end_addr = (void *)((char *)arr->data + (arr->tsize * (new_size + 1)));
    memset(new_end_addr, 0, (arr->tsize * to_remove));
    arr->count = new_size;
}