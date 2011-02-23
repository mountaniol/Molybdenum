#ifndef _lock_h_ksjdfhksdfsdf09sdfsd545s45df4
#define _lock_h_ksjdfhksdfsdf09sdfsd545s45df4

#include <pthread.h>

/* Lock: abstraction of lock */

typedef pthread_mutex_t olock_t;

int olock_lock(olock_t *lock);
int olock_unlock(olock_t *lock);
int olock_init(olock_t * lock);
int olock_destroy(olock_t * lock);


#endif /* _lock_h_ksjdfhksdfsdf09sdfsd545s45df4 */
