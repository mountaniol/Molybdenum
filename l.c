/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include <malloc.h>
#include <strings.h>

#include "l.h"

/* #define E() printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__) */
#define ENTRY() do{}while(0)


que_t * que_create(void)
{
	que_t * q = malloc(sizeof(que_t));
	if(NULL == q) return(NULL);
	bzero(q, sizeof(que_t));
	return(q);
}


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



node_t * 	node_create_data(char * pc_data)
{
	node_t * ps_node;

	ps_node = node_create();
	if (! ps_node) return(NULL);

	ps_node->pc_data = pc_data;
	return(ps_node);
}



node_t * 	node_create(void)
{
	node_t * n = malloc(sizeof(node_t));
	if(NULL == n) return(NULL);
	bzero(n, sizeof(node_t));
	return(n);
}


node_t * 	node_extract(que_t * q)
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


char * 	node_extract_data(que_t * q)
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



node_t * que_add_data_to_tail(que_t *q, char * d)
{
	node_t * ps_node = node_create_data(d);

	if (! ps_node) return(NULL);

	que_add_node_to_tail(q, ps_node);
	return(ps_node);
}


node_t * que_add_data(que_t *q, char * d)
{
	node_t * ps_node = node_create_data(d);

	if (! ps_node) return(NULL);

	que_add_node(q, ps_node);
	return(ps_node);
}


int que_add_node(que_t *q, node_t *n)
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
		return que_add_node(q,n);

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

