#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
	int rank;
	constexpr int n = 3, m = 4;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	MPI_Comm cart;
	int dims[2] = { m, n }, periods[2] = { true, false };

	int reorder = true;

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart);

	int left_rank, right_rank;
	MPI_Cart_shift(cart, 0, 2, &left_rank, &right_rank);

	std::cout << "P" << rank << ": left: " << left_rank << ", right: " << right_rank << std::endl;

	MPI_Finalize();
	return 0;
}
