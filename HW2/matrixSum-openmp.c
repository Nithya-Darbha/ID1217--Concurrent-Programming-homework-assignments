/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/
/* hw1 implementation 2 of min sum and mx - mutex protects critical section
 which are sum , min and max
 same concept in this - critical section is min and max my pragma omp critical
 sum is handled by-pragma omp parallel for reduction (+:total) private(j)
*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;
  int min , max , minx , miny , maxx, maxy; /*row is x  and column is y*/

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    //  printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      //	  printf(" %d", matrix[i][j]);
	  }
	  //	  printf(" ]\n");
  }

  /*intialise values*/
   total =0;
   min = 99;
   max = 0;
   minx = 0;
   miny= 0;
   maxx=0;
   maxy =0;


  start_time = omp_get_wtime();

#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];

      if(matrix[i][j] <min){
        #pragma omp critical
        {
          if(matrix[i][j] <min){
            min = matrix[i][j];
            minx = i;
            miny=j;
          }  
        }  
      }

      if(matrix[i][j] >max){
        #pragma omp critical /*only one thread accesses section at a time*/
        {
          if(matrix[i][j] > max){
            max = matrix[i][j];
            maxx = i;
            maxy=j;
          }
        }  
      }
    }
// implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("it took %g seconds\n", end_time - start_time);
  printf("the min value is %d and the position is (%d, %d) \n" , min, minx,miny);
  printf("the max value is %d and the position is (%d, %d)\n" , max, maxx, maxy);

}

