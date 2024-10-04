/*
  Author: Robb Northrup
  Class: CS 446.1001
  Date: October 3, 2024
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h> // for pthreads
#include <sys/time.h> // for gettimeofday

int readFile(char filename[], int** data);
 void* arraySum(void* arg);
double power(double base, int exp);
typedef struct _thread_data_t;

typedef struct _thread_data_t {
  const int* data;
  int startInd;
  int endInd;
  pthread_mutex_t* lock;
  long long int* totalSum;
}  thread_data_t;



int main(int argc, char* argv[]) {
  unsigned long maxThreadCount = 0;
  int* data = malloc(100000000 * sizeof(int));
  int numThreadsReq = 0;
  long long int totalSum = 0;

  /*
        CHECK TO ENSURE WE ARE WITHING PARAMETERS...
  */
  //NUM ARGUMENTS CORRECT?
  if (argc != 3){
    fprintf(stderr, "%s", "ERROR: 3 arguments required -> (1) executable, (2) data filename, (3) number of threads to use\n");

    return -1;
  }

  //READFILE, DONE CORRECTLY?
  if ((maxThreadCount = readFile(argv[1], &data)) == -1) {
    fprintf(stderr, "%s", "File not found...\n");
    return -1;
  }

  // printf("oof4\n");
  // printf("Max thread count: %ld\n", maxThreadCount);

  //CHECK # OF REQUESTED THREADS, LESS THAN MAX?
  for (int i = 0; i < strlen(argv[2]); i++) {
    numThreadsReq += ((*(argv[2] + i) - 48) * power(10, (strlen(argv[2]) - i - 1)));
  }
  if (numThreadsReq > maxThreadCount) {
    fprintf(stderr, "%s", "Too many threads requested\n");
    return -1;
  }


  /*
        BEGIN SUMMATION...
  */
  // Initialize mutex (sum already initialized, above)
  pthread_mutex_t lock;
  pthread_mutex_init(&lock, NULL);

  // Start timer!!
  struct timeval start, end;
  gettimeofday(&start, NULL);

  // Gen threads and thred data structures!
  pthread_t* threads = malloc(numThreadsReq * sizeof(pthread_t));
  thread_data_t* tdata = malloc(numThreadsReq * sizeof(thread_data_t));
  
  int sliceSize = maxThreadCount / numThreadsReq;
  for(int i = 0; i < numThreadsReq; i++) {
      tdata[i].data = data;
       tdata[i].startInd = i * sliceSize;
      tdata[i].endInd = (i == numThreadsReq - 1) ? maxThreadCount : (i + 1) * sliceSize;
      tdata[i].lock = &lock;
      tdata[i].totalSum = &totalSum;

      pthread_create(&threads[i], NULL, arraySum, &tdata[i]);
  }

  // Join das threads
  for (int i = 0; i < numThreadsReq; i++) {
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

int readFile(char filename[], int** data) {
  FILE* file = fopen(filename, "r");
  int curNum = 0;

  // filename argument is assumed not NULL, thus...
  if (file == NULL) { //file not opened successfully?
    return -1;
  }

  // printf("Filename: %s\n", filename);

  int i = 0;
  //parse file, load numbers into "data"
  while (fscanf(file, "%d", &curNum) != EOF) {
    (*data)[i] = curNum;
    i++;
  }

  //return count (number of numbers to be summed)
  return i;
}

void* arraySum(void* arg) {
  thread_data_t* tdata = (thread_data_t*) arg;
   long long int threadSum = 0;

  //HeRe I'm summing the thread portion of array... 
  for (int i = tdata->startInd; i < tdata->endInd;  i++ )  {
       threadSum += tdata->data[i];
  }

  //CRITICAL sECTION: update sum In safe manner!! ...
  pthread_mutex_lock(tdata->lock);
   *(tdata->totalSum) += threadSum;
  pthread_mutex_unlock(tdata->lock);

  pthread_exit(NULL);
}

double power(double base, int exp) {
  double result = 1.0;

  int pos_exp = exp < 0 ? -exp : exp;

  for (int i=0; i < pos_exp; i++) {
    result *= base;
  }

  if (exp < 0) {
    return (1.0 / result);
  }

  return result;
}