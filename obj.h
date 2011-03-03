#ifndef _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7
#define _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7

#include "otypes.h"
#include "e.h"
#include "lock.h"
#include "signals.h"
#include "que_s.h"

/* Abstraction level. */
/* obj_t is container which holds other structures */
/* Every structure begins from type obj_t and the obj_t keeps the type of structure and its size  */
/* in file obj_c there is realisation of object manipulation ; creation, deleting, copying */


/* Object structure works behind all */
/* User has not use it in a direct manner */

struct que_struct;

struct obj_struct
{
	type_e 		type; 								/* Object type 		*/
    id_t        id;									/* Object id: uniq 	*/
	ticket_t 	ticket;								/* Unical ticket. id + ticket both identify the object */
    olock_t     lock;								/* Object lock 		*/
	obj_e		error;								/* Last error, used in misc. situations	*/
	void * 		data;								/* A pointer to a data.	*/
	struct que_struct * 	q_sig;								/* Que of signals */
	int			signals;							/* Not null if there a signals in the q_siq */
	int 		(*sighandler)(osig_t * ps_sig);		/* This function invoked when a signal received; if this function == NULL the signal will be rejected */

};

typedef struct obj_struct obj_t;

typedef struct obj_operations
{
	obj_t * 	(*new)(void * data);					/* Create a new object */
	int		 	(*free)(obj_t * ps_o);					/* Remove object and free the memory */
	obj_e 		(*init)(obj_t * ps_o);					/* Initialize the object or reset it */
	int			(*valid)(obj_t * ps_o);					/* Check the this object is valid: has right type at least */
	obj_t * 	(*dup)(obj_t * ps_o);					/* Duplicate given object */
	int			(*lock)(obj_t * ps_o);					/* Lock object if it lockable */
	obj_t * 	(*diff)(obj_t * ps_a, obj_t * ps_b);	/* Conpare 2 objects; return elements that present in object b but aren't present in a */
	obj_t * 	(*same)(obj_t * ps_a, obj_t * ps_b);	/* Compare 2 object; return new object of the same type contains common element of given objects */
	int			(*unlock)(obj_t * ps_o);				/* Unlock the object it it is locked */
	obj_t * 	(*next)(obj_t * ps_o);					/* Return pointer to next object if it has such ability */
	size_t 		(*size)(obj_t * ps_o);					/* Return sizeof(object) of given type */
	size_t 		(*amount)(obj_t * ps_o);				/* return number of elements that this object contains; it is object-dependened */
	int 		(*refresh)(obj_t * ps_o);				/* Refresh: depends on object. If it directory it rescans the directory. If it is an file re-read state of the file */
} obj_f;

int 		init_objects();

int 		obj_f_install(type_e, obj_f * o_f);
obj_f * 	obj_f_get(type_e);

obj_e 		obj_init(obj_t * ps_o, type_e t);
obj_e 		obj_finish(obj_t * ps_o);

obj_e 		obj_type_validity(obj_t * ps_o, type_e e_type);
obj_e 		obj_reset(obj_t * ps_o);
obj_t * 	obj_diff(obj_t * ps_a, obj_t * ps_b);
obj_t * 	obj_same(obj_t * ps_a, obj_t * ps_b);
size_t 		obj_amount(obj_t * ps_o);
obj_t * 	obj_dup(obj_t * ps_o);
obj_t * 	obj_new(type_e t, void * data);
int		 	obj_free(obj_t * ps_o);
int		 	obj_lock(obj_t * ps_o);
int		 	obj_unlock(obj_t * ps_o);
size_t	 	obj_size(obj_t * ps_o);
size_t	 	obj_amount(obj_t * ps_o);
obj_e 		obj_err(obj_t * o);


#endif /* _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7 */
