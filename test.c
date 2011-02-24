/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "cbs.h"
#include "d.h"
#include "dw.h"
#include "lock.h"


int main()
{
	obj_t o;
	int i;

	char error[1024];

	obj_e e;
	obj_t * m;
	obj_t * oo[2];

	init_objects();
	cbs_init();

	m = obj_new(OBJ_TYPE_WATCHER, NULL);
	if (!m || obj_err(m))
	{	
		printf("Can't init dholder: m: %p\n", (void *) m);
		return(0);
	}

	for (i = 0 ; i < 2 ; i++)
	{
		oo[i] = obj_new(OBJ_TYPE_DIR, NULL);
		e = obj_err(oo[i]);
		if (e) 
		{
			obj_err_to_str(oo[i], error, 1024);
			printf("Object creation error : %s\n" ,error);
		}
		cbs_set_employee(m, oo[i]);
	}


	printf("Hash amount: %ld\n", cbs_hash_amount());

	for (i = 0 ; i < 2 ; i++)
		obj_free(oo[i]);

	obj_free(m);

	printf("Hash amount: %ld\n", cbs_hash_amount());

	cbs_destroy();

	return(0);


	o.id = cbs_get_id();
	printf("id: %d\n", o.id);
	o.id = cbs_get_id();
	printf("id: %d\n", o.id);

	cbs_return_id(&o);

		o.id = cbs_get_id();
	printf("id: %d\n", o.id);

	cbs_return_id(&o);

	cbs_insert_obj(&o);
	cbs_remove_obj(&o);

	cbs_destroy();
	return(0);
}
