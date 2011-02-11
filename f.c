/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#define _GNU_SOURCE

#include "d.h"
#include "f.h"

dfilter_t * dfilter_create()
{
	return(calloc(sizeof(dfilter_t), 1));
}


int dfilter_destroy(dfilter_t * sp_filter)
{
	if (sp_filter->pc_data) free(sp_filter->pc_data);
	free(sp_filter);
	return(0);
}


char * dfilter_copy_data(efilter_t e, char * pc_data)
{
	switch(e)
	{
	case FILTER_SIZE:
	case FILTER_SIZE_LESS:
	case FILTER_SIZE_MORE:
		{
			long * pl_new = malloc(sizeof(unsigned long) );
			if (!pl_new) return(NULL);
			memcpy(pl_new, pc_data, sizeof(unsigned long));
			return((char *) pl_new );
		}
		break;

	case FILTER_FULL_NAME:
	case FILTER_PART_NAME:
	case FILTER_PART_INSENSITIVE_NAME:
	case FILTER_EXTENTION:
	case FILTER_HAS_NOT:
	case FILTER_HAS_NOT_INSENSITIVE:

		return(strdup(pc_data));
		break;

	case FILTER_TYPE:
	case FILTER_MODE:
		{
			mode_t * pm_new = malloc(sizeof(mode_t));
			if (!pm_new) return(NULL);
			memcpy(pm_new, pc_data, sizeof(mode_t));
			return( (char *) pm_new);
		}
		break;
	}
	return(NULL);
}


dfilter_t * dfilter_create_with_params(efilter_t e, char * pc_data)
{
	dfilter_t * sp_f = dfilter_create();

	if (! sp_f) return(NULL);

	sp_f->e_ftype = e;	
	sp_f->pc_data = dfilter_copy_data(e,pc_data);
	if (!sp_f->pc_data)
	{
		free(sp_f);
		return(NULL);
	}

	return(sp_f);
}


int dfilter_add_data(dfilter_t * sp_f, char * pc_data)
{
	if(!sp_f) return(-1);

	/* allocate memory for data. */
	sp_f->pc_data = 		dfilter_copy_data(sp_f->e_ftype,pc_data);
	if (!sp_f->pc_data) 	return(-1);

	return(0);
}


int dfilter_set_type(dfilter_t * sp_f,  efilter_t e)
{
	if(!sp_f) return(-1);
	sp_f->e_ftype = e;
	return(0);
}


int dfilter_add_to_dfilter(dfilter_t * sp_f, dfilter_t * sp_f_next)
{
	dfilter_t * sp_tmp = sp_f;

	while(sp_tmp->next) sp_tmp = sp_tmp->next;
	sp_tmp->next = sp_f_next;

	return(0);
}


int dfilter_add(dfilter_t * sp_f, efilter_t e, char * pc_data)
{
	dfilter_t * sp_new = dfilter_create_with_params(e, pc_data);
	if (!sp_new) return(-1);

	return(dfilter_add_to_dfilter(sp_f, sp_new));
}



int dfilter_free(dfilter_t * sp_f)
{
	dfilter_t * sp_tmp;

	while(sp_f)
	{
		sp_tmp = sp_f->next;
		dfilter_destroy(sp_f);
		sp_f = sp_tmp;
	}

	return(0);
}



