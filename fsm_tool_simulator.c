#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#ifndef WIN32
#include "ttypes.h"
#endif
#include "fsm_example.h"

void FsmTrace(char* buf, int len);
int  MlnxTmUnsched(void ** timer_handler);
int  MLnxTmSched(int theTimeout, void *data, void ** timer_handler);

void FsmTrace(char* buf, int len)
{
   printf("%s",buf);
}

int MlnxTmUnsched(void ** timer_handler)
{
	return 1;
}

int MLnxTmSched(int theTimeout, void *data, void ** timer_handler)
{
	return 1;
}

int main(void)
{
	int err = 0;
	lag_fsm *pFsm = (lag_fsm *)malloc(sizeof(lag_fsm));

	printf("********** Start FSM Simulator ************\n");

	if (!pFsm) {
		return err;
	}

	pFsm->base.fsm_type = 2;
    err = lag_fsm_init (pFsm, FsmTrace, MLnxTmSched, MlnxTmUnsched);

    lag_fsm_start_ev    (pFsm);
	lag_fsm_start_ev    (pFsm);
	lag_fsm_admin_on_ev  (pFsm);
	lag_fsm_admin_off_ev (pFsm);
	lag_fsm_admin_on_ev  (pFsm);
	lag_fsm_lacp_up_ev   (pFsm,0);
	
	lag_fsm_port_down_ev (pFsm);
	lag_fsm_port_up_ev   (pFsm,1,0);
	lag_fsm_lacp_up_ev   (pFsm,0);
	lag_fsm_mismatch_ev (pFsm,0);

	lag_fsm_lacp_down_ev (pFsm,0);
	lag_fsm_port_up_ev   (pFsm,1,0);
	fsm_timer_trigger_operation(&pFsm->base, 4);
	getchar();
	return err;
}
