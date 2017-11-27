#include<bits/stdc++.h>
#include<cstdio>
#include<unistd.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

using namespace std;

static pthread_mutex_t CountLock   		= PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t Cond_CountNonZero = PTHREAD_COND_INITIALIZER;

static int cnt = 0;

void * decrement_count(void * arg)
{
	pid_t id = syscall(__NR_gettid);
	cout << "Thread ID: " << id << " Function: decrement_count, Count:" << cnt << endl;
    pthread_mutex_lock(&CountLock);
    while (cnt == 0)
        pthread_cond_wait(&Cond_CountNonZero, &CountLock);
	cout << "Thread ID: " << id << " Count [" << cnt << "]" << " Decrementing --" << endl;
	sleep(2);
    cnt = cnt - 1;
	/* If increment_count thread does not issue a broadcast signal then will have to issue the below
       signal to wake up the other decrement thread */
    //pthread_cond_signal(&Cond_CountNonZero);  // Signal broadcast is sent to all the waiting threads
    pthread_mutex_unlock(&CountLock);
}

void * increment_count(void * arg)
{
	pid_t id = syscall(__NR_gettid);
	cout << "Thread ID: " << id << " Function: increment_count, Count:" << cnt << endl;
    pthread_mutex_lock(&CountLock);
    if(cnt == 0){
        pthread_cond_broadcast(&Cond_CountNonZero);  // Signal broadcast is sent to all the waiting threads
        //pthread_cond_signal(&Cond_CountNonZero);   // If no broadcast is sent, then the 1'st woken up thread 
                                                     // will have to wake up the remaining thread
		cout << "Thread ID: " << id << " Incrementing ++" << endl;
		sleep(2);
    	cnt = cnt + 2;
	}
    pthread_mutex_unlock(&CountLock);
}

int main(){

	pthread_t IncThread, DecThread1, DecThread2;

	pthread_create(&DecThread1, NULL, decrement_count, NULL);
	sleep(1);
	pthread_create(&DecThread2, NULL, decrement_count, NULL);
	sleep(1);
	pthread_create(&IncThread, NULL, increment_count, NULL);

	if(pthread_join(IncThread, NULL)){
		cout << "Error joining increment thread" << endl;
	} else {
		cout << "Thread increment joined" << endl;
	}
	
	if(pthread_join(DecThread1, NULL)){
		cout << "Error joining decrement thread 1" << endl;
	} else {
		cout << "Thread 1 decrement joined" << endl;
	}

	if(pthread_join(DecThread2, NULL)){
		cout << "Error joining decrement thread 1" << endl;
	} else {
		cout << "Thread 2 decrement joined" << endl;
	}
	cout << cnt << endl;

	return 0;
}
