#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/param.h>
#include <stdbool.h>

bool is_weight_in_mwis(unsigned long long *mwis, unsigned long long mwis_cnt,
			unsigned long long weight)
{
	int i;
	for (i = 0; i < mwis_cnt; i++) {
		if (mwis[i] == weight)
			return true;
	}
	return false;
}

int main()
{
	unsigned long long num_vertices;
	long long i;
	FILE *fd;
	unsigned long long *weights;
	unsigned long long *opt;
	unsigned long long *mwis;
	unsigned long long mwis_cnt = 0;
	unsigned long long v1, v2;
	int ret = 0;
	unsigned long idx[8] = {0, 1, 2 ,3, 16, 116, 516, 996};

	fd = fopen("mwis.txt", "r");
	if (!fd) {
		printf("error open file\n");
		return -1;
	}

	fscanf(fd, "%llu\n", &num_vertices);
	weights = calloc(num_vertices, sizeof(unsigned long long));
	if (!weights) {
		printf("cannot allocate weights array\n");
		ret = -1;
		goto out;
	}

	opt = calloc(num_vertices, sizeof(unsigned long long));
	if (!opt) {
		printf("cannot allocate opt array\n");
		ret = -1;
		goto out;
	}

	mwis = calloc(num_vertices, sizeof(unsigned long long));
	if (!mwis) {
		printf("cannot allocate result array\n");
		ret = -1;
		goto out;
	}

	for (i = 0; i < num_vertices; i++) {
		fscanf(fd, "%llu\n", &weights[i]);
	}

	opt[0] = weights[0];
	for (i = 1; i < num_vertices; i++) {
		if (i == 1)
			opt[i] = MAX(opt[i - 1], weights[i]);
		else
			opt[i] = MAX(opt[i - 1], weights[i] + opt[i - 2]);
	}

	// restore mwis from opt solution
	i = num_vertices;
	mwis_cnt = 0;
	while (i >= 0) {

		if (i <= 1)
			v2 = 0;
		else
			v2 = opt[i - 2];
		if (i < 1)
			v1 = 0;
		else
			v1 = opt[i - 1];
		if (v1 >= v2 + weights[i])
			i--;
		else {
			mwis[mwis_cnt] = weights[i];
			i -= 2;
			mwis_cnt++;
		}
	}

	for (i = 0; i < 8; i++) {
		if (is_weight_in_mwis(mwis, mwis_cnt, weights[idx[i]]))
			printf("%d", 1);
		else
			printf("%d", 0);
	}
	printf("\n");
out:
	if (weights)
		free(weights);
	if (opt)
		free(opt);
	fclose(fd);
	return ret;
}
