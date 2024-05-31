// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering


#include <iostream>
#include <vector>
#include "math.h"
#include <map>
#include <algorithm>
#include <cmath>
#include <random>
#pragma once

using namespace std;

class algorithm_utils{
    public:

    template <typename T1, typename T2>
    void calculate_distances(const vector<vector<T1> > &dataset, 
    vector<vector<T1> > &centroids, vector<vector<T1> > &dist_mat,
    T2 num_clusters, vector<T2> &assigned_clusters, 
    vector<vector<T1> > &cluster_size, unsigned long long int &he_counter);

    template <typename T1, typename T2>
    void update_centroids(vector<vector <T1> > &dataset, 
    vector<vector<T1> > &new_centroids, vector<T2> &assigned_clusters, 
    vector<vector<T1> > &cluster_size, T2 numCols);

    template <typename T1>
    bool check_convergence(vector<vector <T1> > &new_centroids, 
    vector<vector <T1> > &centroids, T1 threshold, float &diff, float &temp_diff, int &i, int &j);

    template <typename T1, typename T2>
    void extract_data(vector<vector <T1> > &dataset, vector<vector <T1> > &extracted_data, 
    T2 num_points, T2 seed);
    
    template <typename T1>
    void reinit(vector<vector<T1> > &);

    template <typename T1, typename T2>
    void init_centroids(vector<vector <T1> > &centroids, 
    vector<vector <T1> > &dataset, T2 num_cluster, T2 seed, string init_type);
    
    void get_ranodm_indices(int *arr, int size, int seed);

    template <typename T1, typename T2>
    float get_sse(vector<vector <T1> > &dataset, vector<vector <T1> > &centroids, vector<vector<T1> > &cluster_size, 
    vector<T2> assigned_clusters, T2 num_cluster);

    template <typename T1>
    float calc_euclidean(const vector<T1> &, const vector<T1> &, unsigned long long int &he_counter);

    template <typename T1>
    inline float calc_squared_dist(const vector<T1> &point, const vector<T1> &center); 
    
};


template <typename T1>
void algorithm_utils::reinit(vector<vector<T1> > &container){

    for(int i=0;i<container.size(); i++){
        container[i].assign(container[i].size(), 0);
    }
}

// The following code fragmemt is taken from the following question thread.
// https://stackoverflow.com/questions/20734774/random-array-generation-with-no-duplicates
void shuffle(int *arr, size_t size, int seed)
{
    if (size > 1) 
    {
        size_t i;
        srand(seed);
        for (i = 0; i < size - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (size - i) + 1);
          int t = arr[j];
          arr[j] = arr[i];
          arr[i] = t;
        }
    }
}


// The following function is taken from the following question thread.
// https://stackoverflow.com/questions/20734774/random-array-generation-with-no-duplicates
void algorithm_utils::get_ranodm_indices(int *arr, int size, int seed)
{
    if (size > 1) 
    {
        int i = 0, j = 0, t = 0;
        srand(seed);
        
        for (i = 0; i < size - 1; i++) 
        {
          j = i + rand() / (RAND_MAX / (size - i) + 1);
          t = arr[j];
          arr[j] = arr[i];
          arr[i] = t;
        }
    }
}


template <typename T1, typename T2>
void algorithm_utils::init_centroids(vector<vector <T1> > &centroids, 
vector<vector <T1> > &dataset, T2 num_cluster, T2 seed, string init_type){

    int i = 0, j = 0, size = dataset.size();
    
    if (init_type == "random"){

        int test_array[size];

        for (i = 0; i<size ; i++){
            test_array[i] = i;
        }

        shuffle(test_array, size, seed);

        for(i=0; i<num_cluster; i++){  
            for(j=0; j <dataset[i].size(); j++){
                centroids[i][j] = dataset[test_array[i]][j];
            }   
        }
    }
    else if (init_type == "sequential"){
        for(i=0; i<num_cluster; i++){  
            for(j=0; j<dataset[0].size(); j++){
                centroids[i][j] = dataset[i][j];
            }   
        }
    }
    else{
        read_kplus_plus_centroids(init_type, centroids, num_cluster);
    }
}


template <typename T1, typename T2>
void algorithm_utils::extract_data(vector<vector <T1> > &dataset, vector<vector <T1> > &extracted_data, 
T2 num_points, T2 seed){

    int i = 0, j = 0, size = dataset.size();
    int test_array[size];

    for (i = 0; i<size ; i++){
        test_array[i] = i;
    }

    get_ranodm_indices(test_array, size, seed);
    
    for(i=0; i < num_points; i++){ 
        for(j=0; j<dataset[0].size(); j++){
            extracted_data[i][j] = dataset[test_array[i]][j];
        }   
    }
}


