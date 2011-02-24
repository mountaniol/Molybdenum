/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE
#include <search.h>
#include "f.h"
#include "d.h"

#define DD(x...) do{}while(0)
#define DD1(x...) do{}while(0)


int dir_t_allocate_entry(dir_t * ps_d, size_t l_num)
{
	ps_d->entry = calloc(l_num, sizeof(entry_t));

	if(!ps_d->entry) return(-1);
	ps_d->entry_allocated = l_num;
	return(0);
}



struct dfilter_t;
/* Allocate more memory for the entries */
int dir_t_resize(dir_t *ps_dir, size_t i_new_capacity)
{
	entry_t * pc_new = realloc(ps_dir->entry, i_new_capacity * sizeof(entry_t) );
	if (!pc_new && i_new_capacity > 0) return(-1);

	ps_dir->entry = pc_new;
	ps_dir->entry_allocated = i_new_capacity;

	/* If it shrinked change also amount value */
	if (ps_dir->amount > i_new_capacity)  ps_dir->amount = i_new_capacity ;
	return(0);
}


int dir_t_reset(dir_t *ps_dir)
{
	if (ps_dir->entry) free(ps_dir->entry);

	ps_dir->amount 			= 0;
	ps_dir->entry_allocated = 0;

	if(ps_dir->filter) dfilter_free(ps_dir->filter);
	return(0);
}


int dir_t_amount(dir_t *ps_dir)
{
	if (ps_dir)	return(ps_dir->amount);
	return(0);
}



/* Allow resize of the dir_t only if the new size is more then old size */
int dir_t_insrease(dir_t *ps_dir, size_t i_new_size)
{
	if (ps_dir->entry_allocated > i_new_size)
		return(-1);

	return( dir_t_resize(ps_dir,i_new_size) );
}



/* Allow resize of the dir_t only if the new size is less then old size */
int dir_t_shrink(dir_t *ps_dir, size_t i_new_size)
{
	if (ps_dir->entry_allocated < i_new_size)
		return(-1);

	return( dir_t_resize(ps_dir,i_new_size) );
}



dir_t * dir_t_create_empty(void)
{
	dir_t *d = calloc(1, sizeof(dir_t));
	if (!d)	return(NULL);

	/* Type definition */
	return(d);
}



dir_t * dir_t_create(void)
{
	dir_t *d = dir_t_create_empty();
	if (!d)	return(NULL);

	if (dir_t_allocate_entry(d, DIR_T_RECORDS))
	{
		free(d);
		return(NULL);		
	}

	return(d);
}



int dir_t_free(dir_t * d)
{
	if (!d)	return(-1);
	if (d->entry) free(d->entry);
	pthread_mutex_destroy(&d->lock);
	free(d);
	return(0);
}


int dir_t_lock(dir_t * ps_dir)
{
	return(pthread_mutex_lock(&ps_dir->lock));
}


int dir_t_unlock(dir_t * ps_dir)
{
	return(pthread_mutex_unlock(&ps_dir->lock));
}


/* Free all allocated memory, zero memory of the struct */
int dir_t_clean(dir_t * ps_dir)
{
	
	if(!ps_dir) return(-1);
	pthread_mutex_lock(&ps_dir->lock);
	if(ps_dir->entry) free(ps_dir->entry);
	pthread_mutex_unlock(&ps_dir->lock);
	pthread_mutex_destroy(&ps_dir->lock);
	bzero(ps_dir, sizeof(dir_t));
	pthread_mutex_init(&ps_dir->lock, NULL);
	return(0);
}


