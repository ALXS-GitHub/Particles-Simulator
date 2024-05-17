#include "mpi_manager.hpp"
#include <mpi.h> 
#include <omp.h>
#include <iostream>
#ifndef _OPENMP
    #define _OPENMP 0
#endif

int MPIManager::rank = 0;
int MPIManager::size = 1;

void MPIManager::Init() {
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        std::cout << "MPI Initialized with " << size << " processes" << std::endl;
        std::cout << "OpenMP version: " << _OPENMP << std::endl;
    }

    for (int i = 0; i < size; i++) {
        if (rank == i) {
            int num_threads = omp_get_max_threads();
            std::cout << "Number of OMP threads: " << num_threads << " in MPI process " << rank << std::endl;
            omp_set_num_threads(num_threads);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void MPIManager::Finalize() {
    MPI_Finalize();
}
int MPIManager::getRank() {
    return rank;
}

int MPIManager::getSize() {
    return size;
}