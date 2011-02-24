#include <stdlib.h>
#include "cbs.h"


/* Pointer to CBS hash tables   */
static cbs_hash1_t 	*   cbs_hash = NULL;
olock_t 				cbs_hash_lock;
static que_t * 			q_id;
static id_t 			id_current = 1;
olock_t 				id_lock;



/********************************** file opearions ***********************************/


static cbs_hash1_t * cbs_hash1_new()
{
	cbs_hash1_t * ps_h;
	ps_h = calloc(1, sizeof(cbs_hash1_t));
	return(ps_h);
}


static int cbs_hash1_free(cbs_hash1_t * ps_h)
{
	free(ps_h);
	return(0);
}


static cbs_hash2_t * cbs_hash2_new()
{
	cbs_hash2_t * ps_h;
	ps_h = calloc(1, sizeof(cbs_hash2_t));
	return(ps_h);
}


static int cbs_hash2_free(cbs_hash2_t * ps_h)
{
	free(ps_h);
	return(0);
}


static cbs_o * cbs_o_new(obj_t * o)
{
	cbs_o * ps_o;
	ps_o = calloc(1, sizeof(cbs_o));
	ps_o->q_employee = que_create();

	if(!ps_o->q_employee) 
	{
		free(ps_o);
		return(NULL);
	}
	
	ps_o->o = o;
	return(ps_o);
}


static int cbs_o_free(cbs_o * ps_o)
{
	if(ps_o->q_employee) que_destroy(ps_o->q_employee);
	free(ps_o);
	return(0);
}


static obj_t * obj_hash1_new(void * blabla)
{
	return( (obj_t *) cbs_hash1_new());
}


static obj_t * obj_cbs_o_new(void * v)
{
	return( (obj_t *) cbs_o_new(v));
}


static obj_t * obj_hash2_new(void * blabla)
{
	return( (obj_t *) cbs_hash2_new());
}

static int obj_hash1_free(obj_t * o)
{
	return(cbs_hash1_free((cbs_hash1_t *) o) );
}


static int obj_hash2_free(obj_t * o)
{
	return(cbs_hash2_free((cbs_hash2_t *) o) );
}

static int obj_cbs_o_free(obj_t * o)
{
	return(cbs_o_free((cbs_o *) o) );
}


/********************************** end file opearions ***********************************/


int cbs_init(void)
{
	/* init objects operations */
	obj_f * pf_hash1;
	obj_f * pf_hash2;
	obj_f * pf_cbs_o;

	pf_hash1 = calloc(1, sizeof(obj_f));
	pf_hash1->new = obj_hash1_new;
	pf_hash1->free = obj_hash1_free;
	obj_f_install(OBJ_TYPE_CBS_HASH1, pf_hash1);

	pf_hash2 = calloc(1, sizeof(obj_f));
	pf_hash2->new = obj_hash2_new;
	pf_hash2->free = obj_hash2_free;
	obj_f_install(OBJ_TYPE_CBS_HASH2, pf_hash2);

	pf_cbs_o = calloc(1, sizeof(obj_f));
	pf_cbs_o->new = obj_cbs_o_new;
	pf_cbs_o->free = obj_cbs_o_free;
	obj_f_install(OBJ_TYPE_CBS_O, pf_cbs_o);


    cbs_hash = (cbs_hash1_t *) obj_new(OBJ_TYPE_CBS_HASH1, NULL);

    if (!cbs_hash) return(-1);

	q_id = que_create();

	if (!q_id)
	{
		free(cbs_hash);
		return(-1);
	}

	olock_init(&id_lock);
    return(0);
}


static int cbs_hash_destroy()
{
	int i;
	int j;

	for (i = 0 ; i < CBS_HASH_1 ; i++)
	{	
		if(cbs_hash->o[i]) 	
		{

			for (j = 0; j < CBS_HASH_2 ; j++)
			{
				if (cbs_hash->o[i]->o[j]) obj_free((obj_t *) cbs_hash->o[i]->o[j]);
			}

			obj_free(&cbs_hash->o[i]->t);
		}
	}

	obj_free(&cbs_hash->t);
	cbs_hash = NULL;

	return(0);
}




int cbs_destroy()
{
	if(!cbs_hash) return(0);

	olock_lock(&id_lock);

	que_destroy_data(q_id);
	cbs_hash_destroy();

	olock_destroy(&id_lock);
	return(0);
}



