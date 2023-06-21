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

	MPI_File_open(MPI_COMM_WORLD, "file1.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);

	int br[9];
	for (size_t i = 0; i < 9; i++)
	{
		br[i] = rank;
	}

	MPI_File_seek(f, rank * 9 * sizeof(int), MPI_SEEK_SET);
	MPI_File_write(f, br, 9, MPI_INT, &status);

	MPI_File_close(&f);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_File_open(MPI_COMM_WORLD, "file1.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &f);

	MPI_File_read_at(f, rank * 9 * sizeof(int), br, 9, MPI_INT, &status);

	MPI_File_close(&f);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_File_open(MPI_COMM_WORLD, "file2.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);

	int sizes[] = {size, 9};
	int distribs[] = { MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK };
	int dargs[] = { MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG };
	int psizes[] = { 2, 3 };

	MPI_Datatype darray;
	MPI_Type_create_darray(size, rank, 2, sizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_INT, &darray);
	MPI_Type_commit(&darray);

	MPI_File_set_view(f, 0, MPI_INT, darray, "native", MPI_INFO_NULL);
	MPI_File_write_all(f, br, 9, MPI_INT, &status);

	MPI_File_close(&f);

	MPI_Finalize();
	return 0;
}
