/* Vsevolod Mountaniol, 2011, GPL */
/* This file is a part of Molybdenum project */

#include "cbs.h"
#include "d.h"
#include "dw.h"
#include "lock.h"


void sig_hand_1(osig_t * ps_s)
{
	char * message = (char *) ps_s->data;

	printf("I got message %s from %i\n", ps_s->data, ps_s->src);
	cbs_signal_reply(ps_s, ps_s->signum, "Thank you, bobo!");
}

void sig_hand_2(osig_t * ps_s)
{
	char * message = (char *) ps_s->data;

	printf("I got reply %s from %i\n", ps_s->data, ps_s->src);
	cbs_signal_free(ps_s);
}


int main()
{
	obj_t o;
	int i;

	char error[1024];

	obj_e e;
	obj_t * m;
	obj_t * oo[2];
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

	for (i = 0 ; i < 2 ; i++)
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

	cbs_send_sig_id(m->id, oo[0]->id, OBJ_S_USER1, "Test message");

	printf("Object %i signal que amount is: %d\n", oo[0]->id, oo[0]->q_sig->amount);

	sleep(2);

	for (i = 0 ; i < 2 ; i++)
		obj_free(oo[i]);

	obj_free(m);

	printf("Hash amount: %ld\n", cbs_hash_amount());

	cbs_destroy();

	return(0);
}
