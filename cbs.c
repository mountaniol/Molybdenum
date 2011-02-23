#include <stdlib.h>
#include "cbs.h"


/* Pointer to CBS hash tables   */
static cbs_hash1_t 	*   cbs_hash = NULL;
olock_t 				cbs_hash_lock;
static que_t * 			q_id;
static id_t 			id_current = 1;
olock_t 				id_lock;


int cbs_init(void)
{
    cbs_hash = calloc(1, sizeof(cbs_hash1_t));
    if (!cbs_hash) return(-1);

	obj_init(&cbs_hash->t, OBJ_TYPE_NONE);

	q_id = que_create();
	if (!q_id)
	{
		free(cbs_hash);
		return(-1);
	}

	olock_init(&id_lock);
	olock_init(&cbs_hash_lock);
    return(0);
}


static int cbs_hash_destroy()
{
	int i;
	int j;
	olock_lock(&cbs_hash->t.lock);
	for (i = 0 ; i < CBS_HASH_1 ; i++)
	{	
		if(cbs_hash->o[i]) 	
		{
			olock_lock(&cbs_hash->o[i]->t.lock);

			for (j = 0; j < CBS_HASH_2 ; j++)
			{
				if (cbs_hash->o[i]->o[j])
				{
					obj_finish((obj_t *) cbs_hash->o[i]->o[j] );
					free(cbs_hash->o[i]->o[j]);					
				}
			}

			obj_finish(&cbs_hash->o[i]->t);
			free(cbs_hash->o[i]);
		}
	}

	obj_finish(&cbs_hash->t);
	free(cbs_hash);

	return(0);
}



int cbs_destroy()
{
	olock_lock(&cbs_hash_lock);
	olock_lock(&id_lock);

	que_destroy_data(q_id);
	cbs_hash_destroy();

	olock_destroy(&id_lock);
	olock_destroy(&cbs_hash_lock);
	
	return(0);
}



static id_t get_new_id()
{
	id_t i;
	olock_lock(&id_lock);
	i = id_current++;
	olock_unlock(&id_lock);

	return(i);
}


/* Get ID */
id_t cbs_get_id()
{
	id_t * ps_id;
	id_t i;

	ps_id = (id_t *) node_extract_data(q_id);

	if(ps_id)
	{
		i = *ps_id;
		free(ps_id);
	}
	else i = get_new_id();

	return(i);
}



/* Return used id to stack of IDs */

int cbs_return_id(obj_t * o)
{
	id_t * ps_id = calloc(1,sizeof(id_t));
	
	if (!ps_id) 
	{
		o->error = OBJ_E_MEMORY;
		return(-1);
	}

	*ps_id = o->id;
	if(!que_add_data_to_tail(q_id, (char *)  ps_id))
	{
		free(ps_id);
		o->error = OBJ_W_AGAIN;
		return(-1);
	}
	return(0);
}



int cbs_insert_obj(obj_t * o)
{
	cbs_o * ps_cbs_o;

	if(! cbs_hash->o[ CBSH1(o->id) ])
	{	
		cbs_hash->o[ CBSH1(o->id) ] = calloc(1, sizeof(cbs_hash2_t));
		obj_init(&cbs_hash->o[ CBSH1(o->id) ]->t, OBJ_TYPE_NONE);
	}

	/* Level 2 array allocation */

	ps_cbs_o = calloc(1, sizeof(cbs_o));

	if (!ps_cbs_o)
	{
		if (cbs_hash->o[ CBSH1(o->id) ]->amount == 0)
		{

			olock_lock(&cbs_hash->t.lock);
			obj_finish(&cbs_hash->o[ CBSH1(o->id) ]->t);
			free(cbs_hash->o[ CBSH1(o->id) ]);
			olock_unlock(&cbs_hash->t.lock);
		}
		return(-1);
	}

	obj_init((obj_t *) ps_cbs_o, OBJ_TYPE_CBS );

	ps_cbs_o->o = o;

	olock_lock(&cbs_hash->t.lock);
	olock_lock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);

	cbs_hash->o[ CBSH1(o->id) ]->o[ CBSH2(o->id) ] = (obj_t *) ps_cbs_o;
	cbs_hash->amount++;

	olock_unlock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);
	olock_unlock(&cbs_hash->t.lock);
	
	return(0);
}
