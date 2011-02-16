/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "d.h"
#include "dw.h"
int main()
{
	obj_t * sp_a;
    obj_t * sp_b;
    obj_t * sp_result = NULL;

	obj_t * ps_watch;
    int i;

	init_objects();

	obj_t * po_filter = obj_new(OBJ_TYPE_FILTER, NULL);
	ps_watch = 			obj_new(OBJ_TYPE_WATCHER, NULL);

	sp_a =  obj_new(OBJ_TYPE_DIR, "/tmp/a");

	watch_obj(ps_watch, sp_a, 1);

	while(1)
	{
		while(obj_amount(ps_watch) == 0) 
		{
			sleep(1);
		}

		printf("Changed directory\n");
		((obj_watch_t *) ps_watch)->amount = 0;
	}

	return(0);


	if (!sp_a)
	{
		printf("Can't create obj\n");
		return(-1);
	}

    sp_b =  obj_new(OBJ_TYPE_DIR, "/home/vitaly/src/cmd/splitter.orig");

    if (!sp_a || !sp_b)	return(-1);

    sp_result = obj_diff(sp_a, sp_b);

    if(sp_result) 
	{
		printf("sp_result a->b : %d\n", obj_amount(sp_result));
		for (i = 0 ; i < obj_amount(sp_result) ; i++)
			printf("%i: %s\n", i, ((dir_t *)sp_result)->entry[i].name);
		obj_free(sp_result);
	}

	sp_result =  obj_diff(sp_b, sp_a);
    if(sp_result)
	{
		printf("sp_result diff b->a : %d\n", ((dir_t *)sp_result)->amount);
		for (i = 0 ; i < ((dir_t *)sp_result)->amount ; i++)
			printf("%i: %s\n", i, ((dir_t *)sp_result)->entry[i].name);
		obj_free(sp_result);
	}

	obj_free(po_filter);
	obj_free(sp_a);
	obj_free(sp_b);
	obj_free(ps_watch);
    return(0);
  
}
