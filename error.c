#include <stdio.h>
#include "obj.h"
#include "e.h"

int obj_err_to_str(obj_t * o, char * pc_str, int i_strlen)
{
	if (!pc_str || i_strlen <= 0) return(-1);

	if (!o)
	{	
		snprintf(pc_str, i_strlen, "Passed pointer to obj == NULL");
		return(0);
	}

	switch(o->error)
	{
	case OBJ_E_OK:
		snprintf(pc_str, i_strlen, "No error");
		break;
	case OBJ_W_AGAIN:
		snprintf(pc_str, i_strlen, "Try again");
		break;
	case OBJ_E_TYPE:
		snprintf(pc_str, i_strlen, "Wrong object type");
		break;
	case OBJ_E_UNKNOWN:
		snprintf(pc_str, i_strlen, "Object not registred");
		break;
	case OBJ_E_ARG:
		snprintf(pc_str, i_strlen, "Wrong argument");
		break;
	case OBJ_E_MEMORY:
		snprintf(pc_str, i_strlen, "Memory error");
		break;
	case OBJ_E_INDEX:
		snprintf(pc_str, i_strlen, "Asked wrong index");
		break;
	case OBJ_E_ID:
		snprintf(pc_str, i_strlen, "Object has wrong ID");
		break;
	default:
		snprintf(pc_str, i_strlen, "The error code is wrong, possibly wrong pointer passed as an object");
	}

	return(0);
}

