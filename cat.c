#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

int n;

pthread_mutex_t lockA, lockB;

typedef struct ThreadA{
	int count;
	int *a;
	pthread_mutex_t lock;
}thread_a;

typedef struct ThreadB{
	int count;
	int *b;
	pthread_mutex_t lock;
}thread_b;
int conditionA, conditionB;

thread_a A;
thread_b B;
sem_t *barrier1, *barrier2;


static void * A_threads(void *args){
	 pthread_detach (pthread_self ());	
	int *id = (int *)args;
	sleep(1);
	
	pthread_mutex_lock(&B.lock);
		A.a[A.count] = *id;
		A.count++;
		if(A.count % 2 == 0){
			printf("A %d cats A%d A%d\n",*id, A.a[A.count-1], A.a[A.count-2]);
			sem_post(barrier1);
		}	
	pthread_mutex_unlock(&B.lock);
		
	sem_wait(barrier1);
	conditionA = 1;
	if(conditionB != 0){
		printf("A %d merges A%d A%d B%d B%d\n", *id, A.a[A.count-1], A.a[A.count-2], B.b[B.count-1], A.a[B.count-2]);
		conditionA = 0;
	}
	pthread_exit(NULL);
}

static void * B_threads(void *args){
	 pthread_detach (pthread_self ());	
	int *id = (int *)args;
	sleep(1);
	
	pthread_mutex_lock(&B.lock);
		B.b[B.count] = *id;
		B.count++;
		if(B.count % 2 == 0){
			printf("B %d cats B%d B%d\n",*id, B.b[B.count-1], B.b[B.count-2]);
			sem_post(barrier2);
		}
	pthread_mutex_unlock(&B.lock);
	
	sem_wait(barrier2);
	conditionB = 1;
	if(conditionA != 0){
		printf("B%d merges B%d B%d A%d A%d\n", *id, B.b[B.count-1], B.b[B.count-2], A.a[A.count-1], A.a[A.count-2]);
		conditionB = 0;	
	}
	pthread_exit(NULL);
}

void init(){
	conditionA = 0;
	conditionB = 0;
	barrier1 = (sem_t *)malloc(sizeof(sem_t));
	sem_init(barrier1, 0, 0);
	barrier2 = (sem_t *)malloc(sizeof(sem_t));
	sem_init(barrier2, 0, 0);
	
	
	A.count = 0;
	A.a = (int *)malloc(n * sizeof(int));
	
	B.count = 0;
	B.b =  (int *)malloc(n * sizeof(int));
	pthread_mutex_init(&A.lock, NULL);
	pthread_mutex_init(&B.lock, NULL);
}

int main(int argc, char * argv[]){
	
	if(argc!=2){
		printf("usage: %s n\n",argv[0]);
		exit(1);
	}
	n = atoi(argv[1]);
	pthread_t *thA, *thB;
	setbuf(stdout,0);
	
	init();
	
	thA = (pthread_t *)malloc(n * sizeof(pthread_self)); 
	thB = (pthread_t *)malloc(n * sizeof(pthread_self)); 
	
	pthread_mutex_init(&lockA, NULL);
	pthread_mutex_init(&lockB, NULL);
	int *pi, i;
	
	for(i = 0; i<n; i++){
		pi = (int *)malloc(sizeof(int));	
		*pi = i;	
		pthread_create(&thA[i], NULL, A_threads, (void *)pi);
		sleep(1);
		pthread_create(&thB[i], NULL , B_threads, (void *)pi);
		sleep(1);			
	}
	 pthread_exit ((void *) pthread_self());

}

