
/* Copyright (C) January 2014, Leonid Chernin & Evgeny Yoshpe
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "fsm_example.h"

void FsmTrace(char* buf, int len);
int  ConcreteTmUnsched(void ** timer_handler);
int  ConcreteTmSched(int theTimeout, void *data, void ** timer_handler);
static void concrete_timer_cb(void *data);

void FsmTrace(char* buf, int len)
{
   printf("Lag FSM: %s",buf);
}

int ConcreteTmUnsched(void ** timer_handler)
{
    return 0;
}

int ConcreteTmSched(int timeout, void *data, void ** timer_handler)
{
	return 0;
}

static void
concrete_timer_cb(void *data)
{
    int err = 0;
    struct timer_params *timer_data = (struct timer_params *)data;
    UNUSED_PARAM(data);

    printf("concrete_timer_cb called for timer id = %d\n", timer_data->fsm_timer_id);

    /* Change timer manager context to main thread context to handle timer event correctly */

    err = fsm_timer_trigger_operation(timer_data);
    if (err != 0) {
        printf("Failed in fsm_timer_trigger_operation, err = %u\n", err);
    }
}

int main(void)
{
	int err = 0;
	lag_fsm *pFsm = (lag_fsm *)malloc(sizeof(lag_fsm));

	printf("********** FSM test start ************\n");

	if (!pFsm) {
		return err;
	}
#ifndef WIN32
//	complib_init();
#endif

    err = lag_fsm_init(pFsm, FsmTrace, ConcreteTmSched, ConcreteTmUnsched);

    /* init private attribute of FSM */
    pFsm->lag = 1;

    lag_fsm_start_ev     (pFsm);
	lag_fsm_start_ev     (pFsm);
	err = lag_fsm_admin_on_ev (pFsm);
	if (err != 0) {
		printf("\nlag_fsm_admin_on_ev returned error %d\n", err);
	}
	lag_fsm_admin_off_ev (pFsm);
	lag_fsm_admin_on_ev  (pFsm);
	lag_fsm_lacp_up_ev   (pFsm,0);
	lag_fsm_port_down_ev (pFsm);
	lag_fsm_port_up_ev   (pFsm,1,0);
	lag_fsm_lacp_up_ev   (pFsm,0);
	lag_fsm_mismatch_ev  (pFsm,0);
	lag_fsm_lacp_down_ev (pFsm,0);
	lag_fsm_port_up_ev   (pFsm,1,0);
	
	/* Temporary code because timer manager cb calls timer ev triggered operation in own context */
	fsm_timer_trigger_operation(&(pFsm->tm.tmr_mngr.client_data[SIMPLE_STATE_TMR]));
	fsm_timer_trigger_operation(&(pFsm->tm.tmr_mngr.client_data[COMPOSED_STATE_TMR]));
	
	fsm_print(&pFsm->base);
	printf("\n");
	printf("********** FSM test finish ************\n");

	return err;
}
