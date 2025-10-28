#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

using namespace cv;
using namespace std;

struct MyPoint {
    vector<double> data;
};

double distance(const MyPoint &a, const MyPoint &b) {
    double sum = 0;
    for (size_t i = 0; i < a.data.size(); i++)
        sum += (a.data[i] - b.data[i]) * (a.data[i] - b.data[i]);
    return sqrt(sum);
}

vector<MyPoint> kmeans_custom(const vector<MyPoint> &points, int k, vector<int> &assignments, int maxIter = 50) {
    int n = points.size();
    int dim = points[0].data.size();
    vector<MyPoint> centroids(k);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);

    for (int i = 0; i < k; i++)
        centroids[i] = points[dis(gen)];

    assignments.assign(n, 0);

    for (int iter = 0; iter < maxIter; iter++) {
        // Assign points to nearest centroid
        for (int i = 0; i < n; i++) {
            double minDist = distance(points[i], centroids[0]);
            int idx = 0;
            for (int j = 1; j < k; j++) {
                double d = distance(points[i], centroids[j]);
                if (d < minDist) {
                    minDist = d;
                    idx = j;
                }
            }
            assignments[i] = idx;
        }

        // Update centroids
        vector<MyPoint> newCentroids(k);
        vector<int> counts(k, 0);
        for (int i = 0; i < k; i++)
            newCentroids[i].data.assign(dim, 0);

        for (int i = 0; i < n; i++) {
            int cluster = assignments[i];
            for (int d = 0; d < dim; d++)
                newCentroids[cluster].data[d] += points[i].data[d];
            counts[cluster]++;
        }

        for (int i = 0; i < k; i++) {
            if (counts[i] == 0) continue;
            for (int d = 0; d < dim; d++)
                newCentroids[i].data[d] /= counts[i];
        }

        centroids = newCentroids;
    }

    return centroids;
}

void vectorQuantize(Mat &img, const vector<MyPoint> &codebook, const vector<int> &assignments) {
    int idx = 0;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            int cluster = assignments[idx++];
            for (int c = 0; c < 3; c++)
                img.at<Vec3b>(i, j)[c] = static_cast<uchar>(codebook[cluster].data[c]);
        }
    }
}

int main() {
    Mat img = imread("input.jpg");
    if (img.empty()) {
        cerr << "Could not open image!" << endl;
        return -1;
    }

    int maxDim = 200;
    Mat resized;
    float scale = float(maxDim) / max(img.cols, img.rows);
    resize(img, resized, Size(), scale, scale);
    img = resized;

    vector<MyPoint> pixels;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            Vec3b color = img.at<Vec3b>(i, j);
            MyPoint p;
            p.data = { double(color[0]), double(color[1]), double(color[2]) };
            pixels.push_back(p);
        }
    }

    int codebookSize = 16;
    vector<int> assignments;
    vector<MyPoint> codebook = kmeans_custom(pixels, codebookSize, assignments);

    vectorQuantize(img, codebook, assignments);

    imwrite("output.jpg", img);
    cout << "Vector quantization done. Saved as output.jpg" << endl;

    return 0;
}
