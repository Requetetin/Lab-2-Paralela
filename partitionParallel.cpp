// Algoritmo quicksort en C++ extraido de: https://www.programiz.com/dsa/quick-sort

#include <iostream>
#include <fstream>      // std::ofstream
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <string.h>
#include "omp.h"
using namespace std;
using namespace std::chrono;

// function to swap elements
void swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

// function to rearrange array (find the partition point)
int partition(int array[], int low, int high) {
    
  // select the rightmost element as pivot
  int pivot = array[high];
  
  // pointer for greater element
  int i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  #pragma omp parallel for reduction(+: i)
  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      swap(&array[i], &array[j]);
    }
  }
  
  // swap pivot with the greater element at i
  swap(&array[i + 1], &array[high]);
  
  // return the partition point
  return (i + 1);
}

void quickSort(int array[], int low, int high) {
  if (low < high) {
      
    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on righ of pivot
    int pi = partition(array, low, high);

    // recursive call on the left of pivot
    #pragma omp task shared(array)
    quickSort(array, low, pi - 1);

    // recursive call on the right of pivot
    #pragma omp task shared(array)
    quickSort(array, pi + 1, high);
  }
}

// Driver code
int main(int argc, char * argv[]) {
  int N = atoi(argv[1]);
  srand(time(NULL));
  ofstream write_file("random_numbers.csv");
  int *heap_numbers = new int[N];
  omp_set_num_threads(12);

  // Get starting timepoint
  auto start = high_resolution_clock::now();

  #pragma omp parallel for ordered
  for (int i = 0; i < N; i++) {
    #pragma omp ordered
    write_file << rand() % 100 << ",";
  }
  write_file.close();

  ifstream read_file("random_numbers.csv");
  string line;
  getline(read_file, line);
  char* token = strtok(const_cast<char*>(line.c_str()), ",");
  
  for (int i = 0; i < N; i++) {
    heap_numbers[i] = atoi(token);
    token = strtok(nullptr, ",");
  }

  read_file.close();

  quickSort(heap_numbers, 0, N-1);

  ofstream write_sorted_file("write_sorted_file.csv");
  #pragma omp parallel for ordered
  for (int i = 0; i < N; i++) {
    #pragma omp ordered
    write_sorted_file << heap_numbers[i] << ",";
  }
  write_sorted_file.close();

  delete [] heap_numbers;
  
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: " << duration.count() << " microseconds" << endl;

  return 0;
}