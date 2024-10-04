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

int main(int argc, char* argv[]) {
  char* filename = argv[1];
  
  if (argc != 2) {
    fprintf(stderr, "%s", "ERROR: 2 arguments required -> (1) executable, (2) data filename\n");
    return -1;
  }
  
  printf("argv[1] -> %s\n", argv[1]);
  printf("filename -> %s\n", filename);
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "%s", "Failed to open file :(\n");
    return -1;
  }
  
  long long sum = 0;
  int num;
  while (fscanf(file, "%d", &num) != EOF) {
    sum += num;
  }
  
  printf("SUM: %lld\n", sum);
  
  fclose(file);
  return 0;
}
