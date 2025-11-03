//
// Created by Brynn Kim on 10/30/25.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

struct Qb {
    // College stats
    int c_tds;
    int c_yds;
    int c_ints;

    // NFL stats
    int n_tds;
    int n_yds;
    int n_ints;

    // Both will be computed later
    bool success;
    double distance;
};

// Read a single CSV line by commas into vector<string>
vector<string> read_csv(const string& row) {
    vector<string> res;
    string cur;
    for (char c : row) {
        if (c == ',') {
            res.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    res.push_back(cur);
    return res;
}

// Determines and updates success of a QB
void isSuccessful(Qb& q) {
    int count = 0;
    if (q.n_tds >= 25) count++;
    if (q.n_yds >= 2900) count++;
    if (q.n_ints <= 12) count++;
    q.success = (count >= 2);
}

// Determines and updates Euclidean distance of a QB (K-NN algorithm)
void euclideanDistance(Qb& q, const vector<int>& v) {
    q.distance =  sqrt(
        (v[0]-q.c_tds)*(v[0]-q.c_tds) +
        (v[1]-q.c_yds)*(v[1]-q.c_yds) +
        (v[2]-q.c_ints)*(v[2]-q.c_ints)
    );
}

// Assume all data is organized and pushed into KNN in main.cpp
// Assume k is always smaller than the size of the data set (<= 100,000 approx.)
double KNN(vector<Qb>& qb, const vector<int>& user, int k = 100) {
    // Compute success & distance for all QBs
    for (auto& q : qb) {
        isSuccessful(q);
        euclideanDistance(q, user);
    }

    // Sort QBs by ascending distance (closest distance first)
    sort(qb.begin(), qb.end(),
              [](const Qb& a, const Qb& b){ return a.distance < b.distance; });

    // Count how many of the k nearest QBs were successful in the NFL
    int success_count = 0;
    for (int i = 0; i < k; i++) {
        if (qb[i].success) success_count++;
    }

    // Calculate percentage and return
    return 100.0 * (double)success_count / (double)k;
}