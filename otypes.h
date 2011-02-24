#ifndef _otypes_h_dshjg23hgjh5jk4634j4
#define _otypes_h_dshjg23hgjh5jk4634j4

typedef enum obj_type
{
    OBJ_TYPE_NONE = 0,          /* This is special type: no type*/
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
	OBJ_TYPE_CBS_HASH1,
	OBJ_TYPE_CBS_HASH2,
    OBJ_TYPE_CBS_O,   
    OBJ_TYPE_END

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

typedef unsigned int id_t;

#endif /* _otypes_h_dshjg23hgjh5jk4634j4 */
