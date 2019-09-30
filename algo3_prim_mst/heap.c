/*
 * heap.c
 *
 *  Created on: 17 сент. 2019 г.
 *      Author: alekseym
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>

#include "heap.h"

struct rdx_heap *rdx_heap_create(size_t num_items, rdx_heap_type_t heap_type,
		void (*index_change_callback)(void *obj, long long index))
{
	struct rdx_heap *heap = NULL;

	heap = calloc(1, sizeof(struct rdx_heap));
	if (!heap)
		return NULL;

	if (num_items < 1)
		num_items = 1;

	heap->items = calloc(num_items, sizeof(struct rdx_heap_item));
	if (!heap->items) {
		free(heap);
		return NULL;
	}
	heap->max_items_cnt = num_items;

	heap->index_change_callback = index_change_callback;
	heap->heap_type = heap_type;

	return heap;
}

void rdx_heap_destroy(struct rdx_heap *heap)
{
	if (!heap)
		return;

	if (heap->items)
		free(heap->items);
	if (heap)
		free(heap);
}

static struct rdx_heap_item *__get_parent(struct rdx_heap *heap,
					  size_t item_pos)
{
	size_t parent_pos;
	struct rdx_heap_item *parent;

	if (item_pos == 0)
		return NULL;

	parent_pos = (item_pos + 1) / 2 - 1;
	parent = &heap->items[parent_pos];

	return parent;
}

static struct rdx_heap_item *__get_child(struct rdx_heap *heap,
					 size_t item_pos, size_t child_num)
{
	size_t child_pos;
	struct rdx_heap_item *child;

	if ((item_pos * 2 + child_num + 1) >= heap->items_cnt)
		return NULL;
	if (child_num > 1)
		return NULL;

	child_pos = item_pos * 2 + 1 + child_num;
	child = &heap->items[child_pos];

	return child;
}

static bool __compare_keys(struct rdx_heap *heap, long long key1, long long key2)
{
	if (heap->heap_type == RDX_MAX_HEAP)
		return key1 > key2;
	else
		return key1 < key2;
}

static void __swap_items(struct rdx_heap *heap, size_t pos1, size_t pos2)
{
	long long tmp_key;
	void *tmp_value;

	tmp_key = heap->items[pos1].key;
	heap->items[pos1].key = heap->items[pos2].key;
	heap->items[pos2].key = tmp_key;

	tmp_value = heap->items[pos1].value;
	heap->items[pos1].value = heap->items[pos2].value;
	heap->items[pos2].value = tmp_value;

	if (heap->index_change_callback) {
		heap->index_change_callback(heap->items[pos1].value, pos1);
		heap->index_change_callback(heap->items[pos2].value, pos2);
	}
}

//restore heap property for subtree rooted at pos
static void heapify_down(struct rdx_heap *heap, size_t pos)
{
	struct rdx_heap_item *cur_item, *child1, *child2, *swap_child;
	cur_item = &heap->items[pos];

	child1 = __get_child(heap, cur_item->pos, 0);
	child2 = __get_child(heap, cur_item->pos, 1);

	while (child1 || child2)
	{
		if (child1 && child2) {
			if (__compare_keys(heap, child1->key, child2->key)) {
				swap_child = child1;
			} else {
				swap_child = child2;
			}
		} else if (child1 && !child2) {
			swap_child = child1;
		} else {
			swap_child = child2;
		}

		if (__compare_keys(heap, swap_child->key, cur_item->key)) {
			__swap_items(heap, cur_item->pos, swap_child->pos);
			cur_item = swap_child;
			child1 = __get_child(heap, cur_item->pos, 0);
			child2 = __get_child(heap, cur_item->pos, 1);
		} else {
			break;
		}

	}
}

//restore heap property up from pos
static void heapify_up(struct rdx_heap *heap, size_t pos)
{
	struct rdx_heap_item *cur_item, *parent;

	cur_item = &heap->items[pos];
	parent = __get_parent(heap, cur_item->pos);

	while (parent && __compare_keys(heap, cur_item->key, parent->key)) {
		__swap_items(heap, cur_item->pos, parent->pos);
		cur_item = parent;
		parent = __get_parent(heap, cur_item->pos);
	}
}

int rdx_heap_insert(struct rdx_heap *heap, long long key, void *value)
{
	struct rdx_heap_item *new_items;
	size_t items_cnt = heap->items_cnt;

	//if it is not enough space then allcoate bigger array
	if (heap->items_cnt + 1 > heap->max_items_cnt) {
		new_items = calloc(heap->max_items_cnt * 2,
				   sizeof(struct rdx_heap_item));
		if (!new_items)
			return -ENOMEM;
		memcpy(new_items, heap->items,
		       sizeof(struct rdx_heap_item) * heap->items_cnt);
		heap->max_items_cnt = heap->max_items_cnt * 2;
		if (heap->items)
			free(heap->items);
		heap->items = new_items;
	}

	heap->items[items_cnt].key = key;
	heap->items[items_cnt].value = value;
	heap->items[items_cnt].pos = heap->items_cnt;
	if (heap->index_change_callback) {
		heap->index_change_callback(heap->items[items_cnt].value,
				            items_cnt);
	}

	heapify_up(heap, heap->items_cnt);
	heap->items_cnt++;

	return 0;
}

long long rdx_heap_pop(struct rdx_heap *heap, void **value)
{
	long long key;

	if (heap->items_cnt == 0) {
		if (value)
			*value = NULL;
		return LONG_MIN;
	}

	key = heap->items[0].key;
	if (value)
		*value = heap->items[0].value;

	__swap_items(heap, 0, heap->items_cnt - 1);
	heap->items_cnt--;

	heapify_down(heap, 0);

	return key;
}

void rdx_heap_delete(struct rdx_heap *heap, size_t index)
{
	struct rdx_heap_item *cur_item, *parent;

	__swap_items(heap, index, heap->items_cnt - 1);
	heap->items_cnt--;

	//sift up
	cur_item = &heap->items[index];
	parent = __get_parent(heap, cur_item->pos);
	while (parent && __compare_keys(heap, cur_item->key, parent->key)) {
		__swap_items(heap, cur_item->pos, parent->pos);
		cur_item = parent;
		parent = __get_parent(heap, cur_item->pos);
	}
	//sift_down
	heapify_down(heap, index);
}

void rdx_print_heap(struct rdx_heap *heap)
{
	size_t i, j;
	j = 1;
	for (i = 0; i < heap->items_cnt; i++) {
		if (i == j) {
			printf("\n");
			j = j * 2 + 1;
		}
		printf("(%llu,%p)", heap->items[i].key, heap->items[i].value);
	}
	printf("\n");
}

struct rdx_heap *rdx_heapify(void *objs, size_t obj_size,
			     size_t obj_cnt, long long *keys,
			     rdx_heap_type_t heap_type)
{
	struct rdx_heap *heap;
	long long   i;

	heap = rdx_heap_create(obj_cnt, heap_type, NULL);
	if (!heap)
		return NULL;

	for (i = 0; i < obj_cnt; i++) {
		heap->items[i].key = keys[i];
		heap->items[i].pos = i;
		heap->items[i].value = objs + obj_size * i;
		heap->items_cnt++;
	}

	for (i = heap->items_cnt / 2; i >= 0; i--) {
		heapify_down(heap, i);
	}
	return heap;
}


int rdx_check_heap(struct rdx_heap *heap)
{
	long long  i;
	struct rdx_heap_item *item, *parent;

	for (i = heap->items_cnt / 2; i >= 0; i--) {
		item = &heap->items[i];
		parent = __get_parent(heap, item->pos);
		if (parent) {
			if (parent->key > item->key)
				return -1;
		}


	}
	return 0;
}
