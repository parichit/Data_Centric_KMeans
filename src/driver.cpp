// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering

#include <iostream>
#include <string>
#include <tuple>
#include "data_holder.hpp"
#include "IOutils.hpp"
#include "algo_utils.hpp"
#include "kmeans.hpp"
#include "dckmeans.hpp"
#include "ball_kmeans++_xf.hpp"
#include <chrono>
#include <filesystem>

using namespace std;


// Path to the folder containing the data. In this case, the CSV files for each dataset are present 
// inside the Data folder.
string basePath = "../Data/";


int main(){

    std::vector<vector <float> > dataset;
    vector<int> labels;
    string someFilepath = "";

    // CSV Files containing the data
    vector<string> file_list = {"Breastcancer.csv", "CreditRisk.csv",
            "census.csv", "crop.csv", "birch.csv"};
    
    
    // Declare variables
    // Max iterations (chnage this to increase or decrease the iterations)
    int num_iterations = 100;

    // Convergence threshold
    float threshold = 0.001;

    // Change the following for number of clusters (k) 
    int num_clusters = 20;

    // Random seed for reproducing the results
    int seed = 5;

    output_data res;
     
    // Run the algorithms
    // The algorithms are called in the following sequence: Kmeans --> Kmeans-d --> and Ball Kmeans
    // For running specific algorithm, comment out the set of statements for specific algorithm

    
    for (int i =0; i<file_list.size(); i++){ 
        

        someFilepath = someFilepath + file_list[i];

        cout << "Reading the input from: " << someFilepath << endl;

        // Read in the data
        auto t1 = std::chrono::high_resolution_clock::now();
        std::pair<int, int> p = readSimulatedData(someFilepath, dataset, labels, false, false);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto file_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        // Load data in Eigen format for Ball KMeans
        MatrixOur BallK_dataset = load_data(someFilepath);

        
        cout << "File reading: " << file_int.count() << " MS\n";
        cout << "Data size: " << dataset.size() << " X " << dataset[0].size() << endl;
    
        int numRows = p.first;
        int numCols = p.second;



        // Following lines will call the kmeans implementation
        cout << "\nAlgo: KMeans," << " Clusters: " << num_clusters << ", Threshold: " << threshold << endl;
        
        auto t3 = std::chrono::high_resolution_clock::now();
        res = kmeans(dataset, num_clusters, threshold, num_iterations, numCols, 60000, "random", seed+num_clusters);
        auto t4 = std::chrono::high_resolution_clock::now();
        auto km_int = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3);
        
        std::cout << "Total KMeans time: " << km_int.count() << " milliseconds\n";


        // Following lines will call the kmeans-d (kmeans data centric) implementation
        cout << "\nAlgo: DCKM," << " Clusters: " << num_clusters << ", Threshold: " << threshold << endl;
        
        auto t5 = std::chrono::high_resolution_clock::now();
        res = dckmeans(dataset, num_clusters, threshold, num_iterations, numCols, 60000, "random", seed+num_clusters);
        auto t6 = std::chrono::high_resolution_clock::now();
        auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5);
        
        std::cout << "Total Kmeans-d time: " << ms_int2.count() << " milliseconds\n";

    
        
        // Following lines will call the Ball Kmeans implementation
        cout << "\nAlgo: Ball Kmeans," << " Clusters: " << num_clusters << ", Threshold: " << threshold << endl;
        
        // Set timeout limit of 40 minutes to prevent very long runtime
        int time_limit = 2400000;
        
        auto t7 = std::chrono::high_resolution_clock::now();
        res = ball_k_means_Ring(BallK_dataset, false, num_clusters, threshold, num_iterations, time_limit, 
                        "random", seed+num_clusters);
        auto t8 = std::chrono::high_resolution_clock::now();
        auto ms_int3 = std::chrono::duration_cast<std::chrono::milliseconds>(t8 - t7);
        
        std::cout << "Total Ball Kmeans time: " << ms_int3.count() << " milliseconds\n";


        cout << "\n#################################"
        cout << "\n#################################"

    }

    return 0;
}
