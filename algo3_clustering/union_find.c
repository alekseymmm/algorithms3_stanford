/*
 * union_find.c
 *
 *  Created on: 27 сент. 2019 г.
 *      Author: alekseym
 */

#include <stdlib.h>
#include <memory.h>

#include "union_find.h"

struct rdx_uf *rdx_create_uf(size_t num_objects)
{
	size_t i;
	struct rdx_uf *union_find;

	union_find = calloc(1, sizeof(struct rdx_uf));
	if (!union_find)
		return NULL;

	union_find->entries = calloc(num_objects,
				     sizeof(struct rdx_uf_entry));
	if (!union_find->entries) {
		free(union_find);
		return NULL;
	}

	for (i = 0; i < num_objects; i++) {
		union_find->entries[i].parent = i;
		union_find->entries[i].rank = 0;
	}
	union_find->num_entries = num_objects;

	return union_find;
}

void rdx_destroy_uf(struct rdx_uf *union_find)
{
	if (!union_find)
		return;
	if (union_find->entries)
		free(union_find->entries);
	if (union_find)
		free(union_find);
}

size_t rdx_uf_find(struct rdx_uf *union_find, size_t index)
{
	size_t cur_index = index;
	size_t parent = union_find->entries[cur_index].parent;
	size_t cluster;

	while (parent != cur_index) {
		cur_index = parent;
		parent = union_find->entries[cur_index].parent;
	}
	cluster = parent;

	// path compression
	if (cluster != union_find->entries[index].parent) {
		cur_index = index;
		parent = union_find->entries[cur_index].parent;
		union_find->entries[cur_index].parent = cluster;

		while (parent != cur_index) {
			cur_index = parent;
			parent = union_find->entries[cur_index].parent;
			union_find->entries[cur_index].parent = cluster;
		}
	}
	return cluster;
}

void rdx_uf_union(struct rdx_uf *union_find, size_t index_1 , size_t index_2) {
	size_t parent_1, parent_2;
	size_t rank_1, rank_2;

	parent_1 = rdx_uf_find(union_find, index_1);
	parent_2 = rdx_uf_find(union_find, index_2);

	rank_1 = union_find->entries[parent_1].rank;
	rank_2 = union_find->entries[parent_2].rank;

	if (rank_1 > rank_2) {
		union_find->entries[parent_2].parent = parent_1;
	} else if (rank_1 < rank_2) {
		union_find->entries[parent_1].parent = parent_2;
	} else  {// rank_1 == rank_2
		union_find->entries[parent_2].parent = parent_1;
		union_find->entries[parent_1].rank++;
	}
}


