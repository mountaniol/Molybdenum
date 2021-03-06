#ifndef __KJHjhdkjhLKJHKJHkjhlk
#define __KJHjhdkjhLKJHKJHkjhlk

/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include <stdio.h>
#include <pthread.h>

#include "obj.h"

struct node_struct
{
	obj_t t;
	struct  node_struct * next;
	int i_data;
	char * pc_data;
};

typedef struct node_struct node_t;

struct que_struct 
{
	obj_t t;
	int         amount;
	node_t *    list;
	node_t *    tail;
	pthread_mutex_t lock;
};

typedef struct que_struct que_t;
typedef int (*compare_t)(char *, char *);

que_t * 	que_create(void);
int 		que_destroy(que_t *);
int 		que_destroy_data(que_t *q);
int 		que_delete_nodes_and_data(que_t *q);
char * 		que_remove_node_by_data(que_t * q,  char * pc_data);
node_t * 	que_node_create(void);
node_t * 	que_create_node_with_data(char * pc_data);
node_t * 	que_extract(que_t *);
char * 		que_extract_data(que_t * q);
char * 		que_extract_data_r(que_t * q); /* thread safe version */
int 		que_push(que_t *, node_t *);
int 		que_add_node_to_tail(que_t *q, node_t *n);

node_t * 	que_push_data(que_t *, char *);
node_t * 	que_add_data_to_tail(que_t *, char * );
node_t * 	que_add_data_to_tail_r(que_t *q, char * d); /* thread safe version */
int 		que_amount(que_t *);
node_t * 	que_find_node(que_t * q,  int i_data, compare_t);

#endif
