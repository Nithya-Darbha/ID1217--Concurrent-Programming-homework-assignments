#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAXWORKERS 10   /* maximum number of workers */

/* function prototypes*/
void *PiWorker(void *arg);
double read_timer();

double start_time, end_time; /* start and end times */
double l, r, fl, fr, area , epsilion, ans;
int numThreads ;
int numArrived = 0; 
int flag =0 ;

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
pthread_t workerThreads[MAXWORKERS];

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

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


/*f(x)= root of 1-x^2*/
double function(double x){
    return sqrt(1- (x*x));
}

/*recursive quadrature function*/
double quadrature(double l, double r, double fl, double fr, double area, double epsilion, int* threads, pthread_mutex_t* lock){

}


/*compute area - worker function*/
void *AreaWorker(void *arg){

}


/*main method*/
int main(int argc, char* argv[]){

}

