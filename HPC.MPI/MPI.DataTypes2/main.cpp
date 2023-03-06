#include <mpi.h>
#include <iostream>

template <class T>
T find_max(T* arr, int n) {
	return *std::max_element(arr, arr + n);
}

int main(int argc, char** argv) {
	int rank;
	constexpr int n = 4, p = 4;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Comm_size(MPI_COMM_WORLD, &p);


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

	MPI_Datatype temp_type, mpi_type;
	MPI_Type_vector(n/2, n / p, n - n / p - 1, MPI_INT, & temp_type);
	MPI_Type_create_resized(temp_type, 0, n * sizeof(int), &mpi_type);
	MPI_Type_commit(&mpi_type);

	int recv_data[n / 2][n / p]{};

	MPI_Scatter(A, n / p, mpi_type, recv_data, (n / 2) * (n / p), MPI_INT, 0, MPI_COMM_WORLD);

	std::cout << rank << " recv_data: ";

	for (int i = 0; i < n / 2; i++) {
		for (int j = 0; j < n / p;j++) {
			std::cout << recv_data[i][j] << " ";
		}
	}
	std::cout << std::endl;

	int local_max = find_max(&recv_data[0][0], (n / 2) * (n / p));

	std::cout << "local_max: " << local_max << std::endl;

	int global_max;

	MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		std::cout << "global_max: " << global_max;
	}

	MPI_Finalize();
	return 0;
}
