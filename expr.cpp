#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define input "input.txt"
#define output "output.txt"
#define time_out "time.txt"


struct thread_info
{
	int start;
    int end;
};


pthread_t *tids;
pthread_mutex_t mutex;

int result = 0;
int S = 0;
int *nums = NULL;
int N = 0;
int total_combinations = 0;


unsigned long to_ms(struct timespec* tm)
{
	return ((unsigned long) tm->tv_sec * 1000 + (unsigned long) tm->tv_nsec / 1000000);
}

void* thread_proc(void* param)
{
// Индекс потока передается в param
	struct thread_info *data = (struct thread_info *) param;
	int res = 0;

	for (int combination = data->start; combination < data->end && combination < total_combinations; combination++){
		int sum = nums[0];
		
		for (int j = 0; j < N - 1; ++j) {
            if (combination & (1 << j)) 
                sum += nums[j + 1];
            else
                sum -= nums[j + 1];
        }
        if (sum == S)
		    res++;
	}
	pthread_mutex_lock(&mutex);
	result += res;
	pthread_mutex_unlock(&mutex);
	return 0;
}


int main()
{
	//init + read
	struct thread_info* threads = NULL;
	FILE *f = fopen(input, "r");
	int threads_count = 0;
	int init_threads_count = 0;
	fscanf(f, "%d\n%d", &threads_count, &N);
	init_threads_count = threads_count;

	nums = (int *) calloc(N, sizeof(int));
	for(int i = 0; i < N; i++)
		fscanf(f, "%d", &nums[i]);
	fscanf(f, "%d", &S);
	fclose(f);

	pthread_mutex_init(&mutex, 0);
	tids = (pthread_t *) calloc(threads_count, sizeof(pthread_t));
	threads = (struct thread_info *) calloc(threads_count, sizeof(struct thread_info));
	total_combinations = 1 << (N - 1);

    if (total_combinations < threads_count)
    	threads_count = total_combinations;

	int combinations_per_thread = total_combinations / threads_count;
    int remaining_combinations = total_combinations % threads_count;

	int start = 0, end = combinations_per_thread;
	for(int i = 0; i < threads_count; i++){
		threads[i].start = start;

		if (remaining_combinations && i < remaining_combinations)
			end++;

		threads[i].end = end;
		start = end;
		end = start + combinations_per_thread;
	}
	//time
	struct timespec started, finished;
	clock_gettime(CLOCK_REALTIME, &started);

	for(int i = 0; i < threads_count; i++)
		pthread_create(&tids[i], 0, thread_proc, (void *) &threads[i]);

 	for (int i = 0; i < threads_count; ++i) 
        pthread_join(tids[i], NULL);

    clock_gettime(CLOCK_REALTIME, &finished);

    pthread_mutex_destroy(&mutex);
    free(tids);
    free(threads);
    free(nums);

    f = fopen(output, "w");
    fprintf(f, "%d\n", init_threads_count);
    fprintf(f, "%d\n", N);
    fprintf(f, "%d\n", result);
    fclose(f);

    f = fopen(time_out, "w");
    fprintf(f, "%lu\n", to_ms(&finished) - to_ms(&started));
    fclose(f);
	return 0;
}