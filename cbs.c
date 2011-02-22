#include <stdlib.h>
#include "cbs.h"


/* Pointer to CBS hash tables   */
static cbs_hash_t * cbs_hash = NULL;

int cbs_init(void)
{
    cbs_hash = calloc(sizeof(cbs_o), CBS_HASH_1);
    if (!cbs_hash) return(-1);
    return(0);
}


