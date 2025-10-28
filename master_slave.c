#include <mpi.h>
#include <stdio.h>

#define SIZE 100

int main(int argc, char** argv) {
    int rank, size;
    int numbers[SIZE];
    int chunk_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    chunk_size = SIZE / size;  // divide array equally among all processes

    int local_array[chunk_size];

    if(rank == 0) {
        // Initialize array
        for(int i = 0; i < SIZE; i++)
            numbers[i] = i + 1;
    }

    // Scatter chunks to all processes
    MPI_Scatter(numbers, chunk_size, MPI_INT, local_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process computes local sum
    int local_sum = 0;
    for(int i = 0; i < chunk_size; i++)
        local_sum += local_array[i];

    // Reduce all local sums to total sum at master
    int total_sum = 0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        printf("Total sum of array is: %d\n", total_sum);
    }

    MPI_Finalize();
    return 0;
}
