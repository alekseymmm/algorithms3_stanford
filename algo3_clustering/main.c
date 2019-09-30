/*
 * main.c
 *
 *  Created on: 27 сент. 2019 г.
 *      Author: alekseym
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>

#include "graph.h"
#include "union_find.h"

int edge_compare(const void *a, const void *b)
{
	const struct rdx_edge *arg1 = a;
	const struct rdx_edge *arg2 = b;

	return arg1->cost - arg2->cost;
}
static long long __calc_spacing(struct rdx_graph *graph, struct rdx_uf *union_find)
{
	struct rdx_edge *edge;
	size_t i;
	size_t node1, node2, cluster1, cluster2;
	long long spacing = LLONG_MAX;

	for (i = 0; i < graph->num_edges; i++) {
		edge = &graph->edges[i];
		if (edge->cost > spacing)
			continue;
		node1 = edge->node1;
		node2 = edge->node2;

		cluster1 = rdx_uf_find(union_find, node1);
		cluster2 = rdx_uf_find(union_find, node2);
		if (cluster1 != cluster2 &&
		    edge->cost < spacing) {
			spacing = edge->cost;
		}
	}
	return spacing;
}

long long rdx_single_link_clustering(struct rdx_graph *graph, size_t num_clusters)
{
	size_t clusters_cnt = graph->num_nodes;
	struct rdx_uf *union_find;
	struct rdx_edge *cur_edge;
	size_t cur_edge_pos;
	size_t node1, node2, cluster1, cluster2;
	long long spacing;

	union_find = rdx_create_uf(graph->num_nodes);
	if (!union_find)
		return LLONG_MIN;

	qsort(graph->edges, graph->num_edges, sizeof(struct rdx_edge), edge_compare);

	cur_edge_pos = 0;
	while (clusters_cnt > num_clusters &&
	       cur_edge_pos < graph->num_edges) {
		cur_edge = &graph->edges[cur_edge_pos];
		node1 = cur_edge->node1;
		node2 = cur_edge->node2;

		cluster1 = rdx_uf_find(union_find, node1);
		cluster2 = rdx_uf_find(union_find, node2);

		if (cluster1 != cluster2) {
			rdx_uf_union(union_find, node1, node2);
			clusters_cnt--;
		}
		cur_edge_pos++;
	}
	spacing = __calc_spacing(graph, union_find);
	rdx_destroy_uf(union_find);

	return spacing;
}

int main()
{
	FILE *fd;
	size_t num_nodes;
	int i;
	struct rdx_graph *graph;
	long long spacing;

	fd = fopen("clustering1.txt", "r");
	//fd = fopen("test.txt", "r");
	if (!fd) {
		printf("error open file\n");
		return -1;
	}

	fscanf(fd, "%zu\n", &num_nodes);
	graph = rdx_create_graph(0, num_nodes);
	if (!graph) {
		printf("cannot allocate graph\n");
		return -1;
	}

	for (i = 0; i < num_nodes * (num_nodes - 1) / 2; i++) {
		size_t node1_id, node2_id;
		long long cost;
		if (fscanf(fd,
			"%zu %zu %lld\n", &node1_id, &node2_id, &cost) != 3 ) {
				printf("wrong input, line=%d\n", i);
		} else {
			rdx_graph_add_edge(graph,
					   node1_id - 1, node2_id - 1, cost);
		}
	}

	spacing = rdx_single_link_clustering(graph, 4);
	rdx_destroy_graph(graph);
	printf("%lld\n", spacing);
	return 0;
}
