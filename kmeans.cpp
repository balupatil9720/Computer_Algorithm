#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;

struct Point {
    double x, y;
};

double distance(const Point &a, const Point &b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

// KMeans algorithm with convergence stopping
vector<Point> kmeans(const vector<Point> &points, int k, vector<int> &assignments, int maxIter = 100, double tol = 1e-6) {
    int n = points.size();
    vector<Point> centroids(k);
    assignments.resize(n);

    // Initialize centroids randomly
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n-1);
    for(int i=0;i<k;i++) centroids[i] = points[dis(gen)];

    for(int iter=0; iter<maxIter; iter++) {
        // Assignment step
        for(int i=0;i<n;i++){
            double bestDist = numeric_limits<double>::max();
            int bestCluster = 0;
            for(int j=0;j<k;j++){
                double d = distance(points[i], centroids[j]);
                if(d < bestDist){
                    bestDist = d;
                    bestCluster = j;
                }
            }
            assignments[i] = bestCluster;
        }

        // Update step
        vector<Point> newCentroids(k, {0,0});
        vector<int> counts(k,0);
        for(int i=0;i<n;i++){
            int cluster = assignments[i];
            newCentroids[cluster].x += points[i].x;
            newCentroids[cluster].y += points[i].y;
            counts[cluster]++;
        }
        for(int j=0;j<k;j++){
            if(counts[j] > 0){
                newCentroids[j].x /= counts[j];
                newCentroids[j].y /= counts[j];
            } else {
                newCentroids[j] = points[dis(gen)];
            }
        }

        // Check for convergence
        bool converged = true;
        for(int j=0;j<k;j++){
            if(distance(centroids[j], newCentroids[j]) > tol){
                converged = false;
                break;
            }
        }
        centroids = newCentroids;

        if(converged) break; // stop early if centroids barely moved
    }
    return centroids;
}

// Compute total inertia (sum of squared distances to centroids)
double computeInertia(const vector<Point> &points, const vector<Point> &centroids, const vector<int> &assignments){
    double inertia = 0.0;
    for(size_t i=0;i<points.size();i++){
        inertia += pow(distance(points[i], centroids[assignments[i]]),2);
    }
    return inertia;
}

// Elbow method to determine optimal k
int determineOptimalK(const vector<Point> &points, int maxK=10){
    vector<double> inertias;
    vector<int> assignments;
    for(int k=1;k<=maxK;k++){
        vector<Point> centroids = kmeans(points, k, assignments, 50);
        double inertia = computeInertia(points, centroids, assignments);
        inertias.push_back(inertia);
    }

    // Simple elbow detection: find where relative drop slows down
    for(size_t i=1;i<inertias.size();i++){
        double drop = inertias[i-1] - inertias[i];
        double nextDrop = (i+1<inertias.size()) ? inertias[i] - inertias[i+1] : drop;
        if(nextDrop / drop < 0.5)
            return i+1; // k = i+1
    }
    return maxK;
}

int main(){
    const int N = 10000;
    vector<Point> points(N);

    // Generate synthetic cluster data
    int trueClusters = 5;
    vector<Point> clusterCenters = {{200,200},{800,200},{200,800},{800,800},{500,500}};
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> d(0,50);

    for(int i=0;i<N;i++){
        int c = i % trueClusters;
        points[i].x = clusterCenters[c].x + d(gen);
        points[i].y = clusterCenters[c].y + d(gen);
    }

    auto start = chrono::high_resolution_clock::now();
    int optimalK = determineOptimalK(points, 10);
    vector<int> assignments;
    vector<Point> finalCentroids = kmeans(points, optimalK, assignments, 100);
    auto end = chrono::high_resolution_clock::now();

    cout << "Optimal number of clusters determined by Elbow Method: " << optimalK << "\n";
    for(int i=0;i<optimalK;i++)
        cout << "Cluster " << i << " centroid: (" << finalCentroids[i].x << ", " << finalCentroids[i].y << ")\n";

    chrono::duration<double> diff = end-start;
    cout << "Time taken: " << diff.count() << " seconds\n";

    return 0;
}