dir_t * dir_t_scan(char *dir_name)
{
	DIR *d;
	struct dirent * f;
	dir_t * dir;
	char filename[FILENAME_MAX];
	int len;
	struct stat st;

	d = opendir(dir_name);
	if (!d)	return(NULL);

	len = sprintf(filename, "%s/", dir_name);

	dir = dir_t_create();
	memcpy(dir->dir, dir_name, strlen(dir_name));

	while ( (f=readdir(d)) != NULL )
	{

		sprintf(filename + len, "%s", f->d_name);
		if (stat(filename, &st))
		{
			perror("stat failed: ");
			continue;
		}

		if (S_ISREG(st.st_mode))dir->entry[dir->amount].type = DIR_TYPE_FILE;
		if (S_ISDIR(st.st_mode))dir->entry[dir->amount].type = DIR_TYPE_DIR;

		memcpy( dir->entry[dir->amount].name, f->d_name, f->d_reclen);

		dir->entry[dir->amount].len = f->d_reclen;
		DD1("found entry: %s\n", dir->entry[dir->amount].name);
		dir->amount++;
		if (dir->amount >= DIR_T_RECORDS)
		{
			DD1("Too much records, scan incomplete\n");
			closedir(d);
			return(dir);
		}
	}
	closedir(d);
	return(dir);
}


dir_t *dir_t_scan_files_only(char *dir_name)
{
	DIR *d;
	struct dirent *f;
	dir_t *dir; 
	char filename[FILENAME_MAX];
	int len;
	struct stat st;

	d = opendir(dir_name);
	if (!d)
	{
		DD("can't open dir %s\n", dir_name);
		return(NULL);
	}

	dir = dir_t_create();
	memcpy(dir->dir, dir_name, strlen(dir_name));

	len = sprintf(filename, "%s/", dir_name);

	while ((f = readdir(d)) != NULL)
	{
		sprintf(filename + len, "%s", f->d_name);
		stat(filename, &st);
		if (! S_ISREG(st.st_mode))
		{
			DD("found NOT a file %s\n", filename);
			continue;
		}

		DD("found a file %s\n", filename);

		memcpy(dir->entry[dir->amount].name, f->d_name, f->d_reclen);
		dir->entry[dir->amount].type = f->d_type;
		dir->entry[dir->amount].len = f->d_reclen;
		dir->amount++;
		if (dir->amount >= DIR_T_RECORDS)
		{
			printf("Too much records, scan incomplete\n");
			closedir(d);
			return(dir);
		}
	}
	closedir(d);
	return(dir);
}


dir_t *dir_t_scan_dirs_only(char *dir_name)
{
	DIR *d;
	struct dirent *f;
	dir_t *dir;
	struct stat st;
	char filename[FILENAME_MAX];
	int len;

	len = sprintf(filename, "%s/", dir_name);

	d = opendir(dir_name);
	if (!d)
		return(NULL);

	dir = dir_t_create();
	memcpy(dir->dir, dir_name, strlen(dir_name));

	while ((f = readdir(d)) != NULL)
	{
		DD1("entry: %s type: %d DIR_TYPE_DIR == %d\n", f->d_name, f->d_type, DIR_TYPE_DIR);

		if (f->d_name[0] != '.')
		{
			strcpy(filename + len, f->d_name);
			if (stat(filename, &st))
			{
				DD1("couldn't stat %s\n", filename);
				continue;
			}

			if ( ! S_ISDIR(st.st_mode))	continue;

			DD1("dir: %s\n", f->d_name);
			memcpy(dir->entry[dir->amount].name, f->d_name, f->d_reclen);
			dir->entry[dir->amount].type = f->d_type;
			dir->entry[dir->amount].len = f->d_reclen;
			dir->amount++;
			if (dir->amount >= DIR_T_RECORDS)
			{
				printf("Too much records, scan incomplete\n");
				closedir(d);
				return(dir);
			}
		}
	}
	closedir(d);
	return(dir);
}


dir_t * dir_t_extract_dirs_by_filter(dir_t *d, char * filter)
{
	dir_t * dd;
	int i;

	if (NULL == d )	return(NULL);

	dd = dir_t_create();
	memcpy(dd->dir, d->dir, FILENAME_MAX);

	for (i = 0 ; i < d->amount ; i++)
	{
		if (d->entry[i].name[0] == '.')	continue;

		if (d->entry[i].type == DIR_TYPE_DIR )
		{
			if (filter)
			{
				if (strstr(d->entry[i].name, filter))
				{
					memcpy( (dd->entry + dd->amount), (d->entry + i), sizeof(entry_t) );
					dd->amount++;
				}
			}
			else
			{
				memcpy( (dd->entry + dd->amount), (d->entry + i), sizeof(entry_t) );
				dd->amount++;
			}
		}
	}
	return(dd);
}


