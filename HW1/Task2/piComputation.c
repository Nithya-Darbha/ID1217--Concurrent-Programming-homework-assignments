/* Computation of value of pi using adaptive quadrature
 Lecture 1 -slide 53 to 55

   usage under Linux:
     gcc piComputation.c -lm -lpthread 
     a.out numWorkers epsilion

*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define MAXTHREADS 10
#define ERRORBOUND 0.1e-4

double pi=0.0 , epsilion;
long numWorkers ;
double splitpoint;
double start_time, end_time;
pthread_mutex_t areaBarrier;

void *Worker(void *);

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

double function(double x){
    return sqrt(1-(x*x));
}

double quadrature(double l,double r,double fl,double fr,double area){
    double m = (l+r)/2.0;
    double fm= function(m);
    double larea = ((fl+fm)*(m-l))/2.0;
    double rarea = ((fm+fr)*(r-m))/2.0;

    if(fabs((larea +rarea)-area)>epsilion){
        larea= quadrature(l,m,fl,fm,larea);
        rarea= quadrature(m,r,fm,fr,rarea);
    }
    return (double)(larea+rarea);
}

void *Worker(void *arg) {
    long myid = (long) arg;
    double x =myid*splitpoint;
    double y = x+splitpoint;
    double fx = function(x);
    double fy = function(y);
    double areasplit = ((y-x)*(fx+fy))/2;

    double piValue= quadrature(x,y,fx,fy,areasplit) *4;

    pthread_mutex_lock(&areaBarrier);
    pi += piValue;
    pthread_mutex_unlock(&areaBarrier);
    
    return NULL;
}

int main(int argc , char* argv[]){
    long l ;

    numWorkers = (argc > 1)? atoi(argv[1]) : MAXTHREADS;
    if(argc>2){
        epsilion = atof(argv[2]);
    }
    else{
        epsilion= ERRORBOUND;
    }

    /*eror margin based on number of threads*/
    //epsilion= epsilion*numWorkers;
    splitpoint=1.0/numWorkers;

    pthread_t workerid[numWorkers];
    pthread_mutex_init(&areaBarrier, NULL);
  
    start_time = read_timer();
    for (l = 0; l < numWorkers; l++) {
        pthread_create(&workerid[l], NULL, Worker, (void *)l);
    }

    /* Wait for all worker threads to finish */
    for (l = 0; l < numWorkers; l++) {
        pthread_join(workerid[l], NULL);
    }
    end_time = read_timer();
    pthread_mutex_destroy(&areaBarrier);

    printf("Pi value= %.10lf\n", pi);
    printf("Error margin= %.10lf\n", epsilion);
    printf("The execution time is %g sec\n", end_time - start_time);

    return 0;  
}