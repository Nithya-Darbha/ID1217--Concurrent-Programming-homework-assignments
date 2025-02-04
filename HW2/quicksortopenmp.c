#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void parallel_quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);

        #pragma omp task if(high - low > 1000)
        parallel_quicksort(arr, low, pivot - 1);

        #pragma omp task if(high - low > 1000)
        parallel_quicksort(arr, pivot + 1, high);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <array_size> <num_threads>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    srand(time(0));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }
    
    omp_set_num_threads(num_threads);
    double start_time = omp_get_wtime();
    
    #pragma omp parallel
    {
        #pragma omp single
        parallel_quicksort(arr, 0, n - 1);
    }
    
    double end_time = omp_get_wtime();
    printf("Time taken: %f seconds\n", end_time - start_time);
    
    free(arr);
    return 0;
}

/*
void insertionSortMethod(int arr[], int hi , int lo){
    for (int i = lo; i <hi; i++) {
        int abc = arr[i];
        int j =i-1;
        while(j>= lo && arr[j]>abc){
            arr[j+1]=arr[j];
            j--;
        }
        arr[j+1]=abc;
    }
}

// from java assignments
public static void swap(int[] array , int one , int two){
int x = array[one];
array[one]= array[two];
array[two]= x;
}

public static void InsertionSortMethod(int[] array){
for (int i = 1; i < array.length; i++) {
int abc = array[i];
for (int j = i-1; j >= 0 && array[j] > abc ; j--) {
swap(array, j, j+1);
}
}

*/

/*
int CriticalSection(int arr[],int capacity,int numWorkers){
    omp_set_num_threads(numWorkers);
    start_time = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single 
        quickSortMethod(arr , 0 , capacity=1);
    }
    end_time = omp_get_wtime();
    printf("Number of threads: %d \n", numWorkers);
    printf("Execution time: %g \n", end_time-start_time);

}

int createArray(int arr[],int capacity){
    for(int i=0; i<capacity;i++){
        arr[i]= rand()%100000;
    }
}
1//-t , 0-f
int checkArray(int arr[],int capacity){
    for(int i=0;i<capacity;i++){
        if(arr[i]<arr[i-1]){
            return 0 ;
        }
    }
    return 1;
}
*/
