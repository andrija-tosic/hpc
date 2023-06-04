#include <mpi.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char** argv) {
	int rank;
	constexpr int n = 4, p = 4;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Comm_size(MPI_COMM_WORLD, &p);

	constexpr int q = 2; // sqrt(p)

	int A[n][n]{}, B[n][n]{}, C[n][n]{};

	if (rank == 0)
	{

		for (size_t i = 0; i < n; i++)
		{
			for (size_t j = 0; j < n; j++)
			{
				A[i][j] = i * n + j + 1;
				B[i][j] = i * n + j;
			}
		}
	}

	MPI_Datatype row_blocks, col_blocks;
	MPI_Type_vector(n / q, n * n / q, n * n / q, MPI_INT, &row_blocks);

	MPI_Type_vector(n, n / q, n, MPI_INT, &col_blocks);
	MPI_Type_create_resized(col_blocks, 0, (n / q) * sizeof(int), &col_blocks);
	MPI_Type_commit(&row_blocks);
	MPI_Type_commit(&col_blocks);

	MPI_Comm row_comm, col_comm;

	MPI_Comm_split(MPI_COMM_WORLD, rank / q, 0, &row_comm);
	MPI_Comm_split(MPI_COMM_WORLD, rank % q, 0, &col_comm);

	int A_recv[n / q][n]{}, B_recv[n][n / q];

	MPI_Scatter(A, 1, row_blocks, A_recv, n * n / q, MPI_INT, 0, row_comm);
	MPI_Scatter(B, 1, col_blocks, B_recv, n * n / q, MPI_INT, 0, col_comm);

	int local[n / q][n / q]{};

	for (size_t i = 0; i < n / q; i++)
	{
		for (size_t j = 0; j < n / q; j++)
		{
			for (size_t k = 0; k < n; k++)
			{
				local[i][j] += A_recv[i][k] * B_recv[k][j];
			}
		}
	}

	int result[n][n]{};

	MPI_Gather(local, n / q, MPI_INT, result, 1, row_blocks, 0, row_comm);
	MPI_Gather(local, n / q, MPI_INT, result, 1, col_blocks, 0, col_comm);

	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			printf("%d ", result[i][j]);
		}
		printf("\n");
	}

	MPI_Finalize();
	return 0;
}
