#ifndef _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7
#define _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7

#include "e.h"

/* Abstraction level. */
/* obj_t is container which holds other structures */
/* Every structure begins from type obj_t and the obj_t keeps the type of structure and its size  */
/* in file obj_c there is realisation of object manipulation ; creation, deleting, copying */

typedef enum obj_type
{
	OBJ_TYPE_BEGIN = 1,
	OBJ_TYPE_CHAR,
	OBJ_TYPE_SHORT,
	OBJ_TYPE_INT,
	OBJ_TYPE_LONG,
	OBJ_TYPE_LOLGLONG,
	OBJ_TYPE_UCHAR,
	OBJ_TYPE_USHORT,
	OBJ_TYPE_UINT,
	OBJ_TYPE_ULONG,
	OBJ_TYPE_POINTER,
	OBJ_TYPE_ENTRY,
	OBJ_TYPE_DIR,
	OBJ_TYPE_FILTER,
	OBJ_TYPE_FILE,
	OBJ_TYPE_QUE,
	OBJ_TYPE_NODE,
	OBJ_TYPE_WATCHER,
	OBJ_TYPE_DHOLDER,
	OBJ_TYPE_END,

} type_e;


enum obj_operation_e
{
	OBJ_NEW = 1,
	OBJ_FREE,
	OBJ_INIT,
	OBJ_VALID,
	OBJ_DUP,
	OBJ_LOCK,
	OBJ_UNLOCK,
	OBJ_NEXT
};

/* Object structure works behind all */
/* User has not use it in a direct manner */
struct obj_struct
{
	type_e 		type; 	/* Object type 	*/
	size_t 		size;	/* Object size 	*/
	obj_e		error;	/* Last error  	*/
	void * 		data;	/* A pointer	*/
};


typedef struct obj_struct obj_t;


typedef struct obj_operations
{
	obj_t * 	(*obj_new)(void * data);
	int		 	(*obj_free)(obj_t * ps_o);
	obj_e 		(*obj_init)(obj_t * ps_o);
	int			(*obj_valid)(obj_t * ps_o);
	obj_t * 	(*obj_dup)(obj_t * ps_o);
	int			(*obj_lock)(obj_t * ps_o);
	int			(*obj_unlock)(obj_t * ps_o);
	obj_t * 	(*obj_next)(obj_t * ps_o);
} obj_f;


int obj_f_install(type_e, obj_f * o_f);
obj_f * obj_f_get(type_e);


obj_e 		obj_type_validity(obj_t * ps_o, type_e e_type);
obj_e 		obj_init(obj_t * ps_o, type_e t);
obj_t * 	obj_dup(obj_t * ps_o);
obj_t * 	obj_new(type_e t, void * data);
int		 	obj_free(obj_t * ps_o);



#endif /* _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7 */
