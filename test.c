/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "d.h"
#include "dw.h"
int main()
{
    dir_t * ps_d= dir_t_scan2("/home/seva/tmp");

    dir_t * sp_a;
    dir_t * sp_b;
    dir_t * sp_result = NULL;
    int i;
    dwatch_t * ps_w ; // = dwatch_create();

    sp_a = dir_t_scan2("/home/seva/src/molybdenum.2");
    sp_b = dir_t_scan2("/home/seva/src/molybdenum");

    if (!sp_a || !sp_b)
    {
        return(-1);
    }
    sp_result = dir_t_find_shortage(sp_a, sp_b);
    if(sp_result) printf("sp_result: %d\n", sp_result->amount);

    for (i = 0 ; i < sp_result->amount ; i++)
    {
        printf("%s\n", sp_result->entry[i].name);
    }

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