static id_t get_new_id()
{
	id_t i;
	if(!cbs_hash) return(0);

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

	if(!cbs_hash) return(0);

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
	id_t * ps_id;

	if(! o || !o->id) return(OBJ_E_ID);
	if(!cbs_hash) return(0);

	ps_id = calloc(1,sizeof(id_t));
	
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

	if(CBS_OBJ_WRONG(o) ) return(OBJ_E_ID);

	if(!cbs_hash) return(-1);

	if(! cbs_hash->o[ CBSH1(o->id) ])
		cbs_hash->o[ CBSH1(o->id) ] = (cbs_hash2_t *) obj_new(OBJ_TYPE_CBS_HASH2, NULL);

	/* Level 2 array allocation */

	ps_cbs_o = (cbs_o *) obj_new(OBJ_TYPE_CBS_O, o);

	if ( !ps_cbs_o )
	{
		if ( cbs_hash->o[ CBSH1(o->id) ]->amount == 0 )
		{

			obj_free(&cbs_hash->o[ CBSH1(o->id) ]->t);
			olock_unlock(&cbs_hash->t.lock);
		}
		return(-1);
	}


	olock_lock(&cbs_hash->t.lock);
	olock_lock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);

	cbs_hash->o[ CBSH1(o->id) ]->o[ CBSH2(o->id) ] = ps_cbs_o;
	cbs_hash->o[ CBSH1(o->id) ]->amount++;
	cbs_hash->amount++;

	olock_unlock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);
	olock_unlock(&cbs_hash->t.lock);
	
	return(0);
}



/* Internal function. Called under lock */
static int remove_from_manager(cbs_o * manager, cbs_o * employee)
{
	return( NULL == que_remove_node_by_data(manager->q_employee, (char *) employee) ? 1 : 0 );
}



int cbs_remove_obj(obj_t * o)
{
	cbs_o * ps_cbs_rm;
	cbs_o * ps_cbs_tmp;

	if(CBS_OBJ_WRONG(o) ) return(OBJ_E_ID);
	if (!cbs_hash) return(OBJ_E_INDEX);
	if (!cbs_hash->o[ CBSH1(o->id) ]) return(OBJ_E_INDEX);
	if (!cbs_hash->o[ CBSH1(o->id) ]->o[CBSH2(o->id)]) return(OBJ_E_INDEX);

	/* Get node of this object */
	ps_cbs_rm = (cbs_o *) CBS_OBJ_TO_CBS(o);

	olock_lock(&cbs_hash->t.lock);
	olock_lock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);

	/* Remove it from all employees */
	do
	{
		/* */
		ps_cbs_tmp = (cbs_o *) node_extract_data(ps_cbs_rm->q_employee);
		if (ps_cbs_tmp)	ps_cbs_tmp->manager = NULL;

	} while (ps_cbs_tmp);

	/* Remove it from its manager */
	if (ps_cbs_rm->manager)
		remove_from_manager(ps_cbs_rm->manager, ps_cbs_rm);

	/* Free the cbs object */
	obj_free( (obj_t *) CBS_OBJ_TO_CBS(o));
	CBS_OBJ_TO_CBS(o) = NULL;

	/* Decrease the amount of objects the hashes hold */
	cbs_hash->amount--;
	cbs_hash->o[ CBSH1(o->id) ]->amount--;
	
	/* Now test the amount of objects in 2 level hash. If it 0 then free the memory */	
	if(cbs_hash->o[ CBSH1(o->id) ]->amount == 0)
	{	
		olock_unlock( &((obj_t *) cbs_hash->o[ CBSH1(o->id) ])->lock);
		obj_free( ( obj_t *) cbs_hash->o[ CBSH1(o->id) ]) ;
		cbs_hash->o[ CBSH1(o->id) ] = NULL;
	}
		
	if (cbs_hash->o[ CBSH1(o->id) ]) olock_unlock(&cbs_hash->o[ CBSH1(o->id) ]->t.lock);
	olock_unlock(&cbs_hash->t.lock);
	
	return(0);
}


long cbs_hash_amount()
{
	return(cbs_hash->amount);
}



/* Set m as manager of e (if no other manager) */
/* Set e as employee of m */

int cbs_set_employee(obj_t * m, obj_t * e)
{
	cbs_o * ps_cbs_m;
	cbs_o * ps_cbs_e;

	if (!m || !e) return(OBJ_E_ARG);
	if (CBS_WRONG_ID(m) || CBS_WRONG_ID(e) ) return(OBJ_E_ID);

	ps_cbs_e = CBS_OBJ_TO_CBS(e);
	ps_cbs_m = CBS_OBJ_TO_CBS(m);

	if (!ps_cbs_e || !ps_cbs_m) return(OBJ_E_INDEX);

	/* Set m as a manager */
	ps_cbs_e->manager = ps_cbs_m;

	/* Set e as an employee */
	que_add_data_to_tail(ps_cbs_m->q_employee, (char *) ps_cbs_e);

	return(0);



}


