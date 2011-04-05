
#include "lock.h"

int olock_lock(olock_t *lock)
{
    return(pthread_mutex_lock(lock));
}

int olock_trylock(olock_t *lock)
{
    return(pthread_mutex_trylock(lock));
}

int olock_unlock(olock_t *lock)
{
    return(pthread_mutex_unlock(lock));
}

int olock_init(olock_t * lock)
{
	return(pthread_mutex_init(lock, NULL));
}


int olock_destroy(olock_t * lock)
{
	return(pthread_mutex_destroy(lock));
}


