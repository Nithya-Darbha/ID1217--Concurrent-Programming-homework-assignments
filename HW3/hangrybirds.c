#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WORMS 10
#define BABYBIRDS 5
int bowl = WORMS;
int worms = WORMS;
int numBbirds;

sem_t mutex;
sem_t emptyBowl;
sem_t fullBowl;

void *babyBird(void *arg) {
    int birdId = (int)arg + 1;
    while (1) {
        sem_wait(&mutex);
        if (bowl > 0) {
            bowl--;
            printf("Bird %d eats a worm, bowl has %d worms left\n", birdId, bowl);
            sem_post(&mutex);
        } else {
            printf("Bird %d finds an empty bowl, chirps to refill\n", birdId);
            sem_post(&emptyBowl);
            sem_post(&mutex);  
            sem_wait(&fullBowl);
        }
        usleep(rand() % 100000);
    }
}

void *parentBird(void *arg) {
    while (1) {
        sem_wait(&emptyBowl);
        printf("Parent bird refills the bowl with 10 worms\n");
        sem_wait(&mutex);
        bowl = WORMS;  
        sem_post(&mutex); 
        for (int i = 0; i < numBbirds; i++) {
            sem_post(&fullBowl);
        }
        printf("Bowl refilled with worms\n");
        usleep(rand() % 200000);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Default values used\n");
        numBbirds = BABYBIRDS;
    } else {
        numBbirds = atoi(argv[1]);
    }
    pthread_t birds[numBbirds];
    pthread_t parent;
    sem_init(&emptyBowl, 0, 0);
    sem_init(&fullBowl, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&parent, NULL, parentBird, NULL);

    for (int i = 0; i < numBbirds; i++) {
        pthread_create(&birds[i], NULL, babyBird, (void *)i);
    }

    for (int i = 0; i < numBbirds; i++) {
        pthread_join(birds[i], NULL);
    }

    pthread_join(parent, NULL);
    sem_destroy(&emptyBowl);
    sem_destroy(&fullBowl);
    sem_destroy(&mutex);
    return 0;
}
