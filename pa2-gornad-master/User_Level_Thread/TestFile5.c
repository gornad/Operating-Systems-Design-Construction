#define _GNU_SOURCE
#include "threads.h"
#define NUM_THREADS 1
int cleanup_check = 0;    // Used to check if last thread is reached to call cleanup
long long int sum = 0;    // Resource usage

int *arr;


// We replace all kernel fucntions with user thread functions declared in threads.h

// Most of the functionality in this code was borrowed from the other TestFiles
// and from kt.test.c

// Counter follows the same logic as its kernel counterpart
void counter()
{
  long tid;
  pid_t id;
  int temp;
  id = GetMyId();
  int *ar = arr;

  struct timespec begin;
  struct timespec current;
  clock_gettime(CLOCK_MONOTONIC, &begin);
 
  long long int rf = 0;
  while (1)
  {
      rf++;

      clock_gettime(CLOCK_MONOTONIC, &current);
      if (((long long)current.tv_sec - (long long)begin.tv_sec) >= 10.0)
      {
       
        break;
      }
  }
  temp = (int)id%100;
  printf("temp = %d\n", temp);
  ar[temp] = rf;

  // We use cleanup_check to keep track of which thread we are on
  // Once we reach and finish the last thread, we CleanUp()
  // We also introduce the loop from other testfiles to keep track of "usage"
  cleanup_check++;
  if (cleanup_check<NUM_THREADS) {
      DeleteThread(id);
  }
  else {
      for(int i=0 ; i<100 ; i++) {
        printf("arr[i] = %d \n", arr[i]);
        sum += arr[i];
      }
      printf("Sum = %d \n",sum);
      CleanUp();
  }

}

// Sleeping follows the same logic as its kernel counterpart
void sleeping()
{
  long tid;
  pid_t id;
  id = GetMyId();
  int *ar = arr;
  
  int temp;

  struct timespec begin;
  struct timespec current;
  clock_gettime(CLOCK_MONOTONIC, &begin);

  long long int rf = 0;
  while (1)
  {
      rf++;
      clock_gettime(CLOCK_MONOTONIC, &current);
      if (id%2 == 0)
      {
        if(((long long)current.tv_sec - (long long)begin.tv_sec) == 5.0)
         sleep(0.003); //3 seconds (using 3 instead of 0.003 caused segfaults when testing the sleeping function for a single thread)
      }

      if (((long long)current.tv_sec - (long long)begin.tv_sec) >= 10.0)
      {
        break;
      }
  }
  temp = id%100;
  ar[temp] = rf;

  // We use cleanup_check to keep track of which thread we are on
  // Once we reach and finish the last thread, we CleanUp()
  // We also introduce the loop from other testfiles to keep track of "usage"

  cleanup_check++;
  if (cleanup_check<NUM_THREADS) {
      DeleteThread(id);
  }
  else {
      for(int i=0 ; i<100 ; i++) {
        printf("arr[i] = %d \n", arr[i]);
        sum += arr[i];
      }
      printf("Sum = %d \n",sum);
      CleanUp();
  }
}

// Main is borrowed from kt.test
// An if statement inside a loop is added to facilitate code testing
int main(int argc, char const *argv[]) {

  int flag = -1;

  arr = (int *)malloc(100 * sizeof(int));
  long long int sum = 0;

  for(int i=0 ; i<100 ; i++)
    arr[i] = 0;

  setup(0);

// For iteration, create either a sleeping or counter thread  
for(int t=0;t<NUM_THREADS;t++){ 
  if (argc != 2)
  {
      printf("Usage: %s [counter] | [sleeping] \n", argv[0]);
      return 1;
  }
  else if (strcmp(argv[1], "counter") == 0)
  {
      CreateThread(counter,1);
  }
  else if (strcmp(argv[1], "sleeping") == 0)
  {
      CreateThread(sleeping,1);
  }
  else
  {
      return 1;
  }
}
  printf("Code under execution .. \n");

  Go(); //Starts the whole process
  return 0;
}
