
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

/* *************************************************************** */
/* File: LagFsm.h */
/* Class/Module: LagMngr */
/*  */
/* Purpose: */
/* Description:  Lag  state machine implementation */
/*  */
/* Modification history: */
/*  */
/* *************************************************************** */
#ifndef _LAG_FSM_H
#define _LAG_FSM_H

#include "fsm_tool_framework.h"

/* only example - ConcreteTimer  is real object that aggregates TimerMngr . InitFunc  fills pointers to functions by values of real functions . */
/* Framework will work with these functions */
/* #define ConcreteTimer TimerMngr */

/*#$*//*----------------------------------------------------------------
            Generated enumerator for  States of FSM
   ---------------------------------------------------------------*/
enum lag_fsm_states_t {
    lag_fsm_idle = 0,
    lag_fsm_admin_down = 1,
    lag_fsm_admin_up = 2,
    lag_fsm_lag_down = 3,
    lag_fsm_wait_lag_up = 4,
    lag_fsm_lag_up = 5,
    lag_fsm_condit1 = 6,
    lag_fsm_mismatch = 7,
};
struct lag_fsm;
/*----------------------------------------------------------------
            Events of FSM
   ---------------------------------------------------------------*/
fsm_consume_results lag_fsm_start_ev(struct lag_fsm  * fsm);
fsm_consume_results lag_fsm_port_up_ev(struct lag_fsm  * fsm,  int lacp_enable,
                                       int*  port_id);
fsm_consume_results lag_fsm_port_down_ev(struct lag_fsm  * fsm);
fsm_consume_results lag_fsm_lacp_up_ev(struct lag_fsm  * fsm,  int*  port_id);
fsm_consume_results lag_fsm_lacp_down_ev(struct lag_fsm  * fsm, int*  port_id);
fsm_consume_results lag_fsm_mismatch_ev(struct lag_fsm  * fsm,  int*  port_id);
fsm_consume_results lag_fsm_admin_on_ev(struct lag_fsm  * fsm);
fsm_consume_results lag_fsm_admin_off_ev(struct lag_fsm  * fsm);
/*----------------------------------------------------------------
              Getters for each State
   ---------------------------------------------------------------*/
tbool lag_fsm_idle_in(struct lag_fsm *fsm);
tbool lag_fsm_admin_down_in(struct lag_fsm *fsm);
tbool lag_fsm_admin_up_in(struct lag_fsm *fsm);
tbool lag_fsm_lag_down_in(struct lag_fsm *fsm);
tbool lag_fsm_wait_lag_up_in(struct lag_fsm *fsm);
tbool lag_fsm_lag_up_in(struct lag_fsm *fsm);
tbool lag_fsm_condit1_in(struct lag_fsm *fsm);
tbool lag_fsm_mismatch_in(struct lag_fsm *fsm);
/*----------------------------------------------------------------
              Constructor of FSM
   ---------------------------------------------------------------*/
int lag_fsm_init(struct lag_fsm *fsm, fsm_user_trace user_trace,
                 void * sched_func, void * unsched_func);
/*###############################################################
 ###### declaration of FSM  #########
   //################################################################*/
typedef struct lag_fsm {
    fsm_base base;
    int state_timer[8];
/*----------------------------------------------------------------
           Private attributes :
   ---------------------------------------------------------------*/
/*#$*/
    void * lag;
}lag_fsm;

void UserTrace( void* theBuf, int theLen );

#endif /* _LAG_FSM_H */
