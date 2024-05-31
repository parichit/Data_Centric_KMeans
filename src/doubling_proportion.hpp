// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering


#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace std;


void double_prop(string basePath){

       string input_path = basePath;     
       string out_path = basePath;
       
       // Declare variable
       vector<string> file_list = {"Breastcancer.csv", "CreditRisk.csv", "census.csv", "crop.csv", "Twitter.csv", "birch.csv"};
       vector<string> data_list = {"Breastcancer", "CreditRisk", "Census", "Crop", "Twitter", "Birch"};

    //    vector<string> file_list = {"Breastcancer.csv", "CreditRisk.csv"};
    //    vector<string> data_list = {"Breastcancer", "CreditRisk"};

        int num_iterations = 2000;
        float threshold = 0.001;
        // vector<float> data_prop = {0.2, 0.4};  
        vector<float> data_prop = {0.2, 0.4, 0.8, 1.0};
        
       string inputfilePath = "", centroidFilePath = "";
       bool run_stat = false;
       
       vector<int> labels;
       output_data km_res;
       output_data kmdc_res;
       output_data ballkm_res;
       
       // Timeout limit
       int time_limit = 600000;
       
       //initial seed for replication (due to random data selection)
       int seed = 78;
       int num_clusters = 5;
       int num_points = 0;
       vector<int> rep = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
       int num_rep = 10;

       unsigned long long int avg_km_num_he = 0, avg_kmdc_num_he = 0, avg_bkm_num_he =0; 
       int avg_km_loop_counter = 0, avg_kmdc_loop_counter = 0, avg_bkm_loop_counter = 0;

       float km_time = 0, kmdc_time = 0, bkm_time = 0, avg_km_runtime = 0, 
       avg_kmdc_runtime = 0, avg_bkm_runtime = 0;

       string km_timeout = "no", kmdc_timeout = "no", ballkm_timeout = "no";

       ofstream avgresFile, allresFile;
       
       string avgoutFile = out_path + "doubling_proportion_avg.csv";
       string alloutFile = out_path + "doubling_proportion_all.csv";
       
       avgresFile.open(avgoutFile, ios::trunc);
       avgresFile << "Algorithm,Data,Clusters,Prop,Distances,Iterations,Timeout";
       avgresFile.close();
       
       allresFile.open(alloutFile, ios::trunc);
       allresFile << "Algorithm,Data,Clusters,Prop,Distances,Iterations,Timeout,Sample";
       allresFile.close();
       
       string alg = "";

       ofstream tempFile;
       algorithm_utils alg_utils;

        for(int i=0; i < file_list.size(); i++){

            cout << "\n%%%%%%%%%%%%%%%%%%%%%%%" << endl;
            cout << "Processing " << file_list[i] << endl;
            cout << "%%%%%%%%%%%%%%%%%%%%%%%\n" << endl;

            inputfilePath = input_path + file_list[i];

            cout << inputfilePath << endl;

            vector<vector <float> > dataset;
            
            std::pair<int, int> p = readSimulatedData(inputfilePath, dataset, labels, false, false);
            int numRows = p.first;
            int numCols = p.second;

            // Load data in Eigen format for Ball KMeans
            MatrixOur BallK_dataset = load_data(inputfilePath);

            for (int j = 0; j<data_prop.size(); j++){

                avg_km_loop_counter = 0;
                avg_kmdc_loop_counter = 0;
                avg_bkm_loop_counter = 0;

                avg_km_runtime = 0;
                avg_kmdc_runtime = 0;
                avg_bkm_runtime = 0;

                avg_km_num_he = 0; 
                avg_kmdc_num_he = 0;
                avg_bkm_num_he = 0;

                float prop = data_prop[j];
                
                cout << "\n" << endl;
                cout << "Data prop: " << prop << endl;
                cout << "\n";

                // Extract the proportion of data from original dataset to run the experiments
                num_points = ceil(dataset.size() * prop);
                vector<vector<float> > extracted_data(num_points, vector<float>(numCols, 0.0));
                alg_utils.extract_data(dataset, extracted_data, num_points, seed+j);

                // Extract data for Ball KMeans
                num_points = ceil(BallK_dataset.rows() * prop);
                MatrixOur extracted_ball_data(num_points, BallK_dataset.cols());
                extract_ball_data(BallK_dataset, extracted_ball_data, prop, num_points, seed+j);

                for(int n = 0; n<rep.size(); n++){

                    km_timeout = "no";
                    kmdc_timeout = "no";
                    ballkm_timeout = "no";

                    //####################
                    // KMeans
                    //####################
                    
                    cout << "Algo: KMeans" << endl; 
                    
                    // alg_utils.init_centroids_sequentially(centroids, extracted_data, clus);   
                    // km_res = kmeans(extracted_data, centroids, clus, threshold, num_iters, numCols, time_limit);
                    auto km_start_time = std::chrono::high_resolution_clock::now();

                    km_res = kmeans(extracted_data, num_clusters, threshold, num_iterations, numCols, 
                                    time_limit, "random", seed+rep[n]);
                    
                    auto km_end_time = std::chrono::high_resolution_clock::now();
                    km_time = std::chrono::duration_cast<std::chrono::milliseconds>(km_end_time - km_start_time).count();
                    
                    if (km_res.timeout == true){
                        km_timeout = "yes";
                        cout << "Timeout: Kmeans time: " << km_time << " milliseconds" << endl;
                    }

                    
                    //####################
                    // KMeans-DataCentric
                    //####################

                    cout << "Algo: Kmeans-DataCentric" << endl; 

                    auto kmdc_start_time = std::chrono::high_resolution_clock::now();
                    
                    kmdc_res = dckmeans(extracted_data, num_clusters, threshold, num_iterations, numCols, 
                    time_limit, "random", seed+rep[n]);

                    auto kmdc_end_time = std::chrono::high_resolution_clock::now();
                    kmdc_time = std::chrono::duration_cast<std::chrono::milliseconds>(kmdc_end_time - kmdc_start_time).count();

                    if (kmdc_res.timeout == true){
                        kmdc_timeout = "yes";
                        cout << "Timeout: Kmeans-DataCentric time: " << kmdc_time << " milliseconds" << endl;
                    }

                    //####################
                    // Ball-KMeans
                    //####################
                    
                    cout << "Algo: BallKMeans" << endl; 

                    auto bkm_start_time = std::chrono::high_resolution_clock::now();

                    ballkm_res = ball_k_means_Ring(extracted_ball_data, false, num_clusters, threshold, num_iterations, 
                    time_limit, "random", seed+rep[n]);

                    auto bkm_end_time = std::chrono::high_resolution_clock::now();
                    bkm_time = std::chrono::duration_cast<std::chrono::milliseconds>(bkm_end_time - bkm_start_time).count();

                    if (ballkm_res.timeout == true){
                        ballkm_timeout = "yes";
                        cout << "Timeout: BallKmeans time: " << bkm_time << " milliseconds" << endl;
                    }

                    // cout << "Data: " << file_list[i] << " Prop: " << prop << "\t" << " Clusters:" << num_clusters 
                    // << "\t calc: " << km_res.num_he <<  " " << kmdc_res.num_he <<  " " << ballkm_res.num_he << " " << 
                    // ballkm_res.loop_counter << " " << km_res.runtime << " " << kmdc_res.runtime << " " << ballkm_res.runtime << endl;

                    allresFile.open(alloutFile, ios::app);

                    allresFile << "\nKmeans" << "," << data_list[i] << "," << to_string(num_clusters) 
                    << "," << to_string(prop) << "," << to_string(km_res.num_he) << "," 
                    << to_string(km_res.loop_counter) << "," << km_timeout << "," << to_string(n+1);

                    allresFile << "\nKmeans-DataCentric" << "," << data_list[i] << "," << to_string(num_clusters) 
                    << "," << to_string(prop) << "," << to_string(kmdc_res.num_he) << "," 
                    << to_string(kmdc_res.loop_counter) << "," << kmdc_timeout << "," << to_string(n+1);

                    allresFile << "\nBall-Kmeans" << "," << data_list[i] << "," << to_string(num_clusters) 
                    << "," << to_string(prop) << "," << to_string(ballkm_res.num_he) << "," 
                    << to_string(ballkm_res.loop_counter) << "," << ballkm_timeout << "," << to_string(n+1);    

                    allresFile.close();

                    avg_km_loop_counter = avg_km_loop_counter + km_res.loop_counter;
                    avg_km_num_he = avg_km_num_he + km_res.num_he;
                    avg_km_runtime = avg_km_runtime + km_time;

                    avg_kmdc_loop_counter = avg_kmdc_loop_counter + kmdc_res.loop_counter;
                    avg_kmdc_num_he = avg_kmdc_num_he + kmdc_res.num_he;
                    avg_kmdc_runtime = avg_kmdc_runtime + kmdc_time;

                    avg_bkm_loop_counter = avg_bkm_loop_counter + ballkm_res.loop_counter;
                    avg_bkm_num_he = avg_bkm_num_he + ballkm_res.num_he;
                    avg_bkm_runtime = avg_bkm_runtime + bkm_time;

                }

                avg_km_loop_counter = (float)avg_km_loop_counter/num_rep;
                avg_km_num_he = (float)avg_km_num_he/num_rep;
                avg_km_runtime = (float)avg_km_runtime/num_rep;

                avg_kmdc_loop_counter = (float)avg_kmdc_loop_counter/num_rep;
                avg_kmdc_num_he = (float) avg_kmdc_num_he/num_rep;
                avg_kmdc_runtime = (float)avg_kmdc_runtime/num_rep;

                avg_bkm_loop_counter = (float)avg_bkm_loop_counter/num_rep;
                avg_bkm_num_he = (float)avg_bkm_num_he/num_rep;
                avg_bkm_runtime = (float)avg_bkm_runtime/num_rep;

                avgresFile.open(avgoutFile, ios::app);

                avgresFile << "\nKmeans" << "," << data_list[i] << "," << to_string(num_clusters) 
                << "," << to_string(prop) << "," << to_string(avg_km_num_he) << "," 
                << to_string(avg_km_loop_counter) << "," << km_timeout;

                avgresFile << "\nKmeans-DataCentric" << "," << data_list[i] << "," << to_string(num_clusters) 
                << "," << to_string(prop) << "," << to_string(avg_kmdc_num_he) << "," 
                << to_string(avg_kmdc_loop_counter) << "," << kmdc_timeout;

                avgresFile << "\nBall-Kmeans" << "," << data_list[i] << "," << to_string(num_clusters) 
                << "," << to_string(prop) << "," << to_string(avg_bkm_num_he) << "," 
                << to_string(avg_bkm_loop_counter) << "," << ballkm_timeout; 

                avgresFile.close();

            }

        }

       cout << "Completed Doubling Proportion Experiments" << endl;
    //    return 0;
}
