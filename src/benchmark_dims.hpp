// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering


#include <iostream>
#include <string>
#include <tuple>
#include <iomanip>
#include <chrono>

using namespace std;


void benchmark_dims(string basePath){

       string dims_input_path = basePath;    
       string dims_output_path = basePath;
       
       // Declare variables
        vector<string> dims_file_list = {"200_dims.csv" , "300_dims.csv", "400_dims.csv", "500_dims.csv", "600_dims.csv"};
        vector<int> num_dims = {200, 300, 400, 500, 600};

        // vector<string> dims_file_list = {"200_dims.csv", "300_dims.csv"};
        // vector<int> num_dims = {200, 300};
       
       vector<int> labels;

        int num_iterations = 2000;
        float threshold = 0.01;
        int num_clusters = 10;
        int seed = 15;
        int num_rep = 10;
        
        // 50 minutes cutoff for running
        int time_limit = 3000000, dims = 0;

        unsigned long long int avg_kmdc_num_he = 0, avg_bkm_num_he =0; 
        int avg_kmdc_loop_counter = 0, avg_bkm_loop_counter = 0;

        float kmdc_time = 0, bkm_time = 0, avg_kmdc_runtime = 0, avg_bkm_runtime = 0;
        string kmdc_timeout = "no", ballkm_timeout = "no";
            
        string inputfilePath = "";
        
        output_data kmdc_res;
        output_data ballkm_res;

        ofstream allResOutFile, avgResOutFile;
        string alloutFile = dims_output_path + "benchmark_dims_all_runs.csv" ;
        string avgoutFile = dims_output_path + "benchmark_dims_avg_runs.csv" ;
        
        allResOutFile.open(alloutFile, ios::trunc);
        allResOutFile << "Algorithm,Dims,Runtime,Runtime_per_Iter,Runtime_speedup,Distances,Dist_speed_up,Timeout";
        allResOutFile.close();

        avgResOutFile.open(avgoutFile, ios::trunc);
        avgResOutFile << "Algorithm,Dims,Runtime,Runtime_per_Iter,Runtime_speedup,Distances,Dist_speed_up,Timeout";
        avgResOutFile.close();

        algorithm_utils alg_utils;

        ////////////////////////////
        // Dimensionality experiments
        ////////////////////////////

        for(int i = 0; i < num_dims.size(); i++){

            dims = num_dims[i];
            inputfilePath = dims_input_path + dims_file_list[i];
            
            cout << "\n%%%%%%%%%%%%%%%%%%%%%%%" << endl;
            cout << "Processing " << dims_file_list[i] << endl;
            cout << "%%%%%%%%%%%%%%%%%%%%%%%\n" << endl;
            
            vector<vector <float> > dataset;
            
            std::pair<int, int> p = readSimulatedData(inputfilePath, dataset, labels, false, false);
            int numRows = p.first;
            int numCols = p.second;

            // Load data in Eigen format for Ball KMeans
            MatrixOur BallK_dataset = load_data(inputfilePath);

            avg_kmdc_loop_counter = 0;
            avg_bkm_loop_counter = 0;

            avg_kmdc_runtime = 0;
            avg_bkm_runtime = 0;

            avg_kmdc_num_he = 0;
            avg_bkm_num_he = 0;

        
            for(int k = 0; k<num_rep; k++){ 

                kmdc_timeout = "no";
                ballkm_timeout = "no";           
                
                //####################
                // KMeans-DataCentric
                //####################

                cout << "Algo: Kmeans-DataCentric" << endl; 

                auto kmdc_start_time = std::chrono::high_resolution_clock::now();

                kmdc_res = dckmeans(dataset, num_clusters, threshold, num_iterations, numCols, time_limit, "random", seed+k);

                auto kmdc_end_time = std::chrono::high_resolution_clock::now();
                kmdc_time = std::chrono::duration_cast<std::chrono::seconds>(kmdc_end_time - kmdc_start_time).count();

                if (kmdc_res.timeout == true){
                    kmdc_timeout = "yes";
                    cout << "Timeout: Kmeans-DataCentric time: " << kmdc_time << " seconds" << endl;
                }
                else{
                    cout << "Total Kmeans-DataCentric time: " << kmdc_time << " seconds" << endl;
                }

                //####################
                // Ball-KMeans
                //####################

                cout << "Algo: Ball-KMeans" << endl; 
                
                auto bkm_start_time = std::chrono::high_resolution_clock::now();
            
                ballkm_res = ball_k_means_Ring(BallK_dataset, false, num_clusters, threshold, num_iterations, time_limit, 
                            "random", seed+k);

                auto bkm_end_time = std::chrono::high_resolution_clock::now();
                bkm_time = std::chrono::duration_cast<std::chrono::seconds>(bkm_end_time - bkm_start_time).count();

                if (ballkm_res.timeout == true){
                    ballkm_timeout = "yes";
                    cout << "Timeout: BallKmeans time: " << bkm_time << " seconds" << endl;
                }
                else{
                    cout << "Total BallKmeans time: " << bkm_time << " seconds" << endl;
                }

                allResOutFile.open(alloutFile, ios::app);

                allResOutFile << "\nKmeans-DataCentric" << "," << to_string(dims) 
                << "," << std::setprecision(2) << to_string(kmdc_res.loop_counter) <<  "," << 
                std::setprecision(2) << to_string(kmdc_time) << "," << std::setprecision(3) <<
                to_string(kmdc_time/kmdc_res.loop_counter) << "," << std::setprecision(3) << to_string(bkm_time/kmdc_time)
                    << "," << std::setprecision(2) << to_string(kmdc_res.num_he) <<
                "," << std::setprecision(2) << to_string((float)ballkm_res.num_he/kmdc_res.num_he) << "," << kmdc_timeout;

                allResOutFile << "\nBall-Kmeans" << "," << to_string(dims) 
                << "," << std::setprecision(2) << to_string(ballkm_res.loop_counter) <<  "," << 
                std::setprecision(2) << to_string(bkm_time) << "," << std::setprecision(3) <<
                to_string(bkm_time/ballkm_res.loop_counter) << "," << std::setprecision(2) << to_string(1)
                    << "," << std::setprecision(2) << to_string(ballkm_res.num_he) <<
                "," << std::setprecision(2) << to_string(1) << "," << ballkm_timeout;    

                allResOutFile.close();

                avg_kmdc_loop_counter = avg_kmdc_loop_counter + kmdc_res.loop_counter;
                avg_kmdc_num_he = avg_kmdc_num_he + kmdc_res.num_he;
                avg_kmdc_runtime = avg_kmdc_runtime + kmdc_time;

                avg_bkm_loop_counter = avg_bkm_loop_counter + ballkm_res.loop_counter;
                avg_bkm_num_he = avg_bkm_num_he + ballkm_res.num_he;
                avg_bkm_runtime = avg_bkm_runtime + bkm_time;
            }

            avg_kmdc_loop_counter = (float)avg_kmdc_loop_counter/num_rep;
            avg_kmdc_num_he = (float) avg_kmdc_num_he/num_rep;
            avg_kmdc_runtime = (float)avg_kmdc_runtime/num_rep;

            avg_bkm_loop_counter = (float)avg_bkm_loop_counter/num_rep;
            avg_bkm_num_he = (float)avg_bkm_num_he/num_rep;
            avg_bkm_runtime = (float)avg_bkm_runtime/num_rep;

            avgResOutFile.open(avgoutFile, ios::app);

            avgResOutFile << "\nKmeans-DataCentric" << "," << to_string(dims) 
            << "," << std::setprecision(3) << to_string(avg_kmdc_runtime) << "," << std::setprecision(3) <<
            to_string((float)avg_kmdc_runtime/avg_kmdc_loop_counter) << "," << std::setprecision(3) << to_string(bkm_time/kmdc_time)
            << "," << std::setprecision(2) << to_string(avg_kmdc_num_he) <<
            "," << std::setprecision(3) << to_string((float)avg_kmdc_num_he/avg_bkm_num_he) << "," << kmdc_timeout;

            avgResOutFile << "\nBall-Kmeans" << "," << to_string(dims) 
            <<  "," << std::setprecision(3) << to_string(avg_bkm_runtime) << "," << std::setprecision(3) <<
            to_string((float)avg_bkm_runtime/avg_bkm_loop_counter) << "," << to_string(1)
            << "," << to_string(avg_bkm_num_he) << ","
            << to_string(1) << "," << ballkm_timeout;

            avgResOutFile.close();

        }

    cout << "Completed synthetic data dimensionality benchmarks" << endl;
}