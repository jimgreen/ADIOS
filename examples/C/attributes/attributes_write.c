/* ADIOS C Example: write some attributes along with variables
*/
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "adios.h"
int main (int argc, char ** argv) 
{
    char        filename [256];
    int         rank, size, i;
    int         NX = 10, level = 1; 
    double      t[NX], mean = 0;
    MPI_Comm    comm = MPI_COMM_WORLD;
    const char * str = "Nov, 2009";

    /* ADIOS variables declarations for matching gwrite_temperature.ch */
    int         adios_err;
    uint64_t    adios_groupsize, adios_totalsize;
    int64_t     adios_handle;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (comm, &rank);
    MPI_Comm_size (comm, &size);

    for (i = 0; i < NX; i++)
    {
        t[i] = rank * NX + i;
        mean += t[i];
    }

    mean /= NX;

    strcpy (filename, "attributes.bp");

    adios_init ("attributes.xml");

    adios_open (&adios_handle, "temperature", filename, "w", &comm);
#include "gwrite_temperature.ch"
    adios_close (adios_handle);

    MPI_Barrier (comm);

    adios_finalize (rank);

    MPI_Finalize ();
    return 0;
}
