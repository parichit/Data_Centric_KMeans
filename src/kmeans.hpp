#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

class conv_kmeans{
    template <typename Tfloat, typename Tint>
    output_data kmeans(vector<vector <Tfloat> > &dataset,
    Tint num_clusters, Tfloat threshold, Tint num_iterations, Tint numCols, Tint time_limit, 
    string init_type, Tint seed);

};

template <typename Tfloat, typename Tint>
inline output_data kmeans(vector<vector <Tfloat> > &dataset,
Tint num_clusters, Tfloat threshold, Tint num_iterations, Tint numCols, Tint time_limit, 
string init_type, Tint seed){

    Tint loop_counter = 0;
    vector<vector<Tfloat> > centroids(num_clusters, vector<Tfloat>(numCols, 0));
    vector<vector<Tfloat> > new_centroids(num_clusters, vector<Tfloat>(numCols, 0));
    vector<vector <Tfloat> > dist_matrix(dataset.size(), vector<Tfloat>(num_clusters));
    vector<vector<Tfloat> > cluster_size(num_clusters, vector<Tfloat>(2));  

    vector<Tint> assigned_clusters(dataset.size());
 
    // Create objects
    algorithm_utils alg_utils;
    print_utils pu;

    int i =0, j = 0;
    float temp_diff =0, diff = 0;
    output_data result;

    unsigned long long int he_counter = 0;

    // Start time counter 
    auto start = std::chrono::high_resolution_clock::now();
    
    // Initialize centroids
    alg_utils.init_centroids(centroids, dataset, num_clusters, seed, init_type);

    // print_2d_vector(centroids, 2, "KM centroids");

    alg_utils.calculate_distances(dataset, centroids, dist_matrix, 
    num_clusters, assigned_clusters, cluster_size, he_counter);
   
    while (loop_counter < num_iterations){

        loop_counter++;

        // Calculate new centroids
        alg_utils.update_centroids(dataset, new_centroids, assigned_clusters, 
        cluster_size, numCols);
        

        // Check Convergence
        if (alg_utils.check_convergence(new_centroids, centroids, threshold, diff, temp_diff, i, j)){
                // cout << "Convergence at iteration: " << loop_counter << "\n";
                break;
        }

        // Re-calculate distances
        alg_utils.calculate_distances(dataset, new_centroids, dist_matrix,
                                    num_clusters, assigned_clusters, cluster_size, he_counter);

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
            result.sse = get_sse(dataset, new_centroids, cluster_size, assigned_clusters, num_clusters);
            cout << "Kmeans Timed Out :(" << endl;
            return result;
        }
        
    }

    result.loop_counter = loop_counter;
    result.num_he = he_counter;
    result.centroids = new_centroids;
    result.sse = get_sse(dataset, new_centroids, cluster_size, assigned_clusters, num_clusters);

    return result;

}