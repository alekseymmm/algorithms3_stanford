/*
 * graph.h
 *
 *  Created on: Sep 29, 2019
 *      Author: alex
 */

#ifndef GRAPH_H_
#define GRAPH_H_

struct rdx_edge {
	size_t node1;
	size_t node2;
	long long cost;
};

struct rdx_graph {
	struct rdx_edge *edges;
	size_t num_edges;
	size_t num_nodes;
	size_t max_num_edges;
};

struct rdx_graph *rdx_create_graph(size_t num_edges, size_t num_nodes);
void rdx_destroy_graph(struct rdx_graph *graph);
int rdx_graph_add_edge(struct rdx_graph *graph, size_t from, size_t to,
		       long long cost);

#endif /* GRAPH_H_ */
