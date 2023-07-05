#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
	int rank, size, cart_rank, upper_lower_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm cart_comm;
	const int n = 4,
		dimensions = 2,
		dims[dimensions] = { n, n },
		periods[dimensions] = { 0, 0 };
	int cart_coords[dimensions], sum;

	if (size != n * n)
		exit(-1);

	MPI_Cart_create(MPI_COMM_WORLD, dimensions, dims, periods, 1, &cart_comm);
	MPI_Comm_rank(cart_comm, &cart_rank);
	MPI_Cart_coords(cart_comm, cart_rank, dimensions, cart_coords);

	if (cart_coords[0] == cart_coords[1]) {
		printf("[%d]", cart_rank);
		fflush(stdout);
	}

	MPI_Request req;

	MPI_Comm upper_lower_group;
	MPI_Comm_split(cart_comm, cart_coords[0] + cart_coords[1] < n, cart_rank, &upper_lower_group);
	MPI_Comm_rank(upper_lower_group, &upper_lower_rank);

	MPI_Reduce(&cart_rank, &sum, 1, MPI_INT, MPI_SUM, 0, upper_lower_group);

	MPI_Comm upper_diagonal_comm;

	MPI_Comm_split(upper_lower_group, cart_coords[0] == cart_coords[1], cart_rank, &upper_diagonal_comm);

	std::cout << std::endl;

	MPI_Comm newgroup;
	if (upper_lower_rank == 0) {
		MPI_Isend(&sum, 1, MPI_INT, 0, 0, cart_comm, &req);
	} if (rank == 0) {
		int res_sum;
		MPI_Recv(&res_sum, 1, MPI_INT, MPI_ANY_SOURCE, 0, cart_comm, MPI_STATUS_IGNORE);
		printf("P%d: Lower sum is %d.\n", rank, res_sum);
		MPI_Recv(&res_sum, 1, MPI_INT, MPI_ANY_SOURCE, 0, cart_comm, MPI_STATUS_IGNORE);
		printf("P%d: Upper sum is %d.\n", rank, res_sum);
		fflush(stdout);
	}

	MPI_Finalize();
	return 0;
}
