// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering


#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>

using namespace std;

void benchmark_clus(string basePath){

        string clus_input_path = basePath; 
        string clus_output_path = basePath;
        
        // Declare variables
        int num_iterations = 2000, clus = 0;
        float threshold = 0.01;
        vector<int> num_clusters = {20, 40, 60, 80, 100};
        // vector<int> num_clusters = {20, 100};
        int seed = 12;
        int num_rep = 10; 

        unsigned long long int avg_kmdc_num_he = 0, avg_bkm_num_he =0; 
        int avg_kmdc_loop_counter = 0, avg_bkm_loop_counter = 0;

        float kmdc_time = 0, bkm_time = 0, avg_kmdc_runtime = 0, avg_bkm_runtime = 0;

        // 60 minutes cutoff for running
        int time_limit = 3600000;
        vector<int> labels;
            
        string inputfilePath = "";
        string kmdc_timeout = "no", ballkm_timeout = "no";

        output_data kmdc_res;
        output_data ballkm_res;

        ofstream allResOutFile, avgResOutFile;
        string alloutFile = clus_output_path + "benchmark_clus_all_runs.csv" ;
        string avgoutFile = clus_output_path + "benchmark_clus_avg_runs.csv" ;
        
        allResOutFile.open(alloutFile, ios::trunc);
        allResOutFile << "Algorithm,Clusters,Runtime,Runtime_per_Iter,Runtime_speedup,Distances,Dist_speed_up,Timeout";
        allResOutFile.close();

        avgResOutFile.open(avgoutFile, ios::trunc);
        avgResOutFile << "Algorithm,Clusters,Runtime,Runtime_per_Iter,Runtime_speedup,Distances,Dist_speed_up,Timeout";
        avgResOutFile.close();

        algorithm_utils alg_utils;

        inputfilePath = clus_input_path + "100_clusters.csv";
            
        cout << "\n%%%%%%%%%%%%%%%%%%%%%%%" << endl;
        cout << "Processing " << inputfilePath << endl;
        cout << "%%%%%%%%%%%%%%%%%%%%%%%\n" << endl;
        
        vector<vector <float> > dataset;
        
        std::pair<int, int> p = readSimulatedData(inputfilePath, dataset, labels, false, false);
        int numRows = p.first;
        int numCols = p.second;

        // Load data in Eigen format for Ball KMeans
        MatrixOur BallK_dataset = load_data(inputfilePath);

        
        //////////////////////////
        // Clustering experiments
        //////////////////////////
        
        for(int i = 0; i < num_clusters.size(); i++){

            clus = num_clusters[i];
            avg_kmdc_loop_counter = 0;
            avg_bkm_loop_counter = 0;

            avg_kmdc_runtime = 0;
            avg_bkm_runtime = 0;

            avg_kmdc_num_he = 0;
            avg_bkm_num_he = 0;

            for (int k=0; k < num_rep; k++){

                kmdc_timeout = "no";
                ballkm_timeout = "no";

                //####################
                // KMeans-DataCentric
                //####################

                cout << "Algo: Kmeans-DataCentric" << endl; 

                auto kmdc_start_time = std::chrono::high_resolution_clock::now();

                kmdc_res = dckmeans(dataset, clus, threshold, num_iterations, numCols, time_limit, "random", seed+k);

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

                cout << "Algo: Ball-Kmeans" << endl; 

                auto bkm_start_time = std::chrono::high_resolution_clock::now();
                
                ballkm_res = ball_k_means_Ring(BallK_dataset, true, clus, threshold, num_iterations, time_limit, 
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

                allResOutFile << "\nKmeans-DataCentric" << "," << to_string(clus) 
                << "," << std::setprecision(2) << to_string(kmdc_time) << "," << std::setprecision(3) <<
                to_string((float)kmdc_time/kmdc_res.loop_counter) << "," << std::setprecision(3) << 
                to_string((float)bkm_time/kmdc_time) << "," << to_string(kmdc_res.num_he) <<
                "," << std::setprecision(3) << 
                to_string((float)ballkm_res.num_he/kmdc_res.num_he) << "," << kmdc_timeout;

                allResOutFile << "\nBall-Kmeans" << "," << to_string(clus) 
                << "," << std::setprecision(2) << to_string(bkm_time) << "," << std::setprecision(3) <<
                to_string((float)bkm_time/ballkm_res.loop_counter) << "," << to_string(1)
                << "," << to_string(ballkm_res.num_he) <<
                "," << to_string(1) << "," << ballkm_timeout;    

                allResOutFile.close();

                avg_kmdc_loop_counter = avg_kmdc_loop_counter + kmdc_res.loop_counter;
                avg_kmdc_num_he = avg_kmdc_num_he + kmdc_res.num_he;
                avg_kmdc_runtime = avg_kmdc_runtime + kmdc_time;

                avg_bkm_loop_counter = avg_bkm_loop_counter + ballkm_res.loop_counter;
                avg_bkm_num_he = avg_bkm_num_he + ballkm_res.num_he;
                avg_bkm_runtime = avg_bkm_runtime + bkm_time;
            }
            

            avg_kmdc_loop_counter = (float) avg_kmdc_loop_counter/num_rep;
            avg_kmdc_num_he = (float) avg_kmdc_num_he/num_rep;
            avg_kmdc_runtime = (float) avg_kmdc_runtime/num_rep;

            avg_bkm_loop_counter = (float) avg_bkm_loop_counter/num_rep;
            avg_bkm_num_he = (float) avg_bkm_num_he/num_rep;
            avg_bkm_runtime = (float) avg_bkm_runtime/num_rep;

            cout << avg_kmdc_runtime << "\t" << avg_bkm_runtime << endl;

            avgResOutFile.open(avgoutFile, ios::app);

            avgResOutFile << "\nKmeans-DataCentric" << "," << to_string(clus) 
            <<  "," << std::setprecision(2) << to_string(avg_kmdc_runtime) << "," << std::setprecision(3) <<
            to_string(avg_kmdc_runtime/avg_kmdc_loop_counter) << "," << std::setprecision(3) << 
            to_string(avg_bkm_runtime/avg_kmdc_runtime) << "," << to_string(avg_kmdc_num_he) 
            <<  "," << std::setprecision(3) << to_string((float)avg_bkm_num_he/avg_kmdc_num_he) << "," << kmdc_timeout;

            avgResOutFile << "\nBall-Kmeans" << "," << to_string(clus) 
            <<  "," << std::setprecision(2) << to_string(avg_bkm_runtime) << "," << std::setprecision(3) <<
            to_string(avg_bkm_runtime/avg_bkm_loop_counter) << "," << 
            to_string(1) << "," << to_string(avg_bkm_num_he) <<  "," 
            << to_string(1) << "," << ballkm_timeout;

            avgResOutFile.close();

        }

    cout << "Completed synthetic data clustering benchmarks" << endl;
 }