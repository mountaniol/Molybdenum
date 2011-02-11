/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "d.h"
#include "dw.h"
main()
{
    dir_t * ps_d= dir_t_scan2("/home/seva/tmp/Lib");
    dwatch_t * ps_w = dwatch_create();

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
