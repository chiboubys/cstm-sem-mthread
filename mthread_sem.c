#include <errno.h>
#include "mthread_internal.h"

/* Functions for handling semaphore.  */

    int
mthread_sem_init (mthread_sem_t * sem, unsigned int value)
{
    if(sem == NULL || value ==0)
        return EINVAL;
    sem->lock = 0;
    sem->value = value;
    sem->max = value;
    return 0;
}

/* P(sem), wait(sem) */
    int
mthread_sem_wait (mthread_sem_t * sem)
{
    if(sem == NULL)
        return EINVAL;
    mthread_spinlock_lock(&sem->lock);
    if(sem->value > 0)
        sem->value--;
    else {
        while(sem->value == 0)
            mthread_yield();
        sem->value--;
    }
    mthread_spinlock_unlock(&sem->lock);
    return 0;
}

/* V(sem), signal(sem) */
    int
mthread_sem_post (mthread_sem_t * sem)
{
    if(sem == NULL)
        return EINVAL;
    mthread_spinlock_lock(&sem->lock);
    int ret = 0;
    if(sem->value + 1 > sem->max)
        ret = EOVERFLOW ;
    else
        sem->value++;
    mthread_spinlock_unlock(&sem->lock);

    return ret;
}

    int
mthread_sem_getvalue (mthread_sem_t * sem, int *sval)
{
    if(sem == NULL || sval == NULL)
        return EINVAL;
    mthread_spinlock_lock(&sem->lock);
    *sval = sem->value;
    mthread_spinlock_unlock(&sem->lock);
    return 0;
}

    int
mthread_sem_trywait (mthread_sem_t * sem)
{
    if(sem == NULL)
        return EINVAL;
    if(sem->value == 0)
        return EBUSY;
    return mthread_sem_wait(sem);
}

/* undo sem_init() */
    int
mthread_sem_destroy (mthread_sem_t * sem)
{
    if(sem == NULL)
        return EINVAL;
    mthread_spinlock_lock(&sem->lock);
    int ret = EBUSY;
    if(sem->value == sem->max)
    {
        sem->value = 0;
        ret = 0;
    }
    mthread_spinlock_unlock(&sem->lock);
    return ret;
}
