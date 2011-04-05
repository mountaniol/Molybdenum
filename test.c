/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "cbs.h"
#include "d.h"
#include "dw.h"
#include "lock.h"


void sig_hand_1(osig_t * ps_s)
{
	char * message = (char *) ps_s->data;
	obj_e e;

	printf("I got message %s from %i\n", ps_s->data, ps_s->src);
	do
	{
		e = cbs_signal_reply(ps_s, ps_s->signum, "Thank you, bobo!");
	} while (e != OBJ_E_OK);
}

void sig_hand_2(osig_t * ps_s)
{
	char * message = (char *) ps_s->data;

	printf("I got reply %s from %i\n", ps_s->data, ps_s->src);
	cbs_signal_free(ps_s);
}


#define ASIZE 1024

int main()
{
	obj_t o;
	int i;

	char error[1024];

	obj_e e;
	obj_t * m;
	obj_t * oo[ASIZE];
	id_t id_drc;
	id_t id_dst;

	init_objects();
	cbs_init();

	m = obj_new(OBJ_TYPE_DIR, NULL);
	if (!m || obj_err(m))
	{	
		printf("Can't init dholder: m: %p\n", (void *) m);
		return(0);
	}

	m->sighandler = sig_hand_2;

	for (i = 0 ; i < ASIZE ; i++)
	{
		oo[i] = obj_new(OBJ_TYPE_DIR, NULL);
		e = obj_err(oo[i]);
		if (e) 
		{
			obj_err_to_str(oo[i], error, 1024);
			printf("Object creation error : %s\n" ,error);
		}
		cbs_set_employee(m, oo[i]);

		oo[i]->sighandler = sig_hand_1;
	}


	printf("Hash amount: %ld\n", cbs_hash_amount());
	printf("Sending sig from %i to %i\n", m->id, oo[0]->id);

	for (i = 0; i < ASIZE ; i++)
	{
		cbs_send_sig_id(m->id, oo[i]->id, OBJ_S_USER1, "Test message");
	}

	usleep(100000);

	printf("Object %i signal que amount is: %d\n", oo[0]->id, oo[0]->q_sig->amount);

	//sleep(2);

	for (i = 0 ; i < ASIZE ; i++)
		obj_free(oo[i]);

	obj_free(m);

	printf("Hash amount: %ld\n", cbs_hash_amount());

	cbs_destroy();

	return(0);
}
