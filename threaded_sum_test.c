#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct _thread_data_t {
    const int *data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

void* arraySum(void* arg) {
    thread_data_t* tdata = (thread_data_t*) arg;
    long long int threadSum = 0;

    // Sum the values in the thread's portion of the array
    for (int i = tdata->startInd; i < tdata->endInd; i++) {
        threadSum += tdata->data[i];
    }

    // Critical section: safely update the global sum
    pthread_mutex_lock(tdata->lock);
    *(tdata->totalSum) += threadSum;
    pthread_mutex_unlock(tdata->lock);

    pthread_exit(NULL);
}

int readFile(const char* filename, int** data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("File not found...\n");
        return -1;
    }

    int value, count = 0;
    while (fscanf(file, "%d", &value) != EOF) {
        (*data)[count++] = value;
    }

    fclose(file);
    return count;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Not enough parameters.\n");
        return -1;
    }

    // Parse command line arguments
    const char* filename = argv[1];
    int threadCount = atoi(argv[2]);

    // Read file and get data
    int* data = malloc(100000000 * sizeof(int));
    int dataSize = readFile(filename, &data);
    if (dataSize == -1) {
        return -1;
    }

    if (threadCount > dataSize) {
        printf("Too many threads requested\n");
        return -1;
    }

    // Initialize total sum and mutex
    long long int totalSum = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    // Timing start
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Create threads and thread data structures
    pthread_t* threads = malloc(threadCount * sizeof(pthread_t));
    thread_data_t* tdata = malloc(threadCount * sizeof(thread_data_t));

    int sliceSize = dataSize / threadCount;
    for (int i = 0; i < threadCount; i++) {
        tdata[i].data = data;
        tdata[i].startInd = i * sliceSize;
        tdata[i].endInd = (i == threadCount - 1) ? dataSize : (i + 1) * sliceSize;
        tdata[i].lock = &lock;
        tdata[i].totalSum = &totalSum;

        pthread_create(&threads[i], NULL, arraySum, &tdata[i]);
    }

    // Join threads
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    // Timing end
    gettimeofday(&end, NULL);
    long long elapsedTime = (end.tv_sec - start.tv_sec) * 1000LL + (end.tv_usec - start.tv_usec) / 1000LL;

    // Output total sum and time taken
    printf("Final sum: %lld\n", totalSum);
    printf("Total time: %lld ms\n", elapsedTime);

    // Clean up
    free(data);
    free(threads);
    free(tdata);
    pthread_mutex_destroy(&lock);

    return 0;
}
