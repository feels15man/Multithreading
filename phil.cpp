#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#define TCNT 5

sem_t state_sem[TCNT + 1];
// pthread_mutex_t mutex;
pthread_t threads[TCNT];
pthread_t control_thread;

unsigned long t_start = 0;
unsigned long t_end = 0;
unsigned long t_delay = 0;


unsigned long to_ms(struct timespec* tm)
{
  return ((unsigned long) tm->tv_sec * 1000 + (unsigned long) tm->tv_nsec / 1000000);
}


void* thread_entry_phil(void* my_id) {
    int id = (int *) my_id - (int *) 0;
    // printf("id %d\n", id);
    struct timespec time;

    // printf("%d started", id);
    clock_gettime(CLOCK_REALTIME, &time);
    // printf(" : %lu - %lu\n", to_ms(&time), t_end);

    //     int t;
    // sem_getvalue(&state_sem[id], &t);
    // printf("id %d -> SEM_val: %d\n", t);

    while (1) {
        // printf("-------%d wait for sem\n", id);
        sem_wait(&state_sem[id]);
        sem_wait(&state_sem[id]);
        // printf("-------%d got sem\n", id);

        // clock_gettime(CLOCK_REALTIME, &time);
        
        // printf("%d no break\n", id);
        // pthread_mutex_lock(&mutex);

        clock_gettime(CLOCK_REALTIME, &time);
        printf("%lu:%d:T->E\n", to_ms(&time) - t_start, id + 1);
        // pthread_mutex_unlock(&mutex);

        usleep(t_delay * 1000);

        // pthread_mutex_lock(&mutex);
        clock_gettime(CLOCK_REALTIME, &time);
        printf("%lu:%d:E->T\n", to_ms(&time) - t_start, id + 1);
        // pthread_mutex_unlock(&mutex);

        sem_post(&state_sem[5]);
        // printf("-------%d sem_posted\n", id);
    }
    return 0;
}


void* thread_entry_main(void* my_id){
    int phil_eats = 0;
    int phil_eats_id[2] = {0, 2};
    struct timespec time;
    // sem_wait(&state_sem[5]);

    // int t;
    // sem_getvalue(&state_sem[5], &t);
    // printf("SEM_val: %d\n",  t);

    clock_gettime(CLOCK_REALTIME, &time);
    while (t_end > to_ms(&time)){
        // printf("Cycling\n");
        sem_post(&state_sem[phil_eats_id[0]]);
        sem_post(&state_sem[phil_eats_id[0]]);

        sem_post(&state_sem[phil_eats_id[1]]);
        sem_post(&state_sem[phil_eats_id[1]]);
        // printf("-------control waits for sem\n");
        sem_wait(&state_sem[5]);
        sem_wait(&state_sem[5]);

        phil_eats_id[0] = (phil_eats_id[0] + 1) % 5;
        phil_eats_id[1] = (phil_eats_id[1] + 1) % 5;
        clock_gettime(CLOCK_REALTIME, &time);
    }
    // printf("Main out\n");
    return 0;
}


int main(int argc, char const *argv[])
{
  if (argc != 3)
    return -1;

  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

    t_start = to_ms(&start_time);
    t_end = t_start + atoi(argv[1]);
    t_delay = atoi(argv[2]);

    // printf("%lu %lu %lu\n", t_start, t_end, t_delay);

  sem_init(&state_sem[0], 0, 0);
  sem_init(&state_sem[1], 0, 0);
  sem_init(&state_sem[2], 0, 0);
  sem_init(&state_sem[3], 0, 0);
  sem_init(&state_sem[4], 0, 0);
  sem_init(&state_sem[5], 0, 0);

    // sem_wait(&state_sem[0]);

  // pthread_mutex_init(&mutex, 0);

    for (int i = 0; i < TCNT; i++)
        pthread_create(&threads[i], 0, thread_entry_phil, (int *)0 + i);
    pthread_create(&control_thread, 0, thread_entry_main, 0);
    
  //for (int i = 0; i < TCNT; ++i) 
    //pthread_join(threads[i], NULL);
  pthread_join(control_thread, NULL);

    // pthread_mutex_destroy(&mutex);

    for(int i = 0; i < TCNT + 1; i++)
      sem_destroy(&state_sem[i]);

    return 0;
}