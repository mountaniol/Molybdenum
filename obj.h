#ifndef _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7
#define _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7

/* Abstraction level. */
/* obj_t is container which holds other structures */
/* Every structure begins from type obj_t and the obj_t keeps the type of structure and its size  */
/* in file obj_c there is realisation of object manipulation ; creation, deleting, copying */


typedef enum obj_type
{
	OBJ_TYPE_CHAR = 1,
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
	OBJ_TYPE_WATCHER,
	OBK_TYPE_DHOLDER
} type_e;


struct obj_struct
{
	type_e type;
	size_t size;
	void * data;
};


typedef struct obj_struct obj_t;


#endif /* _obj_h_sdfkjhdflajsdhfasdf76sdf76sd7f6asdf7 */
