#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define OVERFLOW 3
#define BEES 2
int pot = 0;  
int potCapacity = OVERFLOW;
int numberOfBees;

sem_t mutex; 
sem_t fullPot;     
sem_t emptyPot;    

void* honeyBee(void* arg) {
    int beeId =(int)arg + 1;  
    while(1){
        sem_wait(&emptyPot);     
        sem_wait(&mutex);   
        if (pot == potCapacity) {
        } 
        else{
        pot++;
        printf("honeybee %ld added honey,pot now has %d portions.\n",beeId,pot);
            if (pot == potCapacity) {
                printf("pot reached its capacity, honeybee %ld is waking up the bear!\n",beeId);
                sem_post(&fullPot);  
            }
        }
        sem_post(&mutex);  
        sem_post(&emptyPot);    
        usleep(rand()%100000); 
    }
}

void* bear(void* arg) {
    while (1) {
        sem_wait(&fullPot);    
        sem_wait(&mutex);  
        printf("bear wakes up! bear eats all the honey,pot is now empty.\n");
        pot = 0; 
        sem_post(&emptyPot);
        sem_post(&mutex);  
        printf("bear goes back to sleep.\n");
        usleep(rand() % 200000); 
    }
}

int main(int argc,char* argv[]) {
    if (argc<3) {
        printf("default values used");
        numberOfBees=BEES;
        potCapacity=OVERFLOW;
    } else {
        numberOfBees=atoi(argv[1]);
        potCapacity = atoi(argv[2]);
    }
    pthread_t bees[numberOfBees];
    pthread_t bearThread;
    sem_init(&emptyPot,0,1); 
    sem_init(&fullPot,0,0);   
    sem_init(&mutex,0,1);
    
    pthread_create(&bearThread,NULL,bear,NULL);

    for (int i=0;i<numberOfBees;i++) {
        pthread_create(&bees[i],NULL,honeyBee,(void*)i);
    }
    
    for (int i=0;i<numberOfBees;i++) {
        pthread_join(bees[i],NULL);
    }
    pthread_join(bearThread,NULL);

    sem_destroy(&emptyPot);
    sem_destroy(&fullPot);
    sem_destroy(&mutex);
    return 0;
}

