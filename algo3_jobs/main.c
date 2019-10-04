/*
 * main.c
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: alekseym
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define PA 2 // 2

struct job {
	int weight;
	int len;
};

int compare_jobs(const void *a, const void *b)
{
	const struct job *job1 = a;
	const struct job *job2 = b;
#if PA == 1
	int score1 = job1->weight - job1->len;
	int score2 = job2->weight - job2->len;
#else // PA ==2
	double score1 = (double)job1->weight / (double)job1->len;
	double score2 = (double)job2->weight / (double)job2->len;
#endif

	if (score1 == score2) {
		if (job1->weight > job2->weight)
			return -1;
		else
			return 1;
	} else {
		if (score2 > score1)
			return 1;
		else
			return -1;
		//return score2 - score1;
	}
}

unsigned long long calc_completion_time(struct job *jobs, int num_jobs)
{
	unsigned long long cur_time = 0;
	unsigned long long res = 0;
	int i;

	for (i = 0; i < num_jobs; i++) {
		cur_time += jobs[i].len;
		res += cur_time * jobs[i].weight;
	}

	return res;
}

int main()
{
	FILE *fd;
	int num_jobs;
	int i;
	struct job *jobs;
	unsigned long long res;

	fd = fopen("../jobs.txt", "r");
	if (!fd) {
		printf("error open file\n");
		return -1;
	}

	fscanf(fd, "%d", &num_jobs);

	jobs = malloc(num_jobs * sizeof(struct job));

	for (i = 0; i < num_jobs; i++) {
		if (fscanf(fd, "%d %d", &jobs[i].weight, &jobs[i].len) != 2) {
			printf("error while reading file.\n");
		}
	}
	qsort(jobs, num_jobs, sizeof(struct job), compare_jobs);

	res = calc_completion_time(jobs, num_jobs);
	printf("%llu\n", res);

	fclose(fd);
	free(jobs);

	return 0;
}
