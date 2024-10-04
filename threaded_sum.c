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
#include <sys/time.h> // for gettimeofda

int readFile(char filename[], int** data);
void* arraySum(void*);
double power(double base, int exp);
typedef struct _thread_data_t;

typedef struct _thread_data_t {
  const int* data;
  int startInd;
  int endInd;
  pthread_mutex_t* lock;
  long long int* totalSum;
} thread_data_t;



int main(int argc, char* argv[]) {
  printf("FUCK1\n");
  int* data = malloc(100000000 * sizeof(int));
  printf("FUCK2\n");

  if (argc != 3) {
    fprintf(stderr, "%s", "ERROR: 3 arguments required -> (1) executable, (2) data filename, (3) number of threads to use\n");
    return -1;
  }

  printf("FUCK3\n");

  if (readFile(argv[1], &data) == -1) {
    fprintf(stderr, "%s", "File not found...\n");
    return -1;
  }

  printf("FUCK4\n");

  // int numThreadsReq = 0;
  // for (int i = 0; i < strlen(argv[2]); i++) {
  //   numThreadsReq += ((*(argv[2] + i) - 48) * power(10, (strlen(argv[2]) - i - 1)));
  // }
  // if (numThreadsReq < numThreadAvail)
  
  return 0;
}

int readFile(char filename[], int** data) {
  printf("FUCK5\n");
  FILE* file = open(filename, "r");
  int curNum = 0;

  printf("FUCK6\n");
  // file argument is assumed not NULL, thus...
  if (file  == NULL) { //file opened successfully?
    return -1;
  }

  printf("Filename: %s\n", filename);

  printf("FUCK7\n");
  int i = 0;
  printf("FUCK10\n");
  while (fscanf(file, "%d", &curNum) != EOF) {
    printf("FUCK9\n");
    (*data)[i] = curNum;
    i++;
    printf("%d\n", i);
  }
  printf("FUCK8\n");

  return i;
}

void* arraySum(void*) {

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