/* Test a single entry against chain of filters. */
/* Returns 0 if if it passes test, 1 if it doesn't  */
int dfilter_test_entry(dfilter_t * ps_f, entry_t * ps_entry)
{
	dfilter_t * ps_tmp = ps_f;
	
	while(ps_tmp)
	{
		switch(ps_tmp->e_ftype)
		{
		/* The searsch by file size  */
		case FILTER_SIZE:
			{
				off_t * l_size = (off_t *) ps_tmp->pc_data;
				if (ps_entry->s_st.st_size != *l_size)
					return(-1);
			}
			break;

		/* The searsch by file size  */
		case FILTER_SIZE_LESS:
			{
				off_t * l_size = (off_t *) ps_tmp->pc_data;
				if (*l_size < ps_entry->s_st.st_size)
					return(-1);
			}
			break;

		/* The searsch by file size  */
		case FILTER_SIZE_MORE:
			{
				off_t * l_size = (off_t *) ps_tmp->pc_data;
				if (*l_size > ps_entry->s_st.st_size)
					return(-1);
			}
			break;


		/* Case sensitive comp. by full name */
		case FILTER_FULL_NAME:
			if (strcmp(ps_entry->name, ps_tmp->pc_data))
				return(-1);
			break;


		/* Find substring in the file name. Case sensitive. */
		case FILTER_PART_NAME:
			if (NULL == strstr(ps_entry->name, ps_tmp->pc_data)) return(-1);
			break;

		/* Searches the substring in the file name. Case insensitive. */
		case FILTER_PART_INSENSITIVE_NAME:
			if ( NULL == strcasestr(ps_entry->name, ps_tmp->pc_data) ) 
				return(-1);
			break;

		/* The entry passes if not found substring.  */
		case FILTER_HAS_NOT:
			if ( strstr(ps_entry->name, ps_tmp->pc_data) ) 
				return(-1);
			break;

		/* The entry passes if not found substring. Case insensitive. */
		case FILTER_HAS_NOT_INSENSITIVE:
			if ( strcasestr(ps_entry->name, ps_tmp->pc_data) ) 
				return(-1);
			break;

		/* YYY The search by file type: read file type from the st_mode */
		case FILTER_TYPE:
			{
				etype_t * ps_etype = (etype_t *) ps_tmp->pc_data;

				switch(*ps_etype)
				{
				case FILTER_TYPE_REG:
					if(!S_ISREG(ps_entry->s_st.st_mode)) return(-1);
					break;
				case FILTER_TYPE_SCK: 
					if(!S_ISSOCK(ps_entry->s_st.st_mode)) return(-1);
					break;
				case FILTER_TYPE_LNK:
					if(!S_ISLNK(ps_entry->s_st.st_mode)) return(-1);
					break;
				case FILTER_TYPE_BLK:
					if(!S_ISBLK(ps_entry->s_st.st_mode)) return(-1);
					break;
				case FILTER_TYPE_DIR:
					if(!S_ISDIR(ps_entry->s_st.st_mode)) return(-1);
					break;
				case FILTER_TYPE_CHR:
					if(!S_ISCHR(ps_entry->s_st.st_mode)) return(-1);
					break;
				case FILTER_TYPE_FIF:
					if(!S_ISFIFO(ps_entry->s_st.st_mode)) return(-1);
					break;
				}
			}
			break;

		/* YYY The search by file extention.  */
		case FILTER_EXTENTION:
			{
				char * pc_r_index;
				pc_r_index = rindex(ps_entry->name, '.');
				if (! pc_r_index) return(-1);
				if (strcasecmp(pc_r_index + 1, ps_tmp->pc_data)) return(-1);
			}
			break;

		/* YYY The search by file mode: from st_mode */
		case FILTER_MODE:
			break;

		}

		ps_tmp = ps_tmp->next;
	}

	/* If i am here it means that this entry passed all filters */
	return(0);
}

/* The function of interest. It apply whole the chain of rules to every item in dir_t_in. If an item answers to all rules it copyed to dir_t_out */
/* The function returns 0 on success, < 0 on an error */

int dfilter_by_copy(dir_t * sp_dir_in,  dir_t * sp_dir_out, dfilter_t *sp_f)
{

	int i;

	if (! sp_dir_in || ! sp_dir_out || !sp_f) 
	{
		printf("One of params is wrong\n");
		return(-1);
	}

	if(!sp_dir_out->entry || sp_dir_out->entry_allocated < sp_dir_in->amount)
	{
		dir_t_allocate_entry(sp_dir_out, sp_dir_in->amount);
	}

	sp_dir_out->amount = 0;

	for (i = 0; i < sp_dir_in->amount ; i++)
	{
		if (0 == dfilter_test_entry(sp_f, &sp_dir_in->entry[i]) )
		{
			/* YYY entry copying should be a part of support lib */
			memcpy(&(sp_dir_out->entry[sp_dir_out->amount++]), &(sp_dir_in->entry[i]), sizeof(entry_t));
		}
	}

	return(0);
}



/* Apply filter to dir_t: only passed through the filter entries will be kept */
/* After the operation the dir_t will be resized to hold only kept entryes */
int dfilter_by_shrink(dir_t * sp_dir,  dfilter_t *sp_f)
{

	int i;
	int j = 0;

	if (! sp_dir || !sp_f) 
	{
		printf("One of params is wrong\n");
		return(-1);
	}

	for (i = 0; i < sp_dir->amount ; i++)
	{
		if (0 == dfilter_test_entry(sp_f, &sp_dir->entry[i]) && j < i)
		{
			/* YYY entry copying should be a part of support lib */
			memcpy(&( sp_dir->entry[j++] ), &( sp_dir->entry[i] ), sizeof(entry_t));
		}
	}

	/* Done. If j < i it means that there are obsolete entries. So shrink the array */

	dir_t_shrink(sp_dir, j);
	
	return(0);
}
