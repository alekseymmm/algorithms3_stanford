#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>


#include "heap.h"

struct code_word {
	unsigned long long max_len;
	unsigned long long min_len;
	unsigned long long weight;
};

int main()
{
	unsigned long long num_symbols, i, weight;
	FILE *fd;
	struct code_word *cw, *word1, *word2;
	struct rdx_heap *cw_heap;

	fd = fopen("huffman.txt", "r");
	if (!fd) {
		printf("error open file\n");
		return -1;
	}

	fscanf(fd, "%llu\n", &num_symbols);
	cw = calloc(num_symbols, sizeof(struct code_word));
	if (!cw) {
		printf("cannot allocate code_dords array\n");
		goto out;
	}
	cw_heap = rdx_heap_create(num_symbols, RDX_MIN_HEAP, NULL);
	if (!cw_heap) {
		printf("cannot allocate heap\n");
		goto out;
	}

	for (i = 0; i < num_symbols; i++) {
		int rc;
		fscanf(fd, "%llu\n", &weight);
		cw[i].max_len = 0;
		cw[i].min_len = 0;
		cw[i].weight = weight;
		rc = rdx_heap_insert(cw_heap, cw[i].weight, &cw[i]);
		if (rc) {
			printf("failed to add weight=%llu to heap\n",
				cw[i].weight);
			goto out;
		}
	}

	while (cw_heap->items_cnt > 1) {
		rdx_heap_pop(cw_heap, (void **)&word1);
		rdx_heap_pop(cw_heap, (void **)&word2);
		word1->max_len = MAX(word1->max_len, word2->max_len) + 1;
		word1->min_len = MIN(word1->min_len, word2->min_len) + 1;
		word1->weight += word2->weight;
		rdx_heap_insert(cw_heap, word1->weight, word1);
	}

out :
	rdx_heap_pop(cw_heap, (void **)&word1);
	printf("max codeword len=%llu\n", word1->max_len);
	printf("min codeword len=%llu\n", word1->min_len);
	free(cw);
	fclose(fd);
	return 0;
}
