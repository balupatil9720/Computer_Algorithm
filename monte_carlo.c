#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) fprintf(stderr, "Usage: %s <total_points>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    long long total_points = atoll(argv[1]);
    if (total_points <= 0) {
        if (rank == 0) fprintf(stderr, "total_points must be > 0\n");
        MPI_Finalize();
        return 1;
    }

    long long base = total_points / size;
    int rem = total_points % size;
    long long local_points = base + (rank < rem ? 1 : 0);

    unsigned int seed = (unsigned int) time(NULL) ^ (unsigned int)(rank * 7919);
    srand(seed);

    long long local_count = 0;
    for (long long i = 0; i < local_points; ++i) {
        double x = (double)rand() / (double)RAND_MAX;
        double y = (double)rand() / (double)RAND_MAX;
        if (x*x + y*y <= 1.0) local_count++;
    }

    long long global_count = 0;
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi_est = 4.0 * (double)global_count / (double)total_points;
        double abs_err = fabs(pi_est - M_PI);
        double rel_err_percent = (abs_err / M_PI) * 100.0;
        printf("Processes: %d\n", size);
        printf("Total points: %lld\n", total_points);
        printf("Points inside circle: %lld\n", global_count);
        printf("Estimated pi = %.12f\n", pi_est);
        printf("M_PI         = %.12f\n", M_PI);
        printf("Absolute error = %.12f\n", abs_err);
        printf("Relative error = %.8f %%\n", rel_err_percent);
    }

    MPI_Finalize();
    return 0;
}
