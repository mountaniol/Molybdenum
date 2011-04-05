#ifndef _cbs_s_dkfjhgdkj32jjhkjh9k7j8h9kjhkj
#define _cbs_s_dkfjhgdkj32jjhkjh9k7j8h9kjhkj

#include "obj.h"
#include "l.h"
#include "lock.h"

/*  CBS (Central Bus Station) is a mechanism which dedicated to several goals: 
    1. It links an object ID to the  object; 
    2. It manages the IDs: cbs_get_id() returns free uniq ID for a new object
    3. It kmows how to find an object by its ID: cbs_get_obj(id) returns pointer 
    3. It keeps relations between objects: manager / employee scheme
    4. Most important: it routes signals from an object to an object / objects and adds pointers to src object and dst object to the signal

    What's more?
*/

/* CBS_HASH_1 - size of high level hash*/
/* CBS_HASH_2 - size of second level hash*/
#define CBS_HASH_1  1024
#define CBS_HASH_2  128

#define CBSH1(id)         (id /   CBS_HASH_2 )
#define CBSH2(id)         (id % CBS_HASH_2 )

/* This macro detect if given ID belongs to a hash or not */
#define THIS_HASH(id, num)      (id < (num * CBS_HASH_1 * CBS_HASH_2) )

/* This structure holds an object */
struct cbs_o_struct
{
    obj_t 					t;
    obj_t * 				o;              /* The object it holds  */
    struct cbs_o_struct * 	manager;        /* Manager of this object */
    que_t * 				q_employee;     /* Linked list of employees of this object  */
};

typedef struct cbs_o_struct cbs_o;

struct cbs_hash2_struct
{
    obj_t 						t;							/* Object abstraction */
    struct cbs_hash_struct * 	next;						/* Pointer to the next */
    long 						amount;    					/* Amount: used to free unused nodes. */
	cbs_o 						*o[CBS_HASH_2];				/* Next level hash / Objects  */
};

typedef struct cbs_hash2_struct cbs_hash2_t; 



struct cbs_hash1_struct
{
    obj_t 						t;					/* Object abstraction */
    struct cbs_hash_struct * 	next;				/* Pointer to the next */
    long 						amount;    			/* Amount: used to free unused nodes. */
	cbs_hash2_t					*o[CBS_HASH_1];				/* Next level hash / Objects  */
};

typedef struct cbs_hash1_struct cbs_hash1_t; 



#define CBS_WRONG_ID(o) (o->id < 1)
#define CBS_WRING_TYPE(o) ((o->type == OBJ_TYPE_CBS_HASH1) || (o->type == OBJ_TYPE_CBS_HASH2) || (o->type == OBJ_TYPE_CBS_O))

#define CBS_OBJ_WRONG(o)	(!o | CBS_WRONG_ID(o) | CBS_WRING_TYPE(o))

#define CBS_ID_TO_CBS(idd) (cbs_hash->o[ CBSH1(idd) ]->o[CBSH2(idd)])
#define CBS_OBJ_TO_CBS(j) (cbs_hash->o[ CBSH1(j->id) ]->o[CBSH2(j->id)])


int 	cbs_init(void);
int 	cbs_destroy();
id_t 	cbs_get_id();
int 	cbs_return_id(obj_t * o);

int 	cbs_insert_obj(obj_t * o);
int 	cbs_remove_obj(obj_t * o);

long 	cbs_hash_amount();
int 	cbs_set_employee(obj_t * m, obj_t * e);

osig_t * cbs_sig_new(id_t dst, id_t src, signum_e signum, void * data);
obj_e 	cbs_signal_free(osig_t * ps_sig);
obj_e 	cbs_remove_signals_from_obj(obj_t * ps_o);
obj_e 	cbs_send_sig_id(id_t src, id_t dst, signum_e signum, void * data);
obj_e 	cbs_signal_reply(osig_t * ps_orig_sig, signum_e signum, void * data);
osig_t * cbs_get_signal(obj_t * o);

static obj_e 	cbs_place_sig(osig_t * ps_s);

#endif /*  _cbs_s_dkfjhgdkj32jjhkjh9k7j8h9kjhkj */
