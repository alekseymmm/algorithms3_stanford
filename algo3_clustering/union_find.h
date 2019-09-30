/*
 * union_find.h
 *
 *  Created on: 27 сент. 2019 г.
 *      Author: alekseym
 */

#ifndef UNION_FIND_H_
#define UNION_FIND_H_

struct rdx_uf_entry {
	size_t parent;
	size_t rank;
};

struct rdx_uf {
	struct rdx_uf_entry *entries;
	size_t num_entries;
};

struct rdx_uf *rdx_create_uf(size_t num_objects);
void rdx_destroy_uf(struct rdx_uf *union_find);
size_t rdx_uf_find(struct rdx_uf *union_find, size_t index);
void rdx_uf_union(struct rdx_uf *union_find, size_t index_1 , size_t index_2);

#endif /* UNION_FIND_H_ */
