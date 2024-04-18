#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    #pragma omp parallel for
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
        cout << "Usage: ./ompSort.exe <number of threads> <input file name>\n";
        return 1;
    }

    int numThreads = atoi(argv[1]);
    omp_set_num_threads(numThreads);

    string filename = argv[2];
    auto start = high_resolution_clock::now();
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

    // Sort the numbers using OpenMP
    bubbleSort(numbers);

    // Stop timer
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Sorting duration: " << duration.count() << " milliseconds" << endl;

    // Output sorted array to file
    ofstream outputFile("output.txt");
    if (!outputFile) {
        cout << "Error: Unable to create output file." << endl;
        return 1;
    } else {
        for (int num : numbers) {
            outputFile << num << " ";
        }
        outputFile.close();
    }

    return 0;
}