dir_t * dir_t_extract_files_by_filter(dir_t *d, char * filter)
{
	dir_t * dd;
	int i;

	if (NULL == d || NULL == filter) return(NULL);

	dd = dir_t_create();
	memcpy(dd->dir, d->dir, FILENAME_MAX);

	for (i = 0 ; i < d->amount ; i++)
	{
		if (strstr(d->entry[i].name, filter))
		{
			memcpy( (dd->entry + dd->amount), (d->entry + i), sizeof(entry_t) );
			dd->amount++;
		}
	}

	return dd;
}



dir_t * dir_t_extract_regular_files(dir_t *d)
{
	dir_t * dd;
	int i;

	if (NULL == d) return(NULL);

	dd = dir_t_create();
	memcpy(dd->dir, d->dir, FILENAME_MAX);

	for (i = 0 ; i < d->amount ; i++)
	{
		if (strstr(d->entry[i].name, "_rec"))
		{
			memcpy( (dd->entry + dd->amount), (d->entry + i), sizeof(entry_t) );
			dd->amount++;
		}
	}

	return dd;
}



int compare_entries(const void * v_a, const void * v_b)
{
	entry_t * a = (entry_t *) v_a;
	entry_t * b = (entry_t *) v_b;
	return( strcmp(a->name, b->name));
}


int dir_t_sort(dir_t * dir)
{
	qsort((void *) dir->entry, (size_t) dir->amount, (size_t) sizeof(entry_t), compare_entries);
	return(0);
}


/* Just scan. Don't execute stat. */

que_t * dir_t_scan_to_que(char *dir_name)
{
	DIR *d;
	struct dirent *f;
	char filename[FILENAME_MAX];
	int len;
	time_t t;
	entry_t * ps_entry;
	que_t * q;

	q = que_create();
	if ( !q ) return(NULL);

	d = opendir(dir_name);

	if ( !d ) 
	{
		que_destroy(q);
		return(NULL);
	}

	len = sprintf(filename, "%s/", dir_name);
	time(&t);

	while ( (f = readdir(d)) != NULL )
	{
		ps_entry =  malloc(sizeof(entry_t));
		if(!ps_entry)
		{
			printf("Can't allocate entry_t\n");
			continue;
		}

		sprintf(filename + len, "%s", f->d_name);

		/* Get only files with modification time older then 2 sec */

		memcpy((char *) ps_entry->name, (char *) f->d_name, f->d_reclen);
		ps_entry->type = f->d_type;
		ps_entry->len = f->d_reclen;
		que_add_data(q, (char *) ps_entry);
		
	}

	closedir(d);
	return(q);
}


que_t * dir_t_scan_files_only_to_que(char *dir_name)
{
	DIR *d;
	struct dirent *f;
	char filename[FILENAME_MAX];
	int len;
	struct stat st;
	time_t t;
	entry_t * ps_entry;
	que_t * q;

	q = que_create();
	if ( !q ) return(NULL);

	d = opendir(dir_name);
	if ( !d ) return(NULL);

	len = sprintf(filename, "%s/", dir_name);
	time(&t);

	while ( (f = readdir(d)) != NULL )
	{
		ps_entry =  malloc(sizeof(entry_t));
		if(!ps_entry)
		{
			printf("Can't allocate entry_t\n");
			continue;
		}

		sprintf(filename + len, "%s", f->d_name);

		stat(filename, &st);

		if ( ! S_ISREG(st.st_mode) ) 
		{
			printf("File %s isn't file\n", filename);
			continue;
		}

		/* Get only files with modification time older then 2 sec */

		if ( st.st_ctime + 2 >= t )	
		{
			printf("File %s too new\n", filename);
			continue;
		}

		memcpy(ps_entry->name, f->d_name, f->d_reclen);
		memcpy (&ps_entry->s_st, &st, sizeof(struct stat));
		ps_entry->type = f->d_type;
		ps_entry->len = f->d_reclen;
		que_add_data(q, (char *) ps_entry);
	}

	closedir(d);
	return(q);
}



