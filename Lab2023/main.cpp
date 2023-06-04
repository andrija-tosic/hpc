#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
	int rank, p;
	constexpr int n = 4;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int A[n][n]{};

	if (rank == 0) {
		for (int i = 0; i < n;i++) {
			for (int j = 0; j < n;j++) {
				A[i][j] = i * n + j + 1;
				std::cout << A[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}

	MPI_Status status;
	MPI_Request req;

	if (rank == 0) {
		for (int i = 1; i < p; i++) {
			MPI_Datatype mpi_type;
			MPI_Type_vector(n - i, 1, n + 1, MPI_INT, &mpi_type);
			MPI_Type_commit(&mpi_type);

			MPI_Send(&A[0][i], 1, mpi_type, i, NULL, MPI_COMM_WORLD);
			MPI_Send(&A[i][0], 1, mpi_type, i, NULL, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(A[0], n - rank, MPI_INT, 0, NULL, MPI_COMM_WORLD, &status);
		MPI_Recv(A[1], n - rank, MPI_INT, 0, NULL, MPI_COMM_WORLD, &status);

		std::cout << "rank " << rank << ": " << std::endl;

		for (int i = 0;i < n;i++) {
			for (int j = 0; j < n; j++) {
				std::cout << A[i][j] << " ";
			}

			std::cout << std::endl;
		}

		std::cout << std::endl;
	}

	MPI_Finalize();
	return 0;
}
