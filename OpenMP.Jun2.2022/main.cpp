#include <stdio.h>
int g = 0;
#define N 10

int x[N + 1], y[N], z[N];

void seq() {
	for (int i = 1; i < N; i++)
	{
		y[i] = y[i] + x[i + 1];
		x[i] = x[i] + z[i];
		g += z[i - 1];
	}
}

void par() {
	int x2[N + 1]{};
#pragma omp parallel for
	for (int i = 0; i < N; i++)
	{
		x2[i] = x[i + 1];
	}

#pragma omp parallel for reduction(+:g)
	for (int i = 1; i < N; i++)
	{
		y[i] = y[i] + x2[i];
		x[i] = x[i] + z[i];
		g += z[i - 1];
	}
}


void reset() {
	g = 0;

	for (int i = 0; i < N; i++)
	{
		x[i] = y[i] = z[i] = 1;
	}
	x[N] = 1;
}

void print() {
	printf("\ng=%d\n", g);
	printf("\nx:\n");
	for (int i = 0; i < N + 1; i++)
	{
		printf("%d ", x[i]);
	}
	printf("\ny:\n");
	for (int i = 0; i < N; i++)
	{
		printf("%d ", y[i]);
	}
	printf("\nz:\n");
	for (int i = 0; i < N; i++)
	{
		printf("%d ", z[i]);
	}
}

int main()
{
	reset();
	seq();
	print();
	reset();
	par();
	print();
}