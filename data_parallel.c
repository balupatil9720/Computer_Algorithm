#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>
#define N 10000

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk = N / size;
    int remainder = N % size;
    int* data = NULL;
    if (rank == 0) {
        data = (int*)malloc(sizeof(int) * N);
        for (int i = 0; i < N; i++) data[i] = i + 1;
    }

    int local_chunk = chunk + (rank < remainder ? 1 : 0);
    int* local = (int*)malloc(sizeof(int) * local_chunk);

    int* sendcounts = (int*)malloc(size * sizeof(int));
    int* displs = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        sendcounts[i] = chunk + (i < remainder ? 1 : 0);
        displs[i] = (i > 0 ? displs[i-1] + sendcounts[i-1] : 0);
    }

    double start_time = MPI_Wtime();

    MPI_Scatterv(data, sendcounts, displs, MPI_INT, local, local_chunk, MPI_INT, 0, MPI_COMM_WORLD);

    int local_min = INT_MAX, local_max = INT_MIN;
    long long local_sum = 0, local_product = 1;
    for (int i = 0; i < local_chunk; i++) {
        if (local[i] < local_min) local_min = local[i];
        if (local[i] > local_max) local_max = local[i];
        local_sum += local[i];
        local_product *= local[i]; // overflow expected
    }

    int global_min, global_max;
    long long global_sum, global_product;
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_product, &global_product, 1, MPI_LONG_LONG, MPI_PROD, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();
    double local_time = end_time - start_time;
    double total_time;

    MPI_Reduce(&local_time, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Data Parallelism:\n");
        printf("Min: %d\nMax: %d\nSum: %lld\nProduct (overflow expected): %lld\n", 
                global_min, global_max, global_sum, global_product);
        printf("Total time taken: %.6f seconds\n", total_time);
    }

    free(local);
    if (rank == 0) free(data);
    free(sendcounts);
    free(displs);

    MPI_Finalize();
    return 0;
}
