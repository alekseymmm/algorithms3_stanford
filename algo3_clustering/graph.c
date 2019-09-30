/*
 * graph.c
 *
 *  Created on: Sep 29, 2019
 *      Author: alex
 */
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "graph.h"

struct rdx_graph *rdx_create_graph(size_t num_edges, size_t num_nodes)
{
	struct rdx_graph *graph = NULL;

	graph = calloc(1, sizeof(struct rdx_graph));
	if (!graph) {
		return NULL;
	}

	graph->num_nodes = num_nodes;
	if (num_edges < 1) {
		graph->max_num_edges = 1;
		graph->num_edges = 0;
	} else {
		graph->num_edges = num_edges;
		graph->max_num_edges = num_edges;
	}

	graph->edges = calloc(num_edges, sizeof(struct rdx_edge));
	if (!graph->edges) {
		free(graph);
		return NULL;
	}

	return graph;
}

int rdx_graph_add_edge(struct rdx_graph *graph, size_t from, size_t to,
		       long long cost) {
	struct rdx_edge *new_edges = NULL;
	//if it is not enough space then allocate bigger array
	if (graph->num_edges + 1 > graph->max_num_edges) {
		new_edges = calloc(graph->max_num_edges * 2,
				   sizeof(struct rdx_edge));
		if (!new_edges)
			return -ENOMEM;
		memcpy(new_edges, graph->edges,
		       sizeof(struct rdx_edge) * graph->num_edges);
		graph->max_num_edges = graph->max_num_edges * 2;
		if (graph->edges)
			free(graph->edges);
		graph->edges = new_edges;
	}

	graph->edges[graph->num_edges].node1 = from;
	graph->edges[graph->num_edges].node2 = to;
	graph->edges[graph->num_edges].cost = cost;
	graph->num_edges++;

	return 0;
}

void rdx_destroy_graph(struct rdx_graph *graph)
{
	if (!graph)
		return;

	if (graph->edges)
		free(graph->edges);
	if (graph)
		free(graph);
}