template <typename T1>
inline float algorithm_utils::calc_euclidean(const vector<T1> &point, 
const vector<T1> &center, unsigned long long int &he_counter){
    
    T1 dist = 0.0;
    T1 temp = 0.0;
    
    for (int i=0; i < point.size(); i++){
        temp = point[i] - center[i];
        dist = dist + (temp*temp);
    }
    
    he_counter = he_counter + 1;
    dist = sqrt(dist);
    return dist;
}


template <typename T1>
inline float calc_squared_dist(const vector<T1> &point, 
const vector<T1> &center){
    
    T1 dist = 0.0;
    T1 temp = 0.0;
    
    // cout << point.size() << "\n";
    for (int i=0; i < point.size(); i++){
        temp = point[i] - center[i];
        dist = dist + (temp*temp);
    }
    return dist;
}


template <typename T1, typename T2>
float get_sse(vector<vector <T1> > &dataset, vector<vector <T1> > &centroids, vector<vector<T1> > &cluster_size, 
vector<T2> assigned_clusters, T2 num_cluster){

float total_sse = 0;
vector<float> sse_vec(num_cluster, 0);
int i = 0;

for (i = 0; i<dataset.size(); i++){
    sse_vec[assigned_clusters[i]] += calc_squared_dist(dataset[i], centroids[assigned_clusters[i]]);
}

for(i = 0; i< num_cluster;i++){
    sse_vec[i] /= cluster_size[i][0];
    total_sse += sse_vec[i];
}

return total_sse;

}


template <typename T1, typename T2>
inline void algorithm_utils::calculate_distances(const vector<vector<T1> > &dataset, 
vector<vector<T1> > &centroids, vector<vector<T1> > &dist_mat,
T2 num_clusters, vector<T2> &assigned_clusters, vector<vector<T1> > &cluster_size, 
unsigned long long int &he_counter){

    T2 current_center = 0;
    vector<T1> temp_dist (num_clusters);
    float temp = 0.0;
    float shortestDist2 = 0.0;
    int i =0, j =0;

    assigned_clusters.assign(assigned_clusters.size(), 0);
    algorithm_utils::reinit(cluster_size);

    // Calculate the distance of points to nearest center
    for (i=0; i < dataset.size(); i++){

        shortestDist2 = std::numeric_limits<float>::max();
        
        for (j=0; j < centroids.size(); j++){ 
            
            temp = calc_euclidean(dataset[i], centroids[j], he_counter);
            temp_dist[j] = temp;

            // cout << "hello" << "n";
            
            if (temp < shortestDist2){
                shortestDist2 = temp;
                current_center = j;
            }
        }
        
        dist_mat[i] = temp_dist;
        assigned_clusters[i] = current_center;

        // Increase the size of the cluster
        cluster_size[current_center][0] = cluster_size[current_center][0] + 1;
        
        // Store the max so far
        if (shortestDist2 > cluster_size[current_center][1])
            cluster_size[current_center][1] = shortestDist2;
    }
}


template <typename T1, typename T2>
inline void algorithm_utils::update_centroids(vector<vector <T1> > &dataset, 
vector<vector<T1> > &new_centroids, vector<T2> &assigned_clusters, 
vector<vector<T1> > &cluster_size, T2 numCols){

    int curr_center = 0, index = 0, k = 0, j =0;

    for (index=0; index<dataset.size(); index++){
        curr_center = assigned_clusters[index];
        
        for (j = 0; j<numCols; j++){
            new_centroids[curr_center][j] = new_centroids[curr_center][j] + dataset[index][j];
        }
    }

    for(int i=0; i<new_centroids.size();i++){
        k = cluster_size[i][0];

        for (j = 0; j < new_centroids[i].size(); j++){
            if (k > 0)
                new_centroids[i][j] = new_centroids[i][j]/k;
            // else
            //     new_centroids[i][j] = 0.0;
        }
    } 

}


template <typename T1>
inline bool algorithm_utils::check_convergence(vector<vector <T1> > &new_centroids, 
vector<vector <T1> > &centroids, T1 threshold, float &diff, float &temp_diff, int &i, int &j){

    temp_diff = 0;
    diff = 0;

    if (new_centroids.size() == centroids.size()){
        
        for (i=0; i<new_centroids.size(); i++){
            for (j=0; j< new_centroids[i].size(); j++)
                temp_diff = new_centroids[i][j] - centroids[i][j];
                diff = diff + (temp_diff * temp_diff);
        }
        diff = sqrt(diff/new_centroids.size());
    }
    else
        return false;

    if (diff <= threshold)
        return true;
    return false;
}