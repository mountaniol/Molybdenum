/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "cbs.h"
#include "d.h"
#include "dw.h"
#include "lock.h"


int main()
{
	obj_t o;

	cbs_init();

	o.id = cbs_get_id();
	printf("id: %d\n", o.id);
	o.id = cbs_get_id();
	printf("id: %d\n", o.id);

	cbs_return_id(&o);

		o.id = cbs_get_id();
	printf("id: %d\n", o.id);

	cbs_return_id(&o);

	cbs_insert_obj(&o);


	cbs_destroy();
	return(0);
}
