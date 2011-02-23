#ifndef _signals_h_jsfsdfsudfs7667sd7f6sdf76s4f6s534f3s25s
#define _signals_h_jsfsdfsudfs7667sd7f6sdf76s4f6s534f3s25s

#include "otypes.h"
#include "obj.h"

#define OBJ_S_RUNNIG		(1<<0) 	/* Sender signalizes that he runs */
#define OBJ_S_STOPPED		(1<<1)	/* Sender signalizes that he stopped */
#define OBJ_S_CHANGED		(1<<2)	/* Sender signalizes that he changed */
#define OBJ_S_DIE			(1<<3)	/* The sended command to receiver to die */

/* An object sends the signal to another object(s) through CBS (Central Bus Station) */
struct obj_signal
{
	unsigned int 	signum;
	void * 			data;
	id_t			src;
	id_t			dst;
};

typedef struct obj_signal osig_t;

#endif /* _signals_h_jsfsdfsudfs7667sd7f6sdf76s4f6s534f3s25s */
