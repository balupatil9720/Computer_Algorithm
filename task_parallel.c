#include <stdio.h>
#include <mpi.h>
#include <limits.h>
#define N 10000

int main(int argc, char** argv) {
    int rank, size;
    int data[N];
    double start_time, end_time, local_time, total_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 4) {
        if (rank == 0) printf("Error: Run with at least 4 processes.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        for (int i = 0; i < N; ++i) data[i] = i + 1;
    }

    MPI_Bcast(data, N, MPI_INT, 0, MPI_COMM_WORLD);

    start_time = MPI_Wtime();

    if (rank == 0) {
        int min = INT_MAX;
        for (int i = 0; i < N; ++i) if (data[i] < min) min = data[i];
        printf("🔹 Process %d - Min: %d\n", rank, min);
    } else if (rank == 1) {
        int max = INT_MIN;
        for (int i = 0; i < N; ++i) if (data[i] > max) max = data[i];
        printf("🔹 Process %d - Max: %d\n", rank, max);
    } else if (rank == 2) {
        long long sum = 0;
        for (int i = 0; i < N; ++i) sum += data[i];
        printf("🔹 Process %d - Sum: %lld\n", rank, sum);
    } else if (rank == 3) {
        long long product = 1;
        for (int i = 0; i < N; ++i) product *= data[i]; // overflow expected
        printf("🔹 Process %d - Product (overflow expected): %lld\n", rank, product);
    }

    end_time = MPI_Wtime();
    local_time = end_time - start_time;

    MPI_Reduce(&local_time, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total time taken: %.6f seconds\n", total_time);
    }

    MPI_Finalize();
    return 0;
}
