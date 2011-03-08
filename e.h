#ifndef _e_h_ashdfasdftasd542312763583975340968dkjfghd
#define _e_h_ashdfasdftasd542312763583975340968dkjfghd

/* Object errors and warnings */
/* bits 0 - 15: warnongs, */
/* bits 16 - 31 - errors */

typedef enum object_error
{
	/* Warning flags */
	OBJ_E_OK = 0,			/* Operation completed, no errors */
	OBJ_W_AGAIN,			/* Try again, returned if the object locked */

	OBJ_W_DIE,				/* Object is in die cond  */

	/* Error flags */
	OBJ_E_TYPE = 0xFFFF,	/* Asked wrong operation for type */
	OBJ_E_UNKNOWN, 			/* Returned on obj_init if the object type not registred */
	OBJ_E_ARG,				/* Wrong argument */
	OBJ_E_MEMORY,			/* Memory error: can't allocate for example */
	OBJ_E_INDEX,			/* Asked wrong index */
	OBJ_E_ID				/* Wrong ID of object. Reason: uninitialised CBS */
} obj_e ;


struct obj_struct;
int obj_err_to_str(struct obj_struct * o, char * pc_str, int i_strlen);

#endif /* _e_h_ashdfasdftasd542312763583975340968dkjfghd */

