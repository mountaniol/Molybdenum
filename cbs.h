#ifndef _cbs_s_dkfjhgdkj32jjhkjh9k7j8h9kjhkj
#define _cbs_s_dkfjhgdkj32jjhkjh9k7j8h9kjhkj

#include "obj.h"
#include "l.h"
#include <pthread.h>

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

#define CBS_COUNT_1(id)         (id /   CBS_HASH_2 )
#define CBS_COUNT_2(id)         (id % CBS_HASH_2 )

/* This macro detect if given ID belongs to a hash or not */
#define THIS_HASH(id, num)      (id < (num * CBS_HASH_1 * CBS_HASH_2) )

/* This structure holds an object */
struct cbs_o_struct
{
    obj_t t;
    obj_t * o;                          /* The object */
    obj_t * manager;            /* Manager of this object */
    que_t * q_employee;     /* Linked list of employees of this object  */
};

typedef struct cbs_o_struct cbs_o;

struct cbs_hash_struct
{
    obj_t t;
    cbs_o * o[1024];
    struct cbs_hash_struct * next;
    long amount;    /* Amount: used to free unused nodes. */
    que_t * free_id;
    id_t max_id;            /* Maximal id; never decreases */
};

typedef struct cbs_hash_struct cbs_hash_t; 




#endif /*  _cbs_s_dkfjhgdkj32jjhkjh9k7j8h9kjhkj */