/* stat all files in the dir_t */
/* return 0 on success, number of errors if there were errors */
int dir_t_stat(dir_t * ps_dir)
{
	char filename[FILENAME_MAX];
	int i;
	int i_errors = 0;
	int len;

	len = sprintf(filename, "%s/", ps_dir->dir);

	for (i = 0 ; i < ps_dir->amount ; i++)
	{
		sprintf(filename + len , "%s", ps_dir->entry[i].name);
		if (stat(filename, &ps_dir->entry[i].s_st) ) i_errors++;
	}

	return(i_errors);
}


int dir_t_add_dfilter(dir_t * ps_dir, dfilter_t * ps_f)
{
	if (!ps_dir->filter) ps_dir->filter = ps_f;
	else(dfilter_add_to_dfilter(ps_dir->filter, ps_f));

	return(0);
}


int dir_t_del_dfilter(dir_t * ps_dir)
{
	dfilter_destroy(ps_dir->filter);
	ps_dir->filter = NULL;

	return(0);
}


dir_t * dir_t_scan2(char *dir_name)
{
	dir_t * d;

	que_t * q = dir_t_scan_to_que(dir_name);
	char * pc_entry;

	if (!q) return(NULL);

	d = dir_t_create_empty();

	if (dir_t_allocate_entry(d, q->amount))
	{
		printf("Can't allocate memory for entry\n");
		que_destroy(q);
		return(NULL);
	}

	while(que_amount(q) > 0)
	{
		pc_entry = node_extract_data(q);
		memcpy(&(d->entry[d->amount++]), pc_entry, sizeof(entry_t));
		free(pc_entry);
	}

	que_destroy(q);

	strcpy(d->dir, dir_name);

	return(d);
}



dir_t * dir_t_scan_filter(char *dir_name, dfilter_t * ps_filter)
{
	dir_t * d;

	que_t * q = dir_t_scan_to_que(dir_name);
	char * pc_entry;
	int j;

	if (!q) return(NULL);

/*	printf("Scanned to que: %d\n", q->amount); */

	d = dir_t_create_empty();

	if (dir_t_allocate_entry(d, q->amount))
	{
		printf("Can't allocate memory for entry\n");
		que_destroy(q);
		return(NULL);
	}

	j = que_amount(q);

	while(que_amount(q) > 0)
	{
		pc_entry = node_extract_data(q);

		if (0 == dfilter_test_entry(ps_filter, (entry_t *) pc_entry) )
			memcpy(&(d->entry[d->amount++]), pc_entry, sizeof(entry_t));

		free(pc_entry);
	}

	que_destroy(q);

	strcpy(d->dir, dir_name);

	if (j > d->amount) dir_t_resize(d, d->amount);

	return(d);
}


/* Rescan the directory IF the directory changed */
int dir_t_refresh(dir_t * ps_dir)
{
	dir_t * ps_new;

	if (!ps_dir) return(-1);

	dir_t_lock(ps_dir);
	dir_t_resize(ps_dir, 0);
	dir_t_unlock(ps_dir);

	ps_new = dir_t_scan_filter(ps_dir->dir, ps_dir->filter);
	if (ps_dir->amount)
	{
		ps_dir->amount = ps_new->amount;
		ps_dir->entry_allocated = ps_new->entry_allocated;
		ps_dir->entry = ps_new->entry;
	}

	ps_new->entry = NULL;
	ps_new->amount = ps_new->entry_allocated = 0;
	dir_t_free(ps_new);
	return(0);
}

/* This function finds difference between dir_a and dir_b*/
/* It returns structure with list of files of dir_b that doesn't present in dir_a */
/* Actually it says what you need to copy from B to A */

