#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include "mthread.h"

#define NB_THREADS 6

#define CNRM  "\x1B[0m"
#define CRED "\x1B[31m"
#define CGREEN "\x1B[32m"
#define CBLUE "\x1B[34m"

int counter = 0;
int scounter = 0;

mthread_mutex_t normal_init_mutex;
mthread_sem_t sem;

void *run_routine(void *arg){
    mthread_mutex_lock(&normal_init_mutex);
    counter++;
    mthread_mutex_unlock(&normal_init_mutex);
    mthread_sem_wait(&sem);
    scounter++;
    mthread_sem_post(&sem);
    return NULL;
}

int main(int argc, char** argv){

    printf("- Mthread library Semaphores tests\n");
    // Basic NULL tests
    printf("%s[1] Basic Mutex NULL tests :\n", CBLUE);
    printf("%s",CNRM);
    assert(mthread_mutex_init(NULL, NULL) == EINVAL);
    assert(mthread_mutex_trylock(NULL) == EINVAL);
    assert(mthread_mutex_unlock(NULL) == EINVAL);
    printf("%s>> Test [1] passed\n", CGREEN);
    printf("%s",CNRM);
    printf("%s[2] Basic Semaphores NULL tests :\n", CBLUE);
    printf("%s",CNRM);
    assert(mthread_sem_init(NULL, 0) == EINVAL);
    assert(mthread_sem_init(&sem, 0) == EINVAL);
    assert(mthread_sem_wait(NULL) == EINVAL);
    assert(mthread_sem_post(NULL) == EINVAL);
    assert(mthread_sem_getvalue(NULL, NULL) == EINVAL);
    assert(mthread_sem_trywait(NULL) == EINVAL);
    assert(mthread_sem_destroy(NULL) == EINVAL);
    printf("%s>> Test [2] passed\n", CGREEN);
    printf("%s",CNRM);

    printf("%s[3] Semaphores Value tests :\n", CBLUE);
    printf("%s",CNRM);
    int sem_val = 6;
    assert(mthread_sem_getvalue(NULL, &sem_val) == EINVAL);
    assert(sem_val == 6);
    assert(mthread_sem_getvalue(&sem, NULL) == EINVAL);
    const unsigned int value = 3;
    printf("+ Initializing semaphore with value = %d\n", value); 
    assert(mthread_sem_init(&sem, value) == 0);
    assert(mthread_sem_getvalue(&sem, &sem_val) == 0);
    assert(sem_val == value);
    printf("%s>> Test [3] passed\n", CGREEN);
    printf("%s",CNRM);

    printf("%s[4] Semaphores Wait & Trywait tests :\n", CBLUE);
    printf("%s",CNRM);

    sem_val = -1;
    printf("+ Sempahore wait = %d\n", value); 
    unsigned int s; 
    for(s = 1; s <= value; s++) {
        assert(mthread_sem_wait(&sem) == 0);
        assert(mthread_sem_destroy(&sem) == EBUSY);
        assert(mthread_sem_getvalue(&sem, &sem_val) == 0);
        assert(sem_val == value - (int) s);
    }
    printf("+ Sempahore Trywait = %d\n", value); 
    assert(mthread_sem_trywait(&sem) == EBUSY);
    printf("+ Sempahore Post \n"); 
    for(s = 1; s <= value; s++) {
        assert(mthread_sem_destroy(&sem) == EBUSY);
        assert(mthread_sem_post(&sem) == 0);
        assert(mthread_sem_getvalue(&sem, &sem_val) == 0);
        assert(sem_val == (int) s);
    }
    printf("%s>> Test [4] passed\n", CGREEN);
    printf("%s",CNRM);

    printf("%s[5] Semaphores Destroy tests :\n", CBLUE);
    printf("%s",CNRM);
    assert(mthread_sem_destroy(&sem) == 0);
    printf("+ Semaphore destroyed\n");
    printf("%s>> Test [5] passed\n", CGREEN);
    printf("%s",CNRM);

    printf("%s[6] Semaphores Reinitialization after destroying tests :\n", CBLUE);
    printf("%s",CNRM);
    assert(mthread_sem_init(&sem, 1) == 0);
    printf("+ Semaphore successfully initialized\n");
    printf("%s>> Test [6] passed\n", CGREEN);
    printf("%s",CNRM);

    mthread_t pids[NB_THREADS] = {0};
    unsigned int t;
    printf("%s[7] Semaphore with threads\n", CBLUE);
    printf("%s",CNRM);
    printf("\t+ Creating %d mthreads\n", NB_THREADS);
    for(t=0; t<NB_THREADS; t++){
        mthread_create(&(pids[t]), NULL, run_routine, NULL);
        printf("\t\t+ Thread %d/%d created.\n", t, NB_THREADS);
    }
    printf("\t To check counter value we will use a mutex to compare with semaphore value\n");
    printf("\t+ Joining %d mthreads\n", NB_THREADS);
    for(t=0; t<NB_THREADS; t++){
        mthread_join(pids[t], NULL);
        printf("\t\t+ Thread %d/%d joined.\n", t, NB_THREADS);
    }
    printf("+ %d mthreads joined\n", NB_THREADS);
    printf("+ %d Checking counter value after joining\n", NB_THREADS);
    assert((counter == scounter) && (scounter == NB_THREADS));
    printf("+ %d Counter value is correct\n", NB_THREADS);
    printf("%s>> Test [7] passed\n", CGREEN);
    printf("%s--------- ALL TESTS PASSED SUCCESSFULLY ---------\n", CGREEN);

    printf("%s",CNRM);
    return 0;
}
