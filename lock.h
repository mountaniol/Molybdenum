#ifndef _lock_h_ksjdfhksdfsdf09sdfsd545s45df4
#define _lock_h_ksjdfhksdfsdf09sdfsd545s45df4

#include <pthread.h>

/* Lock: abstraction of lock */

typedef pthread_mutex_t olock_t;

inline int olock_lock(olock_t *lock)
{
    return(pthread_mutex_lock(lock));
}

inline int olock_unlock(olock_t *lock)
{
    return(pthread_mutex_unlock(lock));
}

#endif /* _lock_h_ksjdfhksdfsdf09sdfsd545s45df4 */
