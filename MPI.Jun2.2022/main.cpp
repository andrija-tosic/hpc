#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define p 3
#define n 6
#define m 6

void seq_test() {
	int mat[n][m]{};
	int vec[m]{};
	int vec_res[m];
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			mat[i][j] = i * n + j + 1;
		}
		vec[i] = i + 1;
	}

	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			vec_res[j] = 0;
			for (size_t k = 0; k < m; k++)
			{
				vec_res[j] = mat[i][k] * vec[k];
			}
		}
	}

	for (size_t i = 0; i < m; i++)
	{
		printf("%d ", vec_res[i]);
	}

}

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int mat[n][m]{};
	int vec[m]{};
	int mat_recv[n][m / p];
	int vec_recv[m / p];
	int vec_local_res[m / p];
	int vec_res[m];

	if (rank == 0) {

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				mat[i][j] = i * n + j + 1;
			}
			printf("\n");
			vec[i] = i + 1;
		}
	}

	MPI_Datatype mat_type;
	int blocklengths[n * m];
	for (int i = 0; i < n * m; i++)
	{
		blocklengths[i] = 1;
	}
	int displacements[n * m];
	int i = 0;
	for (int offset = 0; offset < p; offset++)
	{
		for (int j = 0; j < n * m; j += p)
		{
			displacements[i] = offset + j;
			i++;
		}
	}

	MPI_Type_vector(n * m / p, 1, p, MPI_INT, &mat_type);
	MPI_Type_create_resized(mat_type, 0, sizeof(int), &mat_type);
	MPI_Type_commit(&mat_type);

	MPI_Scatter(mat, 1, mat_type, mat_recv, n * m / p, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Datatype vec_type;
	MPI_Type_vector(m / p, 1, p, MPI_INT, &vec_type);
	MPI_Type_create_resized(vec_type, 0, sizeof(int), &vec_type);
	MPI_Type_commit(&vec_type);

	MPI_Scatter(vec, 1, vec_type, vec_recv, m / p, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m / p; j++)
		{
			vec_local_res[j] = 0;
			for (int k = 0; k < m / p;k++)
			{
				vec_local_res[j] += mat_recv[i][k] * vec_recv[k];
			}
		}
	}

	MPI_Gather(vec_local_res, m / p, MPI_INT, vec_res, 1, vec_type, 0, MPI_COMM_WORLD);

	printf("p%d\n", rank);
	for (int i = 0;i < n;i++) {
		for (int j = 0; j < m / p; j++)
		{
			printf("%d ", mat_recv[i][j]);
		}
		printf("\n");
	}
	printf("---\n");
	for (size_t i = 0; i < m / p; i++)
	{
		printf("%d ", vec_recv[i]);
	}

	if (rank == 0) {

		printf("---result---\n");
		for (size_t i = 0; i < m; i++)
		{
			printf("%d ", vec_res[i]);
		}
		printf("---\n");
		seq_test();
	}

	MPI_Finalize();
	return 0;
}
