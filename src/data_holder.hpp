// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering

#include <vector>
#include <Eigen/Dense>
using namespace std;

#pragma ONCE
struct output_data {
    int loop_counter = 0;
    unsigned long long int num_he = 0;
    vector<vector<float> > centroids;
    Eigen::MatrixXf ballkm_centroids;
    bool timeout = false;
    float sse = 0;
};
