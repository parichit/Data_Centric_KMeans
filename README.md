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

-  Link with the eigen header files by using the -I CPP compiler flag. If g++ is not installed in the system, it can be replaced with other c++ compilers available on the system. The following command will throw a warning upon execution, please ignore the warning.

```
g++ -std=c++11 -I ../eigen/ -O3 driver.cpp -o driver.out 
```

- Execute the newly compiled binary as follows:

```
./driver.out
```

- The following output will get printed to the screen

```

```






