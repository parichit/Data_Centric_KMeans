### KMeans Data Centric (Kmeans-d)

Data-centric AI (DCAI) is an emerging paradigm that prioritizes the quality, diversity, and repre-
sentation of data over model architecture and hyperparameter tuning. DCAI emphasizes up-stream data operations such as cleaning, balancing, and preprocessing, rather than solely focusing
on downstream model selection and optimization. This work aims to push DCAI into the model building phase itself, observing whether benefits downstream can be as significant in a classical, well studied algorithm like k-means. We introduce data-centric k-means (or kmeans-d for
short). kmeans-d is a novel adaptation of k-means clustering that achieves significant speedups while preserving algorithmic accuracy. The key innovation classifies data points as high expressive
(HE), impacting the objective function significantly, or low expressive (LE), with minimal in-
fluence. By categorizing data points as HE/LE, kmeans-d extracts quality signals from data to
improve scalability and reduce computational overhead. Comprehensive experimental evalua-
tion demonstrate substantial performance gains of kmeans-d over existing alternatives. The novelty
lies in the pioneering integration of data-centric principles within a fundamental algorithm’s it-
erative core. By rethinking k-means through a data lens, kmeans-d delivers superior efficiency
without sacrificing properties like accuracy and convergence, paving the way for infusing data-centricity into other canonical algorithms.


### Dependency

The code base is purely developed in C++. Both Kmeans and Kmeans-d can be compiled direclty via standard C++ compiler (g++, clang etc.). However, Ball_Kmeans depends on the Eigen linear algebra library and requires linking to compile successfully. Following code snippets show exmaples of compiling the binary.

Note: We have already copied the Eigen header files (required for compiling) into the Github repository.

### Compiling Instructions

- Change the directory to src

```
cd src
```

- Link with the eigen header files by using the -I CPP compiler flag. If g++ is not installed in the system, it can be replaced with other c++ compilers available on the system. The following command will throw a warning upon execution, please ignore the warning.

```
g++ -std=c++11 -I ../eigen/ driver.cpp -o driver.out 
```

- Execute the newly compiled binary as follows:

```
./driver.out
```

- __Sample output:__ The following output will be printed on the screen. Due to githhub's file size restriction, only small to moderate size data is shared under Sample_Data. All datasets are available to download from UCI Machine Learning repository.

```
Reading the input from: ../Sample_Data/Breastcancer.csv
File reading: 35 MS
Data size: 569 X 30

Algo: KMeans, Clusters: 12, Threshold: 0.0001
Total KMeans time: 68 milliseconds

Algo: DCKM, Clusters: 12, Threshold: 0.0001
Total Kmeans-d time: 27 milliseconds

Algo: Ball Kmeans, Clusters: 12, Threshold: 0.0001
Total Ball Kmeans time: 155 milliseconds

#################################
#################################
Reading the input from: ../Sample_Data/CreditRisk.csv
File reading: 43 MS
Data size: 1000 X 20

Algo: KMeans, Clusters: 12, Threshold: 0.0001
Total KMeans time: 201 milliseconds

Algo: DCKM, Clusters: 12, Threshold: 0.0001
Total Kmeans-d time: 53 milliseconds

Algo: Ball Kmeans, Clusters: 12, Threshold: 0.0001
Total Ball Kmeans time: 196 milliseconds

#################################
#################################
Reading the input from: ../Sample_Data/census.csv
File reading: 1081 MS
Data size: 45222 X 6

Algo: KMeans, Clusters: 12, Threshold: 0.0001
Total KMeans time: 3844 milliseconds

Algo: DCKM, Clusters: 12, Threshold: 0.0001
Total Kmeans-d time: 2890 milliseconds

Algo: Ball Kmeans, Clusters: 12, Threshold: 0.0001
Total Ball Kmeans time: 22720 milliseconds

#################################
#################################
Reading the input from: ../Sample_Data/birch.csv
File reading: 650 MS
Data size: 100000 X 2

Algo: KMeans, Clusters: 12, Threshold: 0.0001
Total KMeans time: 2539 milliseconds

Algo: DCKM, Clusters: 12, Threshold: 0.0001
Total Kmeans-d time: 979 milliseconds

Algo: Ball Kmeans, Clusters: 12, Threshold: 0.0001
Total Ball Kmeans time: 12115 milliseconds

#################################
#################################%
```

### Raw Benchmark Results

Raw benchmark data is available as follows:

Experiment Type | Folder Name | File Name
--- | --- | ---
(Random seeding) Benchmarks on real data | results_benchmark_real_data | benchmark_real_avg_runs.csv
(k++ seeding) Benchmarks on real data | results_benchmark_real_data | benchmark_real_kplus_avg_runs.csv
(synthetic data) Clustering experiments | results_synthetic_data | benchmark_clus_avg_runs.csv 
(synthetic data) Dimensionality experiments | results_synthetic_data | benchmark_dims_avg_runs.csv
(synthetic data) Scalability experiments | results_synthetic_data | benchmark_scal_avg_runs.csv
Doubling experiments | results_doubling_exp | doubling_clusters_avg.csv, doubling_proportion_avg.csv
Ablation experiments | results_ablation | (with vectorization) ablation_with_vec_avg.csv, (without vectorization) ablation_with_vec_avg.csv

### Contact Details

For help with compiling/running, or reporting issues-please let us know at parishar[at]iu[dot]edu. We would be happy to help you out.


