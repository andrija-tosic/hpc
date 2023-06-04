#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
	int rank;
	constexpr int n = 3, m = 4;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm cart;
	int dims[2] = { m, n }, periods[2] = { false, true };

	int reorder = true;

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart);

	int up_rank, down_rank;
	MPI_Cart_shift(cart, 1, 1, &up_rank, &down_rank);

	std::cout << "P" << rank << ": up: " << up_rank << ", down: " << down_rank << std::endl;

	int lsum = up_rank + down_rank;

	std::cout << lsum << std::endl;

	int gsum;

	MPI_Reduce(&lsum, &gsum, 1, MPI_INT, MPI_SUM, 0, cart);

	if (rank == 0) {
		std::cout << "P0 sum: " << gsum << std::endl;
	}

	MPI_Finalize();
	return 0;
}
