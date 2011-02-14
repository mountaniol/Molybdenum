#ifndef _e_h_ashdfasdftasd542312763583975340968dkjfghd
#define _e_h_ashdfasdftasd542312763583975340968dkjfghd

/* Object errors and warnings */
/* bits 0 - 15: warnongs, */
/* bits 16 - 31 - errors */

typedef enum object_error
{
	OBJ_E_OK = 0,
	OBJ_W_AGAIN,

	OBJ_E_TYPE = 0xFFFF,
	OBJ_E_ARG,
	OBJ_E_OBJ_SIZE,
	OBJ_E_MEMORY
} obj_e ;

#endif /* _e_h_ashdfasdftasd542312763583975340968dkjfghd */

