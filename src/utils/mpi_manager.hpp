#pragma once

// #include <mpi.h>

class MPIManager {
private:
    static int rank;
    static int size;
public:
    static void Init();

    static void Finalize();

    static int getRank();

    static int getSize();

};