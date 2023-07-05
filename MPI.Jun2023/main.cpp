#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <string>

struct Zaposleni {
	int id;
	char ime[20];
	char prezime[20];
	double plata;
} zaposleni;


int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	constexpr int n = 12;
	constexpr int p = 4; // = size

	MPI_Datatype zaposleniType;
	int blocklengths[] = { 1,  20,  20, 1 };
	MPI_Aint displacements[4]{};

	MPI_Get_address(&zaposleni.id, &displacements[0]);
	MPI_Get_address(&zaposleni.ime, &displacements[1]);
	MPI_Get_address(&zaposleni.prezime, &displacements[2]);
	MPI_Get_address(&zaposleni.plata, &displacements[3]);

	MPI_Aint base_address;
	MPI_Get_address(&zaposleni, &base_address);

	for (size_t i = 0; i < 4; i++)
	{
		displacements[i] -= base_address;
	}

	MPI_Datatype types[] = { MPI_INT, MPI_CHAR, MPI_CHAR, MPI_DOUBLE };

	MPI_Type_create_struct(4, blocklengths, displacements, types, &zaposleniType);
	MPI_Type_commit(&zaposleniType);

	Zaposleni zaSlanje[n]{};
	Zaposleni zaPrimanje[n / p]{};

	if (rank == 0) {
		for (size_t i = 0; i < n; i++)
		{
			//gets_s(zaSlanje[i].ime);
			//gets_s(zaSlanje[i].prezime);
			//scanf_s("%lf", &zaSlanje[i].plata);
			zaSlanje[i].id = i;
			strcpy_s(zaSlanje[i].ime, (std::string("ime") + std::to_string(i)).c_str());
			strcpy_s(zaSlanje[i].prezime, (std::string("prezime") + std::to_string(i)).c_str());
			zaSlanje[i].plata = rand() % 99 + 1;

			printf("%d %s %s %lf\n", zaSlanje[i].id, zaSlanje[i].ime, zaSlanje[i].prezime, zaSlanje[i].plata);
		}
	}

	MPI_Scatter(zaSlanje, n / p, zaposleniType, zaPrimanje, n / p, zaposleniType, 0, MPI_COMM_WORLD);

	struct {
		double val = INT_MAX;
		int index = -1;
	} in{}, out{};

	for (size_t i = 0; i < n / p; i++)
	{
		if (zaPrimanje[i].plata < in.val) {
			in.val = zaPrimanje[i].plata;
			in.index = zaPrimanje[i].id;
		}
	}

	MPI_Reduce(&in, &out, 1, MPI_DOUBLE_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Najmanja plata: id=%d, plata=%lf", out.index, out.val);
	}

	MPI_Finalize();
	return 0;
}
