/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "d.h"
#include "dw.h"
int main()
{
    dir_t * ps_d; //= dir_t_scan2("/home/seva/tmp");

    dir_t * sp_a;
    dir_t * sp_b;
    dir_t * sp_result = NULL;
    int i;
    dwatch_t * ps_w ; // = dwatch_create();

    sp_a = dir_t_scan2("/home/vitaly/src/cmd/splitter");
    sp_b = dir_t_scan2("/home/vitaly/src/cmd/splitter.orig");

    if (!sp_a || !sp_b)	return(-1);

    sp_result = dir_t_diff(sp_a, sp_b);

    if(sp_result) printf("sp_result a->b : %d\n", sp_result->amount);

    for (i = 0 ; i < sp_result->amount ; i++)
        printf("%i: %s\n", i, sp_result->entry[i].name);

	dir_t_free(sp_result);

	sp_result = dir_t_diff(sp_b, sp_a);
    if(sp_result) printf("sp_result diff b->a : %d\n", sp_result->amount);

    for (i = 0 ; i < sp_result->amount ; i++)
        printf("%i: %s\n", i, sp_result->entry[i].name);
	dir_t_free(sp_result);

	sp_result = dir_t_same(sp_b, sp_a);
	if(sp_result) printf("sp_result same b->a : %d\n", sp_result->amount);

	for (i = 0 ; i < sp_result->amount ; i++)
		printf("%i: %s\n", i, sp_result->entry[i].name);


	dir_t_free(sp_result);
	dir_t_free(sp_a);
	dir_t_free(sp_b);

    return(0);

    

    if (!ps_w)
    {
        printf("Can't create watch\n");
        return(-1);
    }

    dwatch_dir(ps_w, ps_d);
    sleep(3);
    dwatch_start(ps_w);

    sleep(100);
    return(0);
}
