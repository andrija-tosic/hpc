#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char** argv) {
	int rank, size;
	MPI_File f;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int N = 10;

	char* tekst = new char[N];
	for (size_t i = 0; i < N; i++)
	{
		tekst[i] = rank;
	}

	MPI_File_open(MPI_COMM_WORLD, "prva.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);

	MPI_File_set_view(f, rank * N, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);

	MPI_File_write(f, tekst, N, MPI_CHAR, &status);

	MPI_File_close(&f);

	MPI_Barrier(MPI_COMM_WORLD);

	// 2.
	MPI_File_open(MPI_COMM_WORLD, "prva.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &f);

	MPI_File_read_shared(f, tekst, N, MPI_CHAR, &status);

	printf("[p%d] %s", rank, tekst);

	MPI_File_close(&f);

	// 3.

	MPI_File_open(MPI_COMM_WORLD, "druga.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &f);

	MPI_Datatype type;
	MPI_Type_vector(N, 2, 2 * size, MPI_CHAR, &type);
	MPI_Type_create_resized(type, 0, 2*size, &type);
	MPI_Type_commit(&type);

	MPI_File_set_view(f, 2 * rank, MPI_CHAR, type, "native", MPI_INFO_NULL);
	MPI_File_write_all(f, tekst, N, MPI_CHAR, &status);

	MPI_File_set_view(f, size*N+2 * rank, MPI_CHAR, type, "native", MPI_INFO_NULL);
	MPI_File_write_all(f, tekst, N, MPI_CHAR, &status);

	MPI_File_close(&f);
	delete[] tekst;
	MPI_Finalize();
	return 0;
}
