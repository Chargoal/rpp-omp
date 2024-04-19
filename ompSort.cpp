#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace chrono;

// Function for bubble sort
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; ++i) {
        for (int j = 0; j < n-i-1; ++j) {
            if (arr[j] > arr[j+1]) {
                swap(arr[j], arr[j+1]);
            }
        }
    }
}

// Function to merge two sorted arrays
vector<int> merge(const vector<int>& left, const vector<int>& right) {
    vector<int> merged;
    size_t leftIndex = 0, rightIndex = 0;

    while (leftIndex < left.size() && rightIndex < right.size()) {
        if (left[leftIndex] <= right[rightIndex]) {
            merged.push_back(left[leftIndex]);
            ++leftIndex;
        } else {
            merged.push_back(right[rightIndex]);
            ++rightIndex;
        }
    }

    while (leftIndex < left.size()) {
        merged.push_back(left[leftIndex]);
        ++leftIndex;
    }

    while (rightIndex < right.size()) {
        merged.push_back(right[rightIndex]);
        ++rightIndex;
    }

    return merged;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <number of threads> <input file name>\n";
        return 1;
    }

    int numThreads = atoi(argv[1]);
    string inputFile = argv[2];

    // Reading array from file
    ifstream inFile(inputFile);
    if (!inFile) {
        cerr << "Error: Unable to open input file: " << inputFile << endl;
        return 1;
    }

    vector<int> data;
    int value;
    while (inFile >> value) {
        data.push_back(value);
    }
    inFile.close();

    // Measure start time
    auto start = high_resolution_clock::now();

    // Distributing array among threads
    size_t chunkSize = data.size() / numThreads;
    vector<vector<int>> chunks(numThreads);
    #pragma omp parallel num_threads(numThreads)
    {
        int threadID = omp_get_thread_num();
        size_t startIndex = threadID * chunkSize;
        size_t endIndex = (threadID == numThreads - 1) ? data.size() : startIndex + chunkSize;
        chunks[threadID].assign(data.begin() + startIndex, data.begin() + endIndex);

        // Sorting its chunk of array
        bubbleSort(chunks[threadID]);
    }

    // Merging sorted chunks
    vector<int> result = chunks[0];
    for (int i = 1; i < numThreads; ++i) {
        result = merge(result, chunks[i]);
    }

    // Measure end time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    // Writing sorted array to file
    ofstream outFile("output.txt");
    if (!outFile) {
        cerr << "Error: Unable to open output file: output.txt\n";
        return 1;
    }

    for (const auto& num : result) {
        outFile << num << " ";
    }
    outFile.close();

    cout << "Time taken: " << duration.count() << " milliseconds" << endl;

    return 0;
}
