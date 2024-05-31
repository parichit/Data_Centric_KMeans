// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering

#include <iostream>
#include <vector>
#include <map>
#include "dckm_utils.hpp"
#include <chrono>
using namespace std;

class DataCentricKmeans{
    template <typename Tfloat, typename Tint>
    output_data dckmeans(vector<vector <Tfloat> > &dataset,
    Tint num_clusters, Tfloat threshold, Tint num_iterations, 
    Tint numCols, Tint time_limit, string init_type, Tint seed);
};


template <typename Tfloat, typename Tint>
output_data dckmeans(vector<vector <Tfloat> > &dataset, Tint num_clusters, 
Tfloat threshold, Tint num_iterations, Tint numCols, Tint time_limit, 
string init_type, Tint seed){

    Tint loop_counter = 0;
    vector<vector<Tfloat> > centroids(num_clusters, vector<Tfloat>(numCols, 0));
    vector<vector<Tfloat> > new_centroids(num_clusters, vector<Tfloat>(numCols, 0));
    vector<vector<Tfloat> > dist_matrix(dataset.size(), vector<Tfloat>(num_clusters));
    vector<Tint> assigned_clusters(dataset.size());
    
    vector<vector<Tfloat> > cluster_size(num_clusters, vector<Tfloat>(3));  
    
    vector<vector <Tfloat> > center_dist_mat (num_clusters, vector<Tfloat>(num_clusters, 0));
    vector<vector<int> > neighbors(num_clusters);
    
    vector<vector<vector <Tfloat> > > mid_points(num_clusters, vector<vector<Tfloat> >(num_clusters, vector<Tfloat>(numCols, 0)));
    vector<vector<vector <Tfloat> > > affine_vectors(num_clusters, vector<vector<Tfloat> >(num_clusters, vector<Tfloat>(numCols, 0)));
    
    vector<Tint> temp1;
    vector<Tfloat> temp2(3);
    vector<vector<Tfloat> > temp_master;
    vector<Tfloat> temp_midpoint(numCols);
    vector<Tfloat> temp_affine(numCols);

    vector<vector<Tfloat> > midpoint_holder;
    vector<vector<Tfloat> > affine_holder;

    Tint my_cluster = 0, i = 0, j = 0, k = 0, l = 0, m = 0;
    Tfloat temp_diff = 0, diff = 0, vec_sum = 0;

    unsigned long long int he_counter = 0;

    output_data result;

    // Create objects
    algorithm_utils alg_utils;
    dckm_utils dc_utils;

    // Start time counter 
    auto start = std::chrono::high_resolution_clock::now();

    // Initialize centroids
    alg_utils.init_centroids(centroids, dataset, num_clusters, seed, init_type);

    // print_2d_vector(centroids, 2, "DCKM centroids");

    // Assign data to nearest center
    alg_utils.calculate_distances(dataset, centroids, dist_matrix,
    num_clusters, assigned_clusters, cluster_size, he_counter);

    while (loop_counter < num_iterations){

        loop_counter++;
        alg_utils.update_centroids(dataset, new_centroids, assigned_clusters, cluster_size, numCols);

        // Check Convergence
        if (alg_utils.check_convergence(new_centroids, centroids, threshold, diff, temp_diff, i, j)){
                cout << "Convergence at iteration: " << loop_counter << "\n";
                break;
        }
        
        find_neighbors(new_centroids, center_dist_mat, cluster_size, neighbors, 
        mid_points, affine_vectors, temp2, temp_master, temp_midpoint, temp_affine, 
        midpoint_holder, affine_holder, he_counter);
        
        determine_data_expression(dataset, new_centroids, cluster_size, center_dist_mat, dist_matrix,
        assigned_clusters, neighbors, affine_vectors, mid_points, 
        he_counter, temp1, my_cluster, i, j, vec_sum);

        // Move the new centroids to older
        centroids = new_centroids;
        
        // reset centroids
        alg_utils.reinit(new_centroids);

        auto temp_end = std::chrono::high_resolution_clock::now();
        auto temptime = std::chrono::duration_cast<std::chrono::milliseconds>(temp_end - start);

        if (temptime.count() >= time_limit){
            result.loop_counter = loop_counter;
            result.num_he = he_counter;
            result.timeout = true;
            result.centroids = new_centroids;
            result.sse = get_sse(dataset, new_centroids, cluster_size, assigned_clusters, num_clusters);
            cout << "DCKmeans Timed Out :(" << endl;
            return result;
        }

    }

    result.loop_counter = loop_counter;
    result.num_he = he_counter;
    result.centroids = new_centroids;
    result.sse = get_sse(dataset, new_centroids, cluster_size, assigned_clusters, num_clusters);

    return result;
}