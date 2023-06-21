#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 32

int y[N]{}, x[N]{}, z[N]{}, g{};

void seq() {
	for (int i = 1; i < N; i++)
	{
		y[i] = y[i] + x[i - 1];
		x[i] = x[i] + z[i];
		g += z[i - 1];
	}
}

void par() {
	int i;

	y[1] = y[1] + x[0];
	g += z[0];

#pragma omp parallel for reduction(+:g)
	for (i = 2; i < N; i++)
	{
		x[i - 1] = x[i - 1] + z[i - 1];
		y[i] = y[i] + x[i - 1];

		g += z[i - 1];
	}
}

void print() {
	for (size_t i = 0; i < N; i++)
	{
		printf("%d ", x[i]);
	}
	printf("\n");
	for (size_t i = 0; i < N; i++)
	{
		printf("%d ", y[i]);
	}
	printf("\n");
	for (size_t i = 0; i < N; i++)
	{
		printf("%d ", z[i]);
	}
	printf("\n");
	printf("%d ", g);
	printf("\n");
}

void init() {
	omp_set_num_threads(8);
	printf("-%d-\n", omp_get_num_procs());

	g = 0;

	for (size_t i = 0; i < N; i++)
	{
		x[i] = y[i] = z[i] = i + 1;
	}
}

int main() {
	init();

	double t = omp_get_wtime();
	seq();
	print();

	init();

	printf("%lf\n", omp_get_wtime() - t);
	t = omp_get_wtime();
	par();
	printf("---\n");
	print();

	printf("%lf\n", omp_get_wtime() - t);
	return 0;
}