dir_t  * dir_t_diff(dir_t * sp_a, dir_t * sp_b)
{
    struct hsearch_data * htab = calloc(1, sizeof(struct hsearch_data));
    ENTRY e;
    ENTRY *ep = NULL;

    dir_t * sp_result =     NULL;
    que_t * ps_q =          NULL;
    entry_t * ps_entry =   	NULL;
    int i;

    if( 0 == hcreate_r( (sp_a->amount * 2),   htab))  
	{
		free(htab);
		return(NULL);
	}

    for(i = 0 ; i<sp_a->amount ; i++)
    {
        e.key = sp_a->entry[i].name;
        e.data = (void *) &sp_a->entry[i];
        hsearch_r(e, ENTER, &ep, htab);

        if (!ep)
        {
            printf("Creation of hash table fell\n");
            hdestroy_r(htab);
			free(htab);
            return(NULL);
        }
    }

    ps_q = que_create();
    if(!ps_q)
    {
        printf("Can't allocate memory for que\n");
        hdestroy_r(htab);
		free(htab);
        return(NULL);
    }


    /* Now search in the hash  */
    for(i = 0 ; i < sp_b->amount; i++)
    {
        e.key = sp_b->entry[i].name;
        hsearch_r(e, FIND, &ep, htab);
        if (!ep)
            que_add_data(ps_q, (char *) &sp_b->entry[i]);
    }

    hdestroy_r(htab);
	free(htab);

    if(ps_q->amount > 0)
    {
        sp_result =  dir_t_create_empty();
        dir_t_allocate_entry(sp_result, ps_q->amount);

		sp_result->amount = 0;

        while(ps_q->amount > 0)
        {
            ps_entry = (entry_t *) node_extract_data(ps_q);
            if(!ps_entry)  break;
            memcpy((char *) &sp_result->entry[sp_result->amount++],  (char *) ps_entry, sizeof(entry_t));
        }
    }   
    
	que_destroy(ps_q);
    return(sp_result);
}




/* This function finds files with same names in dir_a and dir_b*/
dir_t  * dir_t_same(dir_t * sp_a, dir_t * sp_b)
{
    struct hsearch_data * htab = calloc(1, sizeof(struct hsearch_data));
    ENTRY e;
    ENTRY *ep = NULL;

    dir_t * sp_result =     NULL;
    que_t * ps_q =          NULL;
    entry_t * ps_entry =   	NULL;
    int i;

    if( 0 == hcreate_r( (sp_a->amount * 2),   htab))  
	{
		free(htab);
		return(NULL);
	}

    for(i = 0 ; i<sp_a->amount ; i++)
    {
        e.key = sp_a->entry[i].name;
        e.data = (void *) &sp_a->entry[i];
        hsearch_r(e, ENTER, &ep, htab);

        if (!ep)
        {
            printf("Creation of hash table fell\n");
            hdestroy_r(htab);
			free(htab);
            return(NULL);
        }
    }

    ps_q = que_create();
    if(!ps_q)
    {
        printf("Can't allocate memory for que\n");
        hdestroy_r(htab);
		free(htab);
        return(NULL);
    }


    /* Now search in the hash  */
    for(i = 0 ; i < sp_b->amount; i++)
    {
        e.key = sp_b->entry[i].name;
        hsearch_r(e, FIND, &ep, htab);
        if (ep)
            que_add_data(ps_q, (char *) &sp_b->entry[i]);
    }

    hdestroy_r(htab);
	free(htab);

    if(ps_q->amount > 0)
    {
        sp_result =  dir_t_create_empty();
        dir_t_allocate_entry(sp_result, ps_q->amount);

		sp_result->amount = 0;

        while(ps_q->amount > 0)
        {
            ps_entry = (entry_t *) node_extract_data(ps_q);
            if(!ps_entry)  break;
            memcpy((char *) &sp_result->entry[sp_result->amount++],  (char *) ps_entry, sizeof(entry_t));
        }
    }   
    
	que_destroy(ps_q);
    return(sp_result);
}



