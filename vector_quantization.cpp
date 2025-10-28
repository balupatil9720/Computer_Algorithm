#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include <chrono>

using namespace std;

// Point in multidimensional space
struct Point {
    vector<double> data;

    Point(int dim = 2) {
        data.resize(dim, 0);
    }
};

// Euclidean distance
double distance(const Point &a, const Point &b) {
    double sum = 0;
    for(size_t i = 0; i < a.data.size(); i++) {
        double diff = a.data[i] - b.data[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

// KMeans for vector quantization
vector<Point> kmeans(const vector<Point> &points, int k, vector<int> &assignments, int maxIter = 100) {
    int n = points.size();
    int dim = points[0].data.size();
    vector<Point> centroids(k, Point(dim));
    assignments.resize(n);

    // Initialize centroids randomly
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n-1);
    for(int i = 0; i < k; i++)
        centroids[i] = points[dis(gen)];

    for(int iter = 0; iter < maxIter; iter++) {
        // Assignment step
        for(int i = 0; i < n; i++) {
            double bestDist = numeric_limits<double>::max();
            int bestCluster = 0;
            for(int j = 0; j < k; j++) {
                double d = distance(points[i], centroids[j]);
                if(d < bestDist) {
                    bestDist = d;
                    bestCluster = j;
                }
            }
            assignments[i] = bestCluster;
        }

        // Update step
        vector<Point> newCentroids(k, Point(dim));
        vector<int> counts(k, 0);

        for(int i = 0; i < n; i++) {
            int cluster = assignments[i];
            for(int d = 0; d < dim; d++)
                newCentroids[cluster].data[d] += points[i].data[d];
            counts[cluster]++;
        }

        for(int j = 0; j < k; j++) {
            if(counts[j] > 0)
                for(int d = 0; d < dim; d++)
                    newCentroids[j].data[d] /= counts[j];
            else
                newCentroids[j] = points[dis(gen)]; // reinitialize empty cluster
        }
        centroids = newCentroids;
    }
    return centroids;
}

// Vector Quantization: encode points using codebook
vector<int> vectorQuantize(const vector<Point> &points, const vector<Point> &codebook) {
    vector<int> indices(points.size());
    for(size_t i = 0; i < points.size(); i++) {
        double bestDist = numeric_limits<double>::max();
        int bestIndex = 0;
        for(size_t j = 0; j < codebook.size(); j++) {
            double d = distance(points[i], codebook[j]);
            if(d < bestDist) {
                bestDist = d;
                bestIndex = j;
            }
        }
        indices[i] = bestIndex;
    }
    return indices;
}

int main() {
    int numPoints = 10000;
    int dim = 2;       // 2D points for simplicity
    int codebookSize = 16; // Number of codewords (clusters)

    // Generate synthetic data points
    vector<Point> points(numPoints, Point(dim));
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1000);

    for(int i = 0; i < numPoints; i++)
        for(int d = 0; d < dim; d++)
            points[i].data[d] = dis(gen);

    auto start = chrono::high_resolution_clock::now();

    // Generate codebook using KMeans
    vector<int> assignments;
    vector<Point> codebook = kmeans(points, codebookSize, assignments, 100);

    // Encode points using vector quantization
    vector<int> encodedIndices = vectorQuantize(points, codebook);

    auto end = chrono::high_resolution_clock::now();

    cout << "Codebook vectors (centroids):\n";
    for(int i = 0; i < codebookSize; i++) {
        cout << "Codeword " << i << ": (";
        for(int d = 0; d < dim; d++)
            cout << codebook[i].data[d] << (d == dim-1 ? ")\n" : ", ");
    }

    cout << "Example encoded indices for first 10 points:\n";
    for(int i = 0; i < 10; i++)
        cout << encodedIndices[i] << " ";
    cout << "\n";

    chrono::duration<double> diff = end - start;
    cout << "Time taken: " << diff.count() << " seconds\n";

    return 0;
}
