#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Point {
    double age;
    double amount;
    int cluster;
};

// Euclidean distance
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int main() {
    const int N = 100;  
    const int K = 4;   

    // Example dataset: 100 points, each with Age and Amount values
    // You would replace this with actual data (random or real)
    vector<Point> points(N);
    for (int i = 0; i < N; i++) {
        points[i].age = rand() % 50 + 18;    // Random age between 18 and 67
        points[i].amount = rand() % 2000 + 100;  // Random amount between 100 and 2100
        points[i].cluster = -1;  // Initial cluster assignment
    }

    // Initial centroids (Randomly pick K points as initial centroids)
    vector<vector<double>> centroids = { {20, 500}, {40, 1000}, {30, 1200}, {35, 1500} };

    bool changed;
    int iter = 0;

    do {
        changed = false;

        // Assign points to nearest centroid
        for (int i = 0; i < N; i++) {
            double minDist = 1e9;
            int cluster = -1;

            for (int j = 0; j < K; j++) {
                double d = distance(points[i].age, points[i].amount,
                                    centroids[j][0], centroids[j][1]);
                if (d < minDist) {
                    minDist = d;
                    cluster = j;
                }
            }

            if (points[i].cluster != cluster) {
                points[i].cluster = cluster;
                changed = true;
            }
        }

        // Update centroids
        for (int j = 0; j < K; j++) {
            double sumAge = 0, sumAmt = 0;
            int count = 0;
            for (int i = 0; i < N; i++) {
                if (points[i].cluster == j) {
                    sumAge += points[i].age;
                    sumAmt += points[i].amount;
                    count++;
                }
            }
            if (count > 0) {
                centroids[j][0] = sumAge / count;
                centroids[j][1] = sumAmt / count;
            }
        }

        iter++;
    } while (changed && iter < 10);

    // Print results
    cout << "Final clusters after " << iter << " iterations:\n";
    for (int j = 0; j < K; j++) {
        cout << "Cluster " << j + 1
             << " (centroid: Age=" << centroids[j][0]
             << ", Amount=" << centroids[j][1] << ")\n";
        for (int i = 0; i < N; i++) {
            if (points[i].cluster == j) {
                cout << "   Point " << i + 1
                     << " -> Age=" << points[i].age
                     << ", Amount=" << points[i].amount << "\n";
            }
        }
    }

    return 0;
}
