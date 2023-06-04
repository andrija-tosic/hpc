#include <mpi.h>
#include <iostream>

template <class T>
T find_min(T* arr, int n) {
	T min = std::numeric_limits<T>::max();
	for (int i = 0; i < n;i++) {
		if (arr[i] < min) {
			min = arr[i];
		}
	}
	return min;
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

	MPI_Datatype  mpi_type;
	MPI_Type_vector(n / 2, 1, 2, MPI_INT, &mpi_type);
	MPI_Type_create_resized(mpi_type, 0, n * sizeof(int), &mpi_type);
	MPI_Type_commit(&mpi_type);

	int recv_data[n / p][n / 2]{};

	MPI_Scatter(A, n / p, mpi_type, recv_data, (n / p) * (n / 2), MPI_INT, 0, MPI_COMM_WORLD);

	std::cout << rank << " recv_data: ";

	for (int i = 0; i < n / p;i++) {
		for (int j = 0; j < n / 2;j++) {
			std::cout << recv_data[i][j] << " ";
		}
	}
	std::cout << std::endl;

	int local_min = find_min(&recv_data[0][0], (n / p) * (n / 2));

	std::cout << "local_min: " << local_min << std::endl;

	int global_min;

	MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		std::cout << "global_min: " << global_min;
	}

	MPI_Finalize();
	return 0;
}
