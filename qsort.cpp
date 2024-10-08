#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdint>


#define input "input.txt"
#define output "output.txt"
#define time_out "time.txt"
// #define ull unsigned long long

using namespace std;

struct thread_info
{
    int start;
    int end;
};

vector <struct thread_info> queue;

HANDLE sem;
CRITICAL_SECTION cs;

int result = 0;
int *arr = NULL;
long long n = 0;
int total_combinations = 0;
int volatile sorted = 0;


DWORD WINAPI thread_entry(void* param);
int qsort(int *arr, int left, int right);


int main()
{
    //init + read
    FILE *f = fopen(input, "r");
    int threads_count = 0;
    int init_threads_count = 0;
    fscanf(f, "%d\n%d", &threads_count, &n);

    arr = (int *) calloc(n, sizeof(int));
    for(int i = 0; i < n; i++)
        fscanf(f, "%d", &arr[i]);
    fclose(f);

    HANDLE* threads = new HANDLE[threads_count];

    InitializeCriticalSection(&cs);
    int finished, started;
    sem = CreateSemaphore(0, threads_count, threads_count, 0);
    struct thread_info tmp;
    tmp.start = 0;
    tmp.end = n - 1;
    queue.push_back(tmp);

    for (int i = 0; i < threads_count; i++)
        threads[i] = CreateThread(0, 0, thread_entry, 0, 0, 0);

    started = (int) GetTickCount64();

    while (WAIT_TIMEOUT == WaitForMultipleObjects(threads_count, threads, TRUE, INFINITE));

    finished = (int) GetTickCount64();

    for (int i = 0; i < threads_count; i++ )
        CloseHandle(threads[i]);
    DeleteCriticalSection(&cs);


    ofstream output_file(output);
    output_file << threads_count << endl;
    output_file << n << endl;
    for (int j = 0; j < n; j++) 
        output_file << arr[j] << " ";
    output_file << endl;
    output_file.close();

    ofstream time_file(time_out);
    time_file << finished - started;
    time_file.close();

    return 0;
}

DWORD WINAPI thread_entry(void* param)
{
    long old_val;
    struct thread_info* data = (struct thread_info *)param;
    while (sorted < n) {
        WaitForSingleObject(sem, INFINITE);

        EnterCriticalSection(&cs);
        if(!queue.empty()){
            struct thread_info tmp = queue.back();
            queue.pop_back();

            LeaveCriticalSection(&cs);
            int res = qsort(arr, tmp.start, tmp.end);
                
            if(res > 0){
                EnterCriticalSection(&cs);
                sorted += res;
                LeaveCriticalSection(&cs);
                ReleaseSemaphore(sem, 1, &old_val);
            }
        }
        else{
            ReleaseSemaphore(sem, 1, &old_val);
            LeaveCriticalSection(&cs);
        }
    }

    LeaveCriticalSection(&cs);
    ReleaseSemaphore(sem, 1, &old_val);
    return 0;
}


int qsort(int *arr, int left, int right) {
    long old_val;
    if (left >= right) 
        return 1;

    int pivot = arr[(left + right) / 2];

    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    if ((right - left) > 1024) {
        struct thread_info tmp;

        EnterCriticalSection(&cs);

        tmp.start = left;
        tmp.end = j;
        queue.push_back(tmp);
        ReleaseSemaphore(sem, 1, &old_val);

        LeaveCriticalSection(&cs);

        if(i - j > 1){ 

            /* For (1 1 1 1 1 1): */
            /* qsort(1 1 1); 
            !!!1 - unsorted(not included in the variable sorted)!!!; 
            qsort(1 1 1). */

            EnterCriticalSection(&cs);
        
            tmp.start = j + 1;
            tmp.end = i - 1;
            queue.push_back(tmp);
            ReleaseSemaphore(sem, 1, &old_val);
        
            LeaveCriticalSection(&cs);
        }

        EnterCriticalSection(&cs);

        tmp.start = i;
        tmp.end = right;
        queue.push_back(tmp);
        ReleaseSemaphore(sem, 1, &old_val);

        LeaveCriticalSection(&cs);

        return 0;
    }
    else {
        qsort(arr, left, j);
        qsort(arr, i, right);

        return (right - left + 1);
    }
}