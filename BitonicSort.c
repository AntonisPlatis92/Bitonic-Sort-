/*
 bitonic.c 

 This file contains two different implementations of the bitonic sort
        recursive  version :  rec
        imperative version :  impBitonicSort() 
 

 The bitonic sort is also known as Batcher Sort. 
 For a reference of the algorithm, see the article titled 
 Sorting networks and their applications by K. E. Batcher in 1968 


 The following codes take references to the codes avaiable at 

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm 
 */



#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void *recBitonicSort1(void *argst);
void impBitonicSort(void);


int tcnt=0;
pthread_mutex_t mutexsum;
pthread_attr_t attr;
void *status;
pthread_t thread[10];
int maxreached=0;
int NUM_THREADS;
 
struct args{
   int lo;
   int cnt;
   int dir;
   int tid;
};
 struct args *args1;

/** the main program **/  //line63 
int main(int argc, char **argv) {


  printf("Dose to q(Synolo Arithmon)\n");
  int q;
  scanf("%d",&q);
  N=pow(2,q);
  printf("N=%d\n",N);
  a = (int *) malloc(N * sizeof(int));
  int p;
  printf("Dose to p(Synolo Threads)\n");
  scanf("%d",&p);
  NUM_THREADS=pow(2,p);

  args1=(struct args *)malloc(NUM_THREADS * sizeof(struct args));
  init();
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutexsum);
   //print();
  
  return 0;
}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}



/** procedure compare() 
   The parameter dir indicates the sorting direction, ASCENDING 
   or DESCENDING; if (a[i] > a[j]) agrees with the direction, 
   then a[i] and a[j] are interchanged.
**/
inline void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j])) 
    exchange(i,j);
}




/** Procedure bitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}



/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
     int k=cnt/2;
     int tid[2];
     int havch=0;
     if (maxreached==1){
         recBitonicSort(lo, k, ASCENDING); 
     }
     else {
     pthread_mutex_lock (&mutexsum);
     if (tcnt<NUM_THREADS) {
       pthread_attr_init(&attr);
       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
       args1[tcnt].lo=lo;
       args1[tcnt].cnt=k;
       args1[tcnt].dir=ASCENDING;
       args1[tcnt].tid=tcnt;
       tid[havch]=tcnt;
       havch++;
       pthread_create(&thread[tcnt], &attr, recBitonicSort1, (void *)&args1[tcnt]); 
       tcnt++;
       if (tcnt==NUM_THREADS){
           maxreached=1;
       }
       pthread_mutex_unlock (&mutexsum);
     }
     else {
        pthread_mutex_unlock (&mutexsum);
         recBitonicSort(lo, k, ASCENDING); 
     }
     }
     if (maxreached==1){
         recBitonicSort(lo+k, k, DESCENDING); 
     }
     else{
     pthread_mutex_lock (&mutexsum);
     if (tcnt<NUM_THREADS) {
       pthread_attr_init(&attr);
       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
       args1[tcnt].lo=lo+k;
       args1[tcnt].cnt=k;
       args1[tcnt].dir=DESCENDING;
       args1[tcnt].tid=tcnt;
       tid[havch]=tcnt;
       havch++;
       pthread_create(&thread[tcnt], &attr, recBitonicSort1, (void *)&args1[tcnt]); 
       tcnt++;
       if (tcnt==NUM_THREADS){
           maxreached=1;
       }
       pthread_mutex_unlock (&mutexsum);
     }
     else{
        pthread_mutex_unlock (&mutexsum);
        recBitonicSort(lo+k, k, DESCENDING);
     }
     }
     if (havch==1){
         pthread_join(thread[tid[0]], &status);
     }
     else if (havch==2){
         pthread_join(thread[tid[0]], &status);
         pthread_join(thread[tid[1]], &status);
     }
     bitonicMerge(lo, cnt, dir);
    
  }
}


// The thread function 
//Does the same as recBitonicSort   
void *recBitonicSort1(void *argst) {   
   
   
   struct args *args3 = (struct args *)argst;
   int tid1=args3->tid;
   int lo=args3->lo;
   int cnt=args3->cnt;
   int dir=args3->dir;
   int k=cnt/2;
   int tid[2];
   
   int havch=0;
   if (cnt>1) {
      
     if (maxreached==1){
         recBitonicSort(lo, k, ASCENDING); 
     }
     else {
     pthread_mutex_lock (&mutexsum);
     if (tcnt<NUM_THREADS) {
       pthread_attr_init(&attr);
       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
       args1[tcnt].lo=lo;
       args1[tcnt].cnt=k;
       args1[tcnt].dir=ASCENDING;
       args1[tcnt].tid=tcnt;
       tid[havch]=tcnt;
       havch++;
       pthread_create(&thread[tcnt], &attr, recBitonicSort1, (void *)&args1[tcnt]); 
       tcnt++;
       if (tcnt==NUM_THREADS){
           maxreached=1;
       }
       pthread_mutex_unlock (&mutexsum);
     }
     else {
         pthread_mutex_unlock (&mutexsum);
         recBitonicSort(lo, k, ASCENDING); 
     }
     }
     if (maxreached==1){
         recBitonicSort(lo+k, k, DESCENDING); 
     }
     else{
     pthread_mutex_lock (&mutexsum);
     if (tcnt<NUM_THREADS) {
       pthread_attr_init(&attr);
       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
       args1[tcnt].lo=lo+k;
       args1[tcnt].cnt=k;
       args1[tcnt].dir=DESCENDING;
       args1[tcnt].tid=tcnt;
       tid[havch]=tcnt;
       havch++;
       pthread_create(&thread[tcnt], &attr, recBitonicSort1, (void *)&args1[tcnt]); 
       tcnt++;
       if (tcnt==NUM_THREADS){
           maxreached=1;
       }
       pthread_mutex_unlock (&mutexsum);
     }
     else{
        pthread_mutex_unlock (&mutexsum);
        recBitonicSort(lo+k, k, DESCENDING);
     }
     }
     if (havch==1){
         pthread_join(thread[tid[0]], &status);
     }
     else if (havch==2){
         pthread_join(thread[tid[0]], &status);
         pthread_join(thread[tid[1]], &status);
     }
     bitonicMerge(lo, cnt, dir);
    }
   pthread_exit(NULL);
}

/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
    recBitonicSort(0, N, ASCENDING);
}



/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

  int i,j;
  long k;
  
  for (k=2; k<N; k=2*k) {
        for (j=k>>1; j>0; j=j>>1) {
           for (i=0; i<N; i++) {
	      int ij=i^j;
	      if ((ij)>i) {
	         if ((i&k)==0 && a[i] > a[ij]) 
	           exchange(i,ij);
	         if ((i&k)!=0 && a[i] < a[ij])
	            exchange(i,ij);
	         }
              }
           }
        }
  
}


      
