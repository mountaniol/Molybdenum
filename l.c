/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include <malloc.h>
#include <strings.h>


#include "l.h"

/* #define E() printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__) */
#define ENTRY() do{}while(0)


/* New que */
que_t * que_create(void)
{
	que_t * q = malloc(sizeof(que_t));
	if(NULL == q) return(NULL);
	bzero(q, sizeof(que_t));
	olock_init(&q->lock);
	return(q);
}


/* Destroys the que and the nodes; Doesn't free data */
int que_destroy(que_t *q)
{
	node_t * n;
	if(NULL == q) return(-1);

	

	while (q->list)
	{
		n = q->list;
		q->list = q->list->next;
		free(n);		
	}
	
	free(q);
	return(0);
}

int que_destroy_r(que_t *q)
{
	obj_lock( (obj_t *) q);
	return que_destroy(q);
}




/* Run on the que and free data of every node; Doesn't remove the nodes and que */
int que_destroy_data(que_t *q)
{
	node_t * n;
	if(NULL == q) return(-1);

	while (q->list)
	{
		n = q->list;
		q->list = q->list->next;
		if(n->pc_data)free(n->pc_data);
		free(n);		
	}

	free(q);
	return(0);
}


/* Delete all nodes and free data of every node; But doesn't delete que itself  */
int que_delete_nodes_and_data(que_t *q)
{
	node_t * n;
	if(NULL == q) return(-1);

	while (q->list)
	{
		n = q->list;
		q->list = q->list->next;
		if(n->pc_data)free(n->pc_data);
		free(n);		
	}
	q->amount = 0;

	return(0);
}



node_t * 	que_create_node_with_data(char * pc_data)
{
	node_t * ps_node;
	ps_node = que_node_create();
	if (! ps_node) return(NULL);
	ps_node->pc_data = pc_data;
	return(ps_node);
}



node_t * 	que_node_create(void)
{
	node_t * n = malloc(sizeof(node_t));
	if(NULL == n) return(NULL);
	bzero(n, sizeof(node_t));
	return(n);
}


node_t * 	que_extract(que_t * q)
{
	node_t * n;

	if(NULL == q || NULL == q->list) return(NULL);
	ENTRY(); 
	n = q->list;
	q->list = n->next;
	q->amount--;
	n->next = NULL;

	if (NULL == q->list)
	{
		q->tail = NULL;
	}

	ENTRY();
	/*	LOG("Extracted from que: %s\n", n->filename ); */

	/* e = ((struct event_node *) n->data)->filename; */
	
	return n;
}


char * 	que_extract_data(que_t * q)
{
	node_t * n;
	char * d;

	if(NULL == q || NULL == q->list) return(NULL);
	ENTRY(); 
	n = q->list;
	q->list = n->next;
	q->amount--;
	n->next = NULL;

	if (NULL == q->list)
	{
		q->tail = NULL;
	}

	/*	LOG("Extracted from que: %s\n", n->filename ); */

	/* e = ((struct event_node *) n->data)->filename; */
	d = n->pc_data;
	free(n);	
	return d;
}


char * 	que_extract_data_r(que_t * q)
{
	char * pc_data;
	obj_lock(q);
	//olock_lock(&q->lock);
	pc_data = que_extract_data(q);
	//olock_unlock(&q->lock);
	obj_unlock(q);
	return(pc_data);
}

node_t * que_add_data_to_tail(que_t *q, char * d)
{
	node_t * ps_node = que_create_node_with_data(d);

	if (! ps_node) return(NULL);

	que_add_node_to_tail(q, ps_node);
	return(ps_node);
}


node_t * que_add_data_to_tail_r(que_t *q, char * d)
{
	node_t * ps_node;

	obj_lock( (obj_t *) q);
	ps_node = que_add_data_to_tail(q, d);
	obj_unlock( (obj_t *) q);
	return(ps_node);
}


node_t * que_push_data(que_t *q, char * d)
{
	node_t * ps_node = que_create_node_with_data(d);

	if (! ps_node) return(NULL);

	que_push(q, ps_node);
	return(ps_node);
}


int que_push(que_t *q, node_t *n)
{
	if (NULL == q || n == NULL) 
	{
		/* printf("Error! q = %p, node = %p\n", q, n); */
		return(-1);
	}


	n->next = q->list;
	q->list = n;
	q->amount++;

	if (NULL == q->tail)
	{
		q->tail = q->list;
	}

	/*	LOG("Added to que: %s\n", q->list->filename ); */

	return(0);	
}



int que_add_node_to_tail(que_t *q, node_t *n)
{
	if (NULL == q || n == NULL) 
	{
		/* printf("Error! q = %p, node = %p\n", q, n); */
		return(-1);
	}


	if (NULL == q->tail) 
		return que_push(q,n);

	q->tail->next = n;
	n->next = NULL;
	q->tail = n;
	q->amount++;

	/*	LOG("Added to que: %s\n", q->list->filename ); */

	return(0);	
}



int que_amount(que_t *q)
{
	if (NULL == q) return(-1);
	return(q->amount);
}


node_t * que_find_node(que_t * q,  int i_data, compare_t compare)
{

	node_t * node = q->list;
	if (0 == q->amount)
	{
		/*		LOG("empty que or no data : %p %d \n", data, q->amount); */
		return NULL;
	}

	while (node)
	{
		if (node->i_data == i_data )
		{
			return node;
		}

		node = node->next;
	}

    return(NULL);
}


char * que_remove_node_by_data(que_t * q,  char * pc_data)
{

	node_t * prev;
	node_t * node;
	char * pc_ret_data;

	node = q->list;
	prev = q->list;

	if (0 == q->amount)
	{
		/*		LOG("empty que or no data : %p %d \n", data, q->amount); */
		return NULL;
	}

	while (node)
	{
		if (node->pc_data == pc_data )
		{
			/* Found */
			if (q->list == node) 
			{
				q->list = node->next;
				if (q->tail == node) q->tail = q->list;
			}
			else 
			{
				prev = node->next;
				if (q->tail == node) q->tail = prev;
			}

			pc_ret_data = node->pc_data;

			free(node);
			q->amount--;
			return(pc_ret_data);
		}

		prev = node;
		node = node->next;
	}

    return(NULL);
}



/* It created new que_t and then copy chain of data */
static que_t * que_dup(que_t * ps_src)
{
	que_t  * ps_dst = NULL;
	node_t * ps_node = NULL;

	ps_dst = que_create();
	if(ps_src->amount > 0)
	{
		ps_node = ps_src->list;
		while(ps_node)
		{
			que_push(ps_dst, (node_t *) obj_dup( (obj_t *) ps_node));
			ps_node = ps_node->next;
		}
	}
	return(ps_dst);
}



node_t * node_dup(node_t * ps_node)
{
	return(NULL);
}
