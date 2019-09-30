/*
 * main.c
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: alekseym
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <memory.h>

#include "heap.h"

struct some_obj {
	long long key;
	long long heap_index;
};

#define MAX_ADJACENT_NODES_NUM 100

struct rdx_node {
	size_t id;
	size_t adj_nodes_id[MAX_ADJACENT_NODES_NUM];
	long long  edge_costs[MAX_ADJACENT_NODES_NUM];
	size_t adj_nodes_cnt;
	bool in_mst;
	size_t heap_index;
};

struct rdx_graph {
	struct rdx_node *nodes;
	size_t nodes_cnt;
};

void heap_index_changed(void *obj, long long index)
{
	struct rdx_node *node = obj;
	node->heap_index = index;
}

struct rdx_graph *rdx_create_graph(size_t num_nodes)
{
	struct rdx_graph *graph = NULL;

	graph = calloc(1, sizeof(struct rdx_graph));
	if (!graph) {
		return NULL;
	}

	graph->nodes = calloc(num_nodes, sizeof(struct rdx_node));
	if (!graph->nodes) {
		free(graph);
		return NULL;
	}
	graph->nodes_cnt = 0;

	return graph;
}

void rdx_destroy_graph(struct rdx_graph *graph)
{
	if (!graph)
		return;

	if (graph->nodes)
		free(graph->nodes);
	if (graph)
		free(graph);
}

int rdx_graph_add_node(struct rdx_graph *graph, size_t id)
{
	graph->nodes[graph->nodes_cnt].id = id;
	graph->nodes[graph->nodes_cnt].in_mst = false;
	graph->nodes[graph->nodes_cnt].heap_index = SIZE_MAX;
	graph->nodes[graph->nodes_cnt].adj_nodes_cnt = 0;
	graph->nodes_cnt++;

	return 0;
}

int rdx_graph_add_edge(struct rdx_graph *graph, struct rdx_node *node1,
			struct rdx_node *node2, long long cost)
{
	if (node1->adj_nodes_cnt == MAX_ADJACENT_NODES_NUM ||
	    node2->adj_nodes_cnt == MAX_ADJACENT_NODES_NUM) {
		printf("maximum adj nodes cnt achieved. cannot add node\n");
		return -1;
	}
	node1->adj_nodes_id[node1->adj_nodes_cnt] = node2->id;
	node1->edge_costs[node1->adj_nodes_cnt] = cost;
	node1->adj_nodes_cnt++;

	node2->adj_nodes_id[node2->adj_nodes_cnt] = node1->id;
	node2->edge_costs[node2->adj_nodes_cnt] = cost;
	node2->adj_nodes_cnt++;
	return 0;
}

struct rdx_node *rdx_graph_get_node(struct rdx_graph *graph, size_t id)
{
	return &graph->nodes[id];
}


long long prim_mst(struct rdx_graph *graph)
{
	long long mst_cost = 0;
	size_t mst_nodes_cnt = 0;
	size_t i;
	struct rdx_node *node;
	struct rdx_heap *nodes_min_heap;
	long long min_edge_cost;
	long long cur_cost;
	struct rdx_node *adj_node;

	nodes_min_heap = rdx_heap_create(0, RDX_MIN_HEAP, heap_index_changed);

	node = &graph->nodes[0];
	node->in_mst = true;
	mst_nodes_cnt++;

	for (i = 0; i < node->adj_nodes_cnt; i++) {
		struct rdx_node *adj_node = rdx_graph_get_node(graph,
					node->adj_nodes_id[i]);
		rdx_heap_insert(nodes_min_heap, node->edge_costs[i], adj_node);
		if (rdx_check_heap(nodes_min_heap))
			printf("Heap is corrupt.\n");
	}

	while (mst_nodes_cnt < graph->nodes_cnt) {
		min_edge_cost = rdx_heap_pop(nodes_min_heap, (void *)&node);
		mst_cost += min_edge_cost;
		node->in_mst = true;
		mst_nodes_cnt++;

		for (i = 0; i < node->adj_nodes_cnt; i++) {
			adj_node = rdx_graph_get_node(graph,
						node->adj_nodes_id[i]);
			cur_cost = node->edge_costs[i];

			if (adj_node->in_mst)
				continue;

			//adj_node is not in heap
			if (adj_node->heap_index >= nodes_min_heap->items_cnt) {
				rdx_heap_insert(nodes_min_heap, cur_cost, adj_node);

			} else {
				long long key;
				key = nodes_min_heap->items[adj_node->heap_index].key;

				if (key > cur_cost) {
					rdx_heap_delete(nodes_min_heap,
							adj_node->heap_index);
					rdx_heap_insert(nodes_min_heap,
							cur_cost, adj_node);
				}
			}
		}

	}
	printf("mst cost=%lld\n", mst_cost);
	rdx_heap_destroy(nodes_min_heap);
	return mst_cost;
}

int main()
{
	FILE *fd;
	size_t num_nodes, num_edges;
	int i;
	struct rdx_graph *graph;
	struct rdx_node *node1, *node2;
	long long mst_cost;

	fd = fopen("edges.txt", "r");
	if (!fd) {
		printf("error open file\n");
		return -1;
	}

	fscanf(fd, "%zu %zu\n", &num_nodes, &num_edges);
	graph = rdx_create_graph(num_nodes);
	if (!graph) {
		printf("cannot allocate graph\n");
		return -1;
	}

	for (i = 0; i < num_nodes; i++) {
		rdx_graph_add_node(graph, i);
	}

	for (i = 0; i < num_edges; i++) {
		size_t node1_id, node2_id;
		long long cost;
		if (fscanf(fd,
			"%zu %zu %lld\n", &node1_id, &node2_id, &cost) != 3 ){
				printf("wrong input, line=%d\n", i);
		} else {
			node1 = rdx_graph_get_node(graph, node1_id - 1);
			node2 = rdx_graph_get_node(graph, node2_id - 1);
			rdx_graph_add_edge(graph, node1, node2, cost);
		}

	}
	mst_cost = prim_mst(graph);

	rdx_destroy_graph(graph);
	return 0;
}
//	FILE *fd;
//	int num_verticies, num_edges;
//	int i;
//	unsigned long long res;


//{
//	struct rdx_heap *heap;
//	long long max_value;
//	//void *value = 0xff;
//	long long  keys[10];
//	struct some_obj objs[10];
//	int i;
//
//	heap = rdx_heap_create(0, RDX_MAX_HEAP, &heap_index_changed);
//	for (i = 0; i < 10 ; i++) {
//		//rand in [0, 100]
//		keys[i] = 0 + rand() % (100 - 0 + 1);
//		objs[i].key = keys[i];
//		objs[i].heap_index = -1;
//
//		rdx_heap_insert(heap, objs[i].key, &objs[i]);
//	}
////	rdx_heap_insert(heap, 0, NULL);
////	rdx_heap_insert(heap, 1, NULL);
////	rdx_heap_insert(heap, 2, NULL);
////	rdx_heap_insert(heap, 3, NULL);
////	rdx_heap_insert(heap, 4, NULL);
////	rdx_heap_insert(heap, 5, NULL);
////	rdx_heap_insert(heap, 6, NULL);
//
////	rdx_print_heap(heap);
//	for (i = 0 ; i < 10; i++) {
//		struct some_obj *tmp_obj;
//		max_value = rdx_heap_pop(heap, (void *)&tmp_obj);
//		printf("%lld\n", max_value);
//	}
//
//	//max_value = rdx_extract_max(heap, &value);
//
//	rdx_heap_destroy(heap);
//
//	heap = rdx_heapify(keys, sizeof(long long), 10, keys, RDX_MIN_HEAP);
//	rdx_print_heap(heap);
//	rdx_heap_delete(heap, 2);
//	for (i = 0 ; i < 9; i++) {
//		long long *value;
//		max_value = rdx_heap_pop(heap, (void *)&value);
//		printf("%lld\n", max_value);
//	}
//	printf("done\n");
////
////	fd = fopen("edges.txt", "r");
////	if (!fd) {
////		printf("error open file\n");
////		return -1;
////	}
////
////	fscanf(fd, "%d", &num_jobs);
////
////	jobs = malloc(num_jobs * sizeof(struct job));
////
////	for (i = 0; i < num_jobs; i++) {
////		if (fscanf(fd, "%d %d", &jobs[i].weight, &jobs[i].len) != 2) {
////			printf("error while reading file.\n");
////		}
////	}
////	qsort(jobs, num_jobs, sizeof(struct job), compare_jobs);
////
////	res = calc_completion_time(jobs, num_jobs);
////	printf("%llu\n", res);
////
////	fclose(fd);
////	free(jobs);
//
//	return 0;
//}
