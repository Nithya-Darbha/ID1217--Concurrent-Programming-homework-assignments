#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
double start_time, end_time;

void swap( int *one , int *two){
    int temp = *one;
    *one = *two;
    *two = temp;
}

int divideArray(int arr[], int hi, int lo){
    int pivot = arr[hi];
    int i =lo-1;
    for(int j=lo ; j<hi ; j++){
        if(arr[j]<pivot){
            i++;
            swap(&arr[i],&arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[hi]);
    return i + 1; 
}

int quickSortMethod(int arr[] , int lo, int hi){
    if(lo<hi){
        int pivot = divideArray(arr,hi,lo);
        #pragma omp task
        quickSortMethod(arr,lo,pivot - 1); // left half

        #pragma omp task
        quickSortMethod(arr,pivot + 1,hi);// right half 
    }
}

int main(int argc, char *argv[]){
    if(argc !=3){
        printf("array size and number of threads: %s <arr_size> <numOfthreads>\n",argv[0]);
        return 1;
    }

    int capacity = atoi(argv[1]);
    int threads = atoi(argv[2]);

    /*allocation of array*/
    int *arr = (int*)malloc(capacity * sizeof(int));
    if(arr==NULL){
        printf("mem alloctaion failed");
        return 1;
    }

    /*creation of array*/
    srand(time(0));
     for (int i = 0 ; i<capacity;i++){
        arr[i]= rand()% 100000;
    }

    omp_set_num_threads(threads);
    start_time=omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        quickSortMethod(arr,0,capacity-1);
    }
    end_time=omp_get_wtime();
    printf("array size %d \n", capacity);
    printf("number of threads %d \n", threads);
    printf("execution time %f sec \n", end_time-start_time);
    free(arr);
    return 0 ;
}
