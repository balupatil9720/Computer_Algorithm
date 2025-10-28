#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define N 100000

int main() {
    int data[N];
    for (int i = 0; i < N; i++)
        data[i] = i + 1;  // Initialize array: 1, 2, ..., N

    clock_t start = clock();

    int min = INT_MAX;
    int max = INT_MIN;
    long long sum = 0;
    unsigned long long product = 1; // overflow expected

    for (int i = 0; i < N; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
        sum += data[i];
        product *= data[i]; // overflow is inevitable here
    }

    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Sequential computation (N = %d):\n", N);
    printf("Min: %d\nMax: %d\nSum: %lld\n", min, max, sum);
    printf("Product (overflow expected): %llu\n", product);
    printf("Time taken: %.6f seconds\n", time_taken);

    return 0;
}
