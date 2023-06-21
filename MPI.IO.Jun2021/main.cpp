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
	int br[105];
	for (size_t i = 0; i < 105; i++)
	{
		br[i] = rank;
	}

	MPI_File_open(MPI_COMM_WORLD, "file1.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &f);
	MPI_File_write_at(f, 105 * rank * sizeof(int), &br, 105, MPI_INT, &status);

	MPI_File_close(&f);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_File_open(MPI_COMM_WORLD, "file1.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
	MPI_File_read_at(f, 105 * rank * sizeof(int), br, 105, MPI_INT, &status);

	printf("p%d ", rank);
	for (size_t i = 0; i < 105; i++)
	{
		printf("%d ", br[i]);
	}
	printf("\n");

	MPI_File_close(&f);
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_File f2;

	MPI_File_open(MPI_COMM_WORLD, "file2.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &f2);

	int array_of_blocklengths[14];
	int array_of_displacements[14];

	int disp = rank;

	for (int i = 0;i < 14;i++) {
		array_of_blocklengths[i] = i + 1;
		array_of_displacements[i] = disp;

		disp += (i + 1) * size * sizeof(int);
	}

	MPI_Datatype indexed;	
	MPI_Type_indexed(14, array_of_blocklengths, array_of_displacements, MPI_INT, &indexed);
	MPI_Type_commit(&indexed);

	MPI_File_set_view(f2, 0, MPI_INT, indexed, "native", MPI_INFO_NULL);

	MPI_File_write_all(f2, br, 105, MPI_INT, &status);

	MPI_File_close(&f2);
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}
