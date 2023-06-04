#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
	int rank;
	constexpr int k = 6, n = 4, m = 2, p = 3;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Comm_size(MPI_COMM_WORLD, &p);


	int A[k][n]{}, B[n][m]{}, C[k][m]{};

	if (rank == 0) {

		for (int i = 0; i < k; i++) {
			for (int j = 0; j < n; j++) {
				A[i][j] = i+1;
			}
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				B[i][j] = j+1;
			}
		}
	}

	MPI_Datatype vrste;
	MPI_Type_vector(k / p, n, n * p, MPI_INT, &vrste);
	MPI_Type_create_resized(vrste, 0, sizeof(int) * n, &vrste);
	MPI_Type_commit(&vrste);

	int A_recv[k / p][n]{  };

	MPI_Bcast(B, n * m, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, 1, vrste, A_recv, (k/p)*n, MPI_INT, 0, MPI_COMM_WORLD);

	int local_C[k / p][m]{};

	for (int i = 0; i < k / p; i++) {
		for (int j = 0; j < m; j++) {
			for (int l = 0; l < n; l++) {
				local_C[i][j] += A_recv[i][l] * B[l][j];
			}
		}
	}

	MPI_Datatype novi;
	MPI_Type_vector(k / p, m, m * p, MPI_INT, &novi);
	MPI_Type_create_resized(novi, 0, sizeof(int) * m, &novi);
	MPI_Type_commit(&novi);

	MPI_Gather(local_C, (k / p) * m, MPI_INT, C, 1, novi, 0, MPI_COMM_WORLD);

	if (rank == 0) {

		for (int i = 0; i < k; i++) {
			for (int j = 0; j < m; j++) {
				printf("%d ", C[i][j]);
			}
			printf("\n");
		}
	}


	MPI_Finalize();
	return 0;
}
