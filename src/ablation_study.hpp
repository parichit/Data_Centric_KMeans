// Parichit Sharma, parishar@iu.edu
// Luddy Center for Artificial Intelligence, IU
// Department of CS, Luddy School of Informatics, Computing & Engineering

#include <iostream>
#include <string>
#include <tuple>
#include <iomanip>
#include <chrono>

using namespace std;


void ablation(string basePath, string vecFlag){

       string input_path = basePath;     
       string out_path = basePath;
       
        // Declare variables
       vector<string> file_list = {"census.csv", "Twitter.csv", "birch.csv"};
       vector<string> data_list = {"Census", "Twitter", "Birch"};

        int num_iterations = 100;
        float threshold = 0.05;
        vector<int> num_clusters = {30, 50, 60, 80};
        int num_rep = 10;
        int seed = 20;

        // Timeout limit of 30 minutes
        int time_limit = 1800000;

        unsigned long long int avg_km_num_he = 0, avg_kmdc_num_he = 0, avg_bkm_num_he =0; 
        int avg_km_loop_counter = 0, avg_kmdc_loop_counter = 0, avg_bkm_loop_counter = 0;

        float km_time = 0, kmdc_time = 0, bkm_time = 0, avg_km_runtime = 0, 
        avg_kmdc_runtime = 0, avg_bkm_runtime = 0;

       string km_timeout = "no", dckm_timeout = "no", ballkm_timeout = "no";
        
       string inputfilePath = "", centroidFilePath = "";
       bool run_stat = false;
       vector<int> labels;
       
       output_data km_res, kmdc_res, ballkm_res;
       
       ofstream avgresFile, allresFile;
       string avgoutFile = "", alloutFile = "";
       
       if (vecFlag == "1"){
            avgoutFile = out_path + "ablation_with_vec_avg.csv" ;
            alloutFile = out_path + "ablation_with_vec_all.csv" ;
       }
       else if (vecFlag == "0"){
            avgoutFile = out_path + "ablation_without_vec_avg.csv" ;
            alloutFile = out_path + "ablation_without_vec_all.csv" ;
       }
       
       
       allresFile.open(alloutFile, ios::trunc);
       allresFile << "Algorithm,Data,Clusters,Iters,Runtime,Runtime_per_Iter,Runtime_speedup_km,Runtime_speedup_bkm,Distances,Dist_speed_up,Timeout";
       allresFile.close();

       avgresFile.open(avgoutFile, ios::trunc);
       avgresFile << "Algorithm,Data,Clusters,Iters,Runtime,Runtime_per_Iter,Runtime_speedup_km,Runtime_speedup_bkm,Distances,Dist_speed_up,Timeout";
       avgresFile.close();

       string alg = "";
       vector<string> algorithm = {"Kmeans", "Kmeans-DataCentric", "Ball-Kmeans"};


        for(int i=0; i < file_list.size(); i++){

            inputfilePath = input_path + file_list[i];
              
            cout << "\n%%%%%%%%%%%%%%%%%%%%%%%" << endl;
            cout << "Processing " << inputfilePath << endl;
            cout << "%%%%%%%%%%%%%%%%%%%%%%%\n" << endl;

            vector<vector <float> > dataset;
            
            std::pair<int, int> p = readSimulatedData(inputfilePath, dataset, labels, false, false);
            int numRows = p.first;
            int numCols = p.second;

            // Load data in Eigen format for Ball KMeans
            MatrixOur BallK_dataset = load_data(inputfilePath);
            
            for (int j = 0; j< num_clusters.size(); j++){

                int clus = num_clusters[j];
                
                // cout << "\nClusters:\n " << clus << endl;

                avg_km_loop_counter = 0;
                avg_kmdc_loop_counter = 0;
                avg_bkm_loop_counter = 0;

                avg_km_runtime = 0;
                avg_kmdc_runtime = 0;
                avg_bkm_runtime = 0;

                avg_km_num_he = 0; 
                avg_kmdc_num_he = 0;
                avg_bkm_num_he = 0;

                for (int k = 0 ; k < num_rep ; k++){
        
                    km_timeout = "no";
                    dckm_timeout = "no";
                    ballkm_timeout = "no";

                    //####################
                    // KMeans
                    //####################
                    cout << "Algo: KMeans" << endl; 

                    auto km_start_time = std::chrono::high_resolution_clock::now();

                    km_res = kmeans(dataset, clus, threshold, num_iterations, numCols, 
                            time_limit, "random", seed+clus+k);

                    auto km_end_time = std::chrono::high_resolution_clock::now();

                    if (data_list[i] == "CreditRisk" | data_list[i] == "Breastcancer"){
                        km_time = std::chrono::duration_cast<std::chrono::microseconds>(km_end_time - km_start_time).count();
                        km_time = km_time/1000;
                    }
                    else{
                        km_time = std::chrono::duration_cast<std::chrono::milliseconds>(km_end_time - km_start_time).count();
                    }
                    
                    if (km_res.timeout == true){
                        km_timeout = "yes";
                        cout << "Timeout: Kmeans time: " << km_time << " milliseconds" << endl;
                    }
                    else{
                        std::cout << "Kmeans time: " << km_time << " milliseconds" << endl;
                    }

                    //####################
                    // KMeans-DataCentric
                    //####################
                    cout << "Algo: Kmeans-DataCentric" << endl; 

                    auto kmdc_start_time = std::chrono::high_resolution_clock::now();

                    kmdc_res = dckmeans(dataset, clus, threshold, num_iterations, numCols, time_limit, "random", seed+clus+k);

                    auto kmdc_end_time = std::chrono::high_resolution_clock::now();

                    if (data_list[i] == "CreditRisk" | data_list[i] == "Breastcancer"){
                        // cout << "\nYES\n" << endl;
                        kmdc_time = std::chrono::duration_cast<std::chrono::microseconds>(kmdc_end_time - kmdc_start_time).count();
                        kmdc_time = kmdc_time/1000;
                    }
                    else{
                        kmdc_time = std::chrono::duration_cast<std::chrono::milliseconds>(kmdc_end_time - kmdc_start_time).count();
                    }
                    
                    if (kmdc_res.timeout == true){
                        dckm_timeout = "yes";
                        cout << "Timeout: Kmeans-DataCentric time: " << kmdc_time << " milliseconds" << endl;
                    }
                    else{
                        cout << "Total Kmeans-DataCentric time: " << kmdc_time << " milliseconds" << endl;
                    }
                    
                    //####################
                    // Ball-KMeans
                    //####################
                    cout << "Algo: Ball-KMeans" << endl; 

                    auto bkm_start_time = std::chrono::high_resolution_clock::now();

                    ballkm_res = ball_k_means_Ring(BallK_dataset, true, clus, threshold, num_iterations, time_limit, 
                    "random", seed+clus+k);

                    auto bkm_end_time = std::chrono::high_resolution_clock::now();

                    if (data_list[i] == "CreditRisk" | data_list[i] == "Breastcancer"){
                        bkm_time = std::chrono::duration_cast<std::chrono::microseconds>(bkm_end_time - bkm_start_time).count();
                        bkm_time = bkm_time/1000;
                    }
                    else{
                        bkm_time = std::chrono::duration_cast<std::chrono::milliseconds>(bkm_end_time - bkm_start_time).count();
                    }

                    if (ballkm_res.timeout == true){
                        ballkm_timeout = "yes";
                        cout << "Timeout: BallKmeans time: " << bkm_time << " milliseconds" << endl;
                    }
                    else{
                        cout << "Total BallKmeans time: " << bkm_time << " milliseconds" << endl;
                    }

                    allresFile.open(alloutFile, ios::app);

                    allresFile << "\nKmeans" << "," << data_list[i] << "," << to_string(clus) 
                    << "," << std::setprecision(2) << to_string(km_res.loop_counter) <<  "," << 
                    std::setprecision(6) << to_string(km_time) << "," << std::setprecision(6) <<
                    to_string((float)km_time/km_res.loop_counter) << "," << std::setprecision(2) << to_string(0)
                    << ","  << to_string(0) << "," << std::setprecision(2) << to_string(km_res.num_he) <<
                    "," << std::setprecision(2) << to_string(0) << "," << km_timeout;

                    allresFile << "\nKmeans-DataCentric" << "," << data_list[i] << "," << to_string(clus) 
                    << "," << std::setprecision(2) << to_string(kmdc_res.loop_counter) <<  "," << 
                    std::setprecision(6) << to_string(kmdc_time) << "," << std::setprecision(6) <<
                    to_string((float)kmdc_time/kmdc_res.loop_counter) << "," << std::setprecision(2) << 
                    to_string((float)km_time/kmdc_time) << "," << to_string(bkm_time/kmdc_time)
                    << "," << std::setprecision(2) << to_string(kmdc_res.num_he) <<
                    "," << std::setprecision(2) << to_string((float)km_res.num_he/kmdc_res.num_he) << "," << dckm_timeout;

                    "Algorithm,Data,Clusters,Iters,Runtime,Runtime_per_Iter,Runtime_speedup_km,Runtime_speedup_bkm,Distances,Dist_speed_up,Timeout";

                    allresFile << "\nBall-Kmeans" << "," << data_list[i] << "," << to_string(clus) 
                    << "," << to_string(ballkm_res.loop_counter) <<  "," << 
                    std::setprecision(6) << to_string(bkm_time) << "," << std::setprecision(6) <<
                    to_string((float)bkm_time/ballkm_res.loop_counter) << "," << std::setprecision(2) << 
                    to_string((float)km_time/bkm_time) <<  "," << to_string(1)
                    << "," << std::setprecision(2) << to_string(ballkm_res.num_he) <<
                    "," << std::setprecision(2) << to_string((float)km_res.num_he/ballkm_res.num_he) << "," << ballkm_timeout;

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

                // cout << avg_km_runtime << "\t" << (float)avg_km_runtime/num_rep <<  endl ;
                // cout << avg_kmdc_runtime << "\t" << (float)avg_kmdc_runtime/num_rep <<  endl ;
                // cout << avg_bkm_runtime << "\t" << (float)avg_bkm_runtime/num_rep <<  endl ;

                avg_km_loop_counter = (float)avg_km_loop_counter/num_rep;
                avg_km_num_he = (float)avg_km_num_he/num_rep;
                avg_km_runtime = (float)avg_km_runtime/num_rep;

                avg_kmdc_loop_counter = (float)avg_kmdc_loop_counter/num_rep;
                avg_kmdc_num_he = (float) avg_kmdc_num_he/num_rep;
                avg_kmdc_runtime = (float)avg_kmdc_runtime/num_rep;

                avg_bkm_loop_counter = (float)avg_bkm_loop_counter/num_rep;
                avg_bkm_num_he = (float)avg_bkm_num_he/num_rep;
                avg_bkm_runtime = (float)avg_bkm_runtime/num_rep;

                // cout << avg_km_runtime << endl;
                // cout << avg_kmdc_runtime << endl;
                // cout << avg_bkm_runtime << endl;.

                avgresFile.open(avgoutFile, ios::app);

                avgresFile << "\nKmeans" << "," << data_list[i] << "," << to_string(clus) 
                << "," << std::setprecision(2) << to_string(avg_km_loop_counter) <<  "," << 
                std::setprecision(6) << to_string(avg_km_runtime) << "," << std::setprecision(6) <<
                to_string((float)avg_km_runtime/avg_km_loop_counter) << "," << to_string(0)
                << ","  << to_string(0) << "," << std::setprecision(3) << to_string(avg_km_num_he) <<
                "," << to_string(0) << "," << km_timeout;

                avgresFile << "\nKmeans-DataCentric" << "," << data_list[i] << "," << to_string(clus) 
                << "," << std::setprecision(2) << to_string(avg_kmdc_loop_counter) <<  "," << 
                std::setprecision(6) << to_string(avg_kmdc_runtime) << "," << std::setprecision(6) <<
                to_string((float)avg_kmdc_runtime/avg_kmdc_loop_counter) << "," << std::setprecision(3) << 
                to_string((float)avg_km_runtime/avg_kmdc_runtime) << "," << to_string(avg_bkm_runtime/avg_kmdc_runtime)
                << "," << std::setprecision(2) << to_string(avg_kmdc_num_he) <<
                "," << std::setprecision(2) << to_string((float)avg_km_num_he/avg_kmdc_num_he) << "," << dckm_timeout;

                avgresFile << "\nBall-Kmeans" << "," << data_list[i] << "," << to_string(clus) 
                << "," << std::setprecision(2) << to_string(avg_bkm_loop_counter) <<  "," << 
                std::setprecision(6) << to_string(avg_bkm_runtime) << "," << std::setprecision(6) <<
                to_string((float)avg_bkm_runtime/avg_bkm_loop_counter) << "," << std::setprecision(3) << 
                to_string((float)avg_km_runtime/avg_bkm_runtime) << "," << to_string(1)
                << "," << std::setprecision(2) << to_string(avg_bkm_num_he) <<
                "," << std::setprecision(4) << to_string((float)avg_km_num_he/avg_bkm_num_he) << "," << ballkm_timeout;

                avgresFile.close();
                
            }

        }

    cout << "Completed benchmarks" << endl;
}
