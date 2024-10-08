#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

#define input "input.txt"
#define output "output.txt"
#define time_out "time.txt"

using namespace std;

struct thread_info
{
    int start;
    int end;
};

int result = 0;
vector <int>arr;
long long n = 0;
int total_combinations = 0;
int volatile sorted = 0;


HANDLE mutex;
HANDLE event;


void merge(int l, int mid, int r){
    int n = mid - l + 1;
    int m = r - mid;
    //printf("l=%d mid=%d r=%d\n", l, mid, r);

    vector <int> Left(n), Right(m);

    WaitForSingleObject(event, INFINITE);

    for (int i = 0; i < n; i++)
        Left[i] = arr[l + i];

    for (int i = 0; i < m; i++)
        Right[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = l;
    for (i = 0, j = 0, k = l; i < n && j < m; k++)
        arr[k] = Left[i] > Right[j] ? Right[j++] : Left[i++];
    for (; i < n; i++, k++)
        arr[k] = Left[i];
    for (; j < m; j++, k++)
        arr[k] = Right[j];

    SetEvent(event);
}

void msort(int l, int r){
    if (l >= r)
        return;
    int mid = l + (r - l) / 2;
    msort(l, mid);
    msort(mid + 1, r);
    merge(l, mid, r);
}


DWORD WINAPI thread_entry(void* _data){
   struct thread_info* data = (struct thread_info*) _data;
   //printf("thread entry %d, %d\n", data->start, data->end);
   msort(data->start, data->end);
   return 0;
}


int main()
{
    ofstream output_file(output);
    ofstream time_file(time_out);

    FILE *f = fopen(input, "r");
    int threads_count = 0;
    int init_threads_count = 0;
    fscanf(f, "%d\n%lld", &threads_count, &n);

    // arr = (int *) calloc(n, sizeof(int));
    for(int i = 0; i < n; i++){
        int tmp;
        fscanf(f, "%d", &tmp);
        arr.push_back(tmp);
    }
    fclose(f);

    struct thread_info* thread_info_arr = new struct thread_info[threads_count];
    HANDLE *threads = new HANDLE[threads_count];
    event = CreateEvent(NULL, FALSE, TRUE, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);

    int started, finished;

    if (threads_count >= n)
    {
        printf("too much threads\n");
        started = (int)GetTickCount64();
        msort(0, n - 1);
    }
    else{
        printf("Ya v else\n");
        for(int i = 0; i < threads_count; i++)
        {
            thread_info_arr[i].start = i * (n / threads_count);
            thread_info_arr[i].end = (i + 1) * (n / threads_count) - 1;
        }

        for(int i = 0; i < threads_count; i++)
            threads[i] = CreateThread(0, 0, thread_entry, (void*) &thread_info_arr[i], 0, 0);

        started = (int) GetTickCount64();

        while (WAIT_TIMEOUT == WaitForMultipleObjects(threads_count, threads, TRUE, INFINITE));

        for (int merging_sections = n / threads_count; merging_sections < n; merging_sections *= 2)
            for (int border = 0; border < n - merging_sections; border += 2 * merging_sections)
            {
                if (border + 2 * merging_sections - 1 < n - 1)
                    merge(border, border + merging_sections - 1, border + 2 * merging_sections - 1);
                else
                    merge(border, border + merging_sections - 1, n - 1);
            }
    }

    finished = (int) GetTickCount64();

    for (int i = 0; i < threads_count; i++)
        CloseHandle(threads[i]);    

    CloseHandle(event);
    CloseHandle(mutex);

    time_file << finished - started;
    time_file.close();

    output_file << threads_count << endl << n << endl;
    for (int i = 0; i < n; i++) 
        output_file << arr[i] << " ";
    output_file << endl;
    output_file.close();

    return 0;
}