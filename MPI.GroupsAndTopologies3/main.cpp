#include <mpi.h>
#include <iostream>
#include <numeric>

int main(int argc, char** argv) {
	int rank;
	constexpr int n = 4;

	int A[3][3] = {
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	};

	int v = 10;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm comm1;

	MPI_Comm_split(MPI_COMM_WORLD, rank % 3, 0, &comm1);

	int row[3];

	if (rank == 0) {
		MPI_Scatter(A, 3, MPI_INT, row, 3, MPI_INT, 0, comm1);
	}
	else if (rank % 3 == 0) {
		int comm1_rank, n;
		
		MPI_Comm_rank(comm1, &comm1_rank);
		MPI_Comm_size(comm1, &n); 
		int sum = std::accumulate(row, row + 3, 0);

		if (sum > v)
		{
			printf("P[%d](%d): %d\n", rank, comm1_rank, sum);
			fflush(stdout);
		}

	}

	MPI_Finalize();
	return 0;
}
