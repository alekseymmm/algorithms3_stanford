/*
 * heap.h
 *
 *  Created on: 17 сент. 2019 г.
 *      Author: alekseym
 */

#ifndef HEAP_H_
#define HEAP_H_

typedef void rdx_index_change_callback(void *obj, long long index);

typedef enum {
	RDX_MIN_HEAP = 0,
	RDX_MAX_HEAP,
} rdx_heap_type_t;

struct rdx_heap_item {
	long long key;
	void *value;
	size_t pos;
};

struct rdx_heap {
	struct rdx_heap_item *items;
	size_t items_cnt;
	size_t max_items_cnt;
	void (*index_change_callback)(void *obj, long long index);
	rdx_heap_type_t heap_type;
};

struct rdx_heap *rdx_heap_create(size_t num_items, rdx_heap_type_t heap_type,
		void (*index_change_callback)(void *obj, long long index));
void rdx_heap_destroy(struct rdx_heap *heap);
int rdx_heap_insert(struct rdx_heap *heap, long long key, void *value);
long long rdx_heap_pop(struct rdx_heap *heap, void **value);
struct rdx_heap *rdx_heapify(void *objs, size_t obj_size,
			     size_t obj_cnt, long long *keys,
			     rdx_heap_type_t heap_type);
void rdx_heap_delete(struct rdx_heap *heap, size_t index);
void rdx_print_heap(struct rdx_heap *heap);
int rdx_check_heap(struct rdx_heap *heap);
#endif /* HEAP_H_ */
