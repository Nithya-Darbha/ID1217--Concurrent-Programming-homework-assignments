/* task c
matrix summation using pthreads

   features: bag of tasks implementation 

   usage under Linux:
     gcc matrixSumMutex.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t counterMutex;  /* mutex lock for the barrier */
pthread_mutex_t resultMutex; 
//pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 


/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums; /*  sum */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */
/*max and min values*/
int maxV;
int minV;
/*max and min position*/
int maxX;
int maxY;
int minX;
int minY;
int btcounter;


void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&counterMutex, NULL);
  pthread_mutex_init(&resultMutex, NULL);
  //pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] =rand()%99; //rand()%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

    sums=0;
    minV=100;
    maxV=0;
    maxX=0;
    maxY=0;
    minX=0;
    minY=0;
    btcounter=0;

  /* Create worker threads and wait for them to finish */
start_time = read_timer();
for (l = 0; l < numWorkers; l++) {
    pthread_create(&workerid[l], &attr, Worker, (void *)l);
}

/* Wait for all worker threads to finish */
for (l = 0; l < numWorkers; l++) {
    pthread_join(workerid[l], NULL);
}
end_time = read_timer();

/* Main thread computes and prints the final results */
printf("The total sum is %d\n", sums);
printf("Min value %d and location(%d, %d).\n", minV, minY, minX);
printf("Max value %d and location(%d, %d).\n", maxV, maxY, maxX);
printf("The execution time is %g sec\n", end_time - start_time);

  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;

  int total, j, first, last , row;
  // private variables
  int specificMin, specificMax; // value
  int specificMinx, specificMiny, specificMaxx, specificMaxy;// position


#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);


  /* sum values in my strip */
  total = 0;
  specificMinx = 0;
  specificMiny = 0;
  specificMaxx = 0;
  specificMaxy = 0;
  specificMin = 99;
  specificMax= -1;

  while(true){

    pthread_mutex_lock(&counterMutex);
    row = btcounter;
    btcounter++;
    pthread_mutex_unlock(&counterMutex);


     if(row>= size){
        pthread_mutex_lock(&resultMutex);
        sums += total;
        if(minV>specificMin){
            minV= specificMin;
            minX= specificMinx;
            minY= specificMiny;
        }

        if(maxV < specificMax){
            maxV= specificMax;
            maxX= specificMaxx;
            maxY= specificMaxy;
        }
        pthread_mutex_unlock(&resultMutex);
        pthread_exit(NULL);
     }

     else {
        for (j = 0; j < size; j++){
        int current = matrix[row][j];
        total += matrix[row][j];

        if(current < specificMin){
            specificMin = current;
            specificMinx=row;
            specificMiny=j;
        }

        if(current > specificMax){
            specificMax = current;
            specificMaxx=row;
            specificMaxy=j;
        }
      }
    }
  }   
}
    
    