/* dir_t duplication */
obj_t * obj_copy_dir_t(obj_t * ps_o)
{
	int i;
	dir_t * ps_dir_s = (dir_t *) ps_o;
	dir_t * ps_dir_d = NULL;

	ps_dir_d = dir_t_create_empty();
	if (!ps_dir_d) return(NULL);
	if (ps_dir_s->amount) 
	{	
		i = dir_t_allocate_entry(ps_dir_d, ps_dir_s->amount);
		if (i) goto obj_copy_dir_error;
	}

	memcpy(ps_dir_d->entry, ps_dir_s->entry, sizeof(entry_t) * ps_dir_s->amount);
	memcpy(ps_dir_d->dir, ps_dir_s->dir, FILENAME_MAX);
	
	ps_dir_d->amount = ps_dir_s->amount;
	ps_dir_d->entry_allocated = ps_dir_s->amount;

	if(ps_dir_s->filter) 
	{	
		ps_dir_d->filter = (dfilter_t *) obj_dup( (obj_t * ) ps_dir_s->filter);
		if(!ps_dir_d->filter) goto obj_copy_dir_error;
	}
	
	ps_dir_d->flags = ps_dir_s->flags;
	ps_dir_d->interval = ps_dir_s->interval;
	memcpy(&ps_dir_d->stat, &ps_dir_s->stat, sizeof(struct stat));
	ps_dir_d->ticket = ps_dir_s->ticket;

	return( (obj_t * ) ps_dir_d );

obj_copy_dir_error:
	if(ps_dir_d) dir_t_free(ps_dir_d);
	return(NULL);
}


/********************************************************/
/* Interface for obj_t abstraction */
/********************************************************/


static int obj_dir_t_free(obj_t * ps_o)
{
	return(dir_t_free((dir_t *) ps_o) );
}


static obj_e obj_dir_t_init(obj_t * ps_o)
{
	if(dir_t_reset((dir_t *) ps_o) )
		return(OBJ_E_MEMORY);
	return(OBJ_E_OK);
}

static int obj_dir_t_lock(obj_t * ps_o)
{
	dir_t * ps_d = (dir_t *) (ps_o);
	return pthread_mutex_lock(&ps_d->lock);
}

static int obj_dir_t_unlock(obj_t * ps_o)
{
	dir_t * ps_d = (dir_t *) (ps_o);
	return pthread_mutex_unlock(&ps_d->lock);
}

static obj_t * obj_dir_t_new(void * pv_d)
{
	if (!pv_d) return((obj_t *)dir_t_create_empty());
	return ((obj_t *)dir_t_scan2((char *) pv_d));
}

static int obj_dir_t_valid(obj_t * ps_o)
{
	if (ps_o->type == OBJ_TYPE_DIR) return(OBJ_E_OK);
	return(OBJ_E_TYPE);
}


static size_t obj_dir_t_amount(obj_t * ps_o)
{
	return(((dir_t *)ps_o)->amount);
}


static obj_t * obj_dir_t_diff(obj_t * sp_a, obj_t * sp_b)
{
	return((obj_t * ) dir_t_diff( (dir_t *) sp_a, (dir_t *) sp_b) );
}

static obj_t * obj_dir_t_same(obj_t * sp_a, obj_t * sp_b)
{
	return((obj_t * ) dir_t_same( (dir_t *) sp_a, (dir_t *) sp_b) );
}



void obj_dir_init_me()
{
	obj_f * pf = calloc(1, sizeof(obj_f));
	pf->dup = obj_copy_dir_t;
	pf->free = obj_dir_t_free;
	pf->init = obj_dir_t_init;
	pf->lock = obj_dir_t_lock;
	pf->unlock = obj_dir_t_unlock;
	pf->new = obj_dir_t_new;
	pf->next = NULL;
	pf->valid = obj_dir_t_valid;
	pf->diff = obj_dir_t_diff;
	pf->same = obj_dir_t_same;
	pf->amount = obj_dir_t_amount;

	obj_f_install(OBJ_TYPE_DIR, pf);
}
