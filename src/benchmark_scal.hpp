// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering

#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
using namespace std;

void benchmark_scal(string basePath){

        string scal_input_path = basePath;    
        string scal_output_path = basePath + "benchmark_scal.csv";;
        
        // Declare variables
        vector<string> scal_file_list = {"1000000_points.csv", "3000000_points.csv" , "5000000_points.csv", "8000000_points.csv"};
        vector<int> num_points = {1000000, 3000000, 5000000, 8000000};

        // vector<string> scal_file_list = {"3000000_points.csv", "5000000_points.csv"};
        // vector<int> num_points = {3000000, 5000000};

        vector<int> labels;
        int num_iterations = 2000;
        float threshold = 0.01;
        int num_clusters = 10;
        int seed = 90;

        // 90 minutes cutoff for runtime
        int time_limit = 5400000, points = 0;
        
        string inputfilePath = "";
        bool run_stat = false;
        string kmdc_timeout = "no", ballkm_timeout = "no";

        float kmdc_time = 0, bkm_time = 0;
        
        output_data kmdc_res;
        output_data ballkm_res;
        
        ofstream scaloutFile;
        scaloutFile.open(scal_output_path, ios::trunc);
        scaloutFile << "Algorithm,Num_Points,Runtime,Runtime_per_Iter,Runtime_speedup,Distances,Dist_speed_up,Timeout";
        scaloutFile.close();

        algorithm_utils alg_utils;

        ////////////////////////////
        // Scalability experiments
        ////////////////////////////
        
        for(int i = 0; i < num_points.size(); i++){

            points = num_points[i];
            inputfilePath = scal_input_path + scal_file_list[i];
            
            cout << "\n%%%%%%%%%%%%%%%%%%%%%%%" << endl;
            cout << "Processing " << inputfilePath << endl;
            cout << "%%%%%%%%%%%%%%%%%%%%%%%\n" << endl;
            
            vector<vector <float> > dataset;
            
            std::pair<int, int> p = readSimulatedData(inputfilePath, dataset, labels, false, false);
            int numRows = p.first;
            int numCols = p.second;
            
            // Load data in Eigen format for Ball KMeans
            MatrixOur BallK_dataset = load_data(inputfilePath);

            kmdc_timeout = "no";
            ballkm_timeout = "no";
            
            //####################
            // KMeans-DataCentric
            //####################

            cout << "Algo: Kmeans-DataCentric" << endl;

            auto kmdc_start_time = std::chrono::high_resolution_clock::now();

            kmdc_res = dckmeans(dataset, num_clusters, threshold, num_iterations, numCols, time_limit, "sequential", 0);

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
                        "sequential", 0);

            auto bkm_end_time = std::chrono::high_resolution_clock::now();
            bkm_time = std::chrono::duration_cast<std::chrono::seconds>(bkm_end_time - bkm_start_time).count();

            if (ballkm_res.timeout == true){
                ballkm_timeout = "yes";
                cout << "Timeout: BallKmeans time: " << bkm_time << " seconds" << endl;
            }
            else{
                cout << "Total BallKmeans time: " << bkm_time << " seconds" << endl;
            }

            scaloutFile.open(scal_output_path, ios::app);

            scaloutFile << "\nKmeans-DataCentric" << "," << to_string(points) 
            <<  "," <<  std::setprecision(2) << to_string(kmdc_time) << "," << std::setprecision(3) <<
            to_string((float)kmdc_time/kmdc_res.loop_counter) << "," << std::setprecision(3) 
            << to_string((float)bkm_time/kmdc_time) << "," << to_string(kmdc_res.num_he) <<
            "," << std::setprecision(3) << to_string((float)ballkm_res.num_he/kmdc_res.num_he) << "," << kmdc_timeout;

            scaloutFile << "\nBall-Kmeans" << "," << to_string(points) 
            << "," <<  std::setprecision(2) << to_string(bkm_time) << "," << std::setprecision(3) <<
            to_string((float)bkm_time/ballkm_res.loop_counter) << "," << to_string(1)
            << "," << to_string(ballkm_res.num_he) <<
            "," << to_string(1) << "," << ballkm_timeout;    

            scaloutFile.close();
        }
        
    cout << "Completed synthetic data scalability benchmarks" << endl;
}
