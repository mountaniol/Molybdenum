#ifndef _signals_h_jsfsdfsudfs7667sd7f6sdf76s4f6s534f3s25s
#define _signals_h_jsfsdfsudfs7667sd7f6sdf76s4f6s534f3s25s

#include "otypes.h"
#include "lock.h"
#include "obj.h"

typedef enum obj_signum
{
	OBJ_S_RUNNIG = 1,
	OBJ_S_STOPPED,
	OBJ_S_CHANGED,
	OBJ_S_DIE,
	OBJ_S_USER1
} signum_e;

/* An object sends the signal to another object(s) through CBS (Central Bus Station) */
struct obj_signal
{
	signum_e	 	signum;
	id_t			src;
	id_t			dst;
	olock_t 		lock;
	void * 			data;
};

typedef struct obj_signal osig_t;

#endif /* _signals_h_jsfsdfsudfs7667sd7f6sdf76s4f6s534f3s25s */
