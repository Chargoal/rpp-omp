#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm> // For std::merge
#include <omp.h>

using namespace std;
using namespace std::chrono;

void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: ./ompSort <number of processes> <input file name>\n";
        return 1;
    }

    int numProcesses = atoi(argv[1]);
    string filename = argv[2];

    auto start = chrono::high_resolution_clock::now();
    ifstream inputFile(filename);
    if (!inputFile) {
        cout << "Error: Unable to open file " << filename << endl;
        return 1;
    }

    vector<int> numbers;
    int number;

    while (inputFile >> number) {
        numbers.push_back(number);
    }

    inputFile.close();

    int numbersSize = numbers.size();
    if (numbersSize == 0) {
        cout << "Error: No data found in file " << filename << endl;
        return 1;
    }

    // Divide data into chunks for each process
    int chunkSize = numbersSize / numProcesses;
    #pragma omp parallel num_threads(numProcesses)
    {
        int tid = omp_get_thread_num();
        int startIdx = tid * chunkSize;
        int endIdx = (tid == numProcesses - 1) ? numbersSize : (tid + 1) * chunkSize;
        vector<int> localData(numbers.begin() + startIdx, numbers.begin() + endIdx);
        bubbleSort(localData);
        #pragma omp critical
        numbers.insert(numbers.begin() + startIdx, localData.begin(), localData.end());
    }

    // Merge sorted chunks
    for (int i = 1; i < numProcesses; ++i) {
        int startIndex = i * chunkSize;
        merge(numbers.begin(), numbers.begin() + startIndex, numbers.begin() + startIndex,
            numbers.begin() + startIndex + chunkSize, numbers.begin());
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Sorting duration: " << duration.count() << " milliseconds" << endl;

    ofstream outputFile("output.txt");
    if (!outputFile) {
        cout << "Error: Unable to create output file." << endl;
    } else {
        for (int num : numbers) {
            outputFile << num << " ";
        }
        outputFile.close();
    }

    return 0;
}