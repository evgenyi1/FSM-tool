
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

/****************************************************************
   File: FsmFramework.c
   Class/Module: Fsm C Framework

   Purpose:
   Description:

   Modification history:
           6/2/2010   [LEONIDC]  Created

 ***************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#ifndef WIN32
#include <netinet/in.h>
#include <unistd.h>
#include <termios.h>
#include "logging.h"
#endif
#include "fsm_tool_framework.h"

void fsm_state_base_entry(fsm_base *fsm, fsm_state_base *target_state);
void fsm_state_base_exit(fsm_base *fsm, fsm_state_base *state);
void fsm_state_simple_entry(fsm_base *fsm, fsm_state_base *target_state);
void fsm_state_simple_exit(fsm_base *fsm, fsm_state_base *target_state);
void fsm_state_composed_entry(fsm_base *fsm, fsm_state_base *target_state);
void fsm_state_composed_exit(fsm_base *fsm, fsm_state_base *target_state);
int fsm_schedule_timeout(fsm_base *fsm, fsm_timer_index timer_index,
                         int timeout, unsigned short id);
int fsm_unschedule_timeout(fsm_base *fsm, fsm_timer_index timer_index);
void print_fsm(fsm_base *fsm);
int fsm_tm_sched(fsm_base *fsm, int timeout, fsm_timer_index timer_index,
                 unsigned short id);
int fsm_tm_unsched(fsm_base *fsm, unsigned short id);
int fsm_tm_init(timer_mngr *timer_manager);
int fsm_tm_deinit(timer_mngr *timer_manager);

/*------------------------------------------------------------------------
                       fsm_init

   Purpose:  init of fsm_base
           initialized  attributes , current state assigned to Default state of FSM ,
           passed pointer to real Timer Manager class
   Output :
   Input  :
   Precondition:
   Returns:  none
   ------------------------------------------------------------------------*/
int
fsm_init(fsm_base *fsm, fsm_state_base *curr_state_entry, uint8 num_states,
         int *state_timer_values, fsm_user_trace user_trace,
         void *timer_sched, void *timer_unsched)
{
    int rc = 1;

    /* itsTraceMode   = FALSE; */
    fsm->busy_flag = 0;
    fsm->current_event = NULL;
    fsm->curr_state = curr_state_entry;
    fsm->prev_state = fsm->last_simple_state = fsm->curr_state;
    fsm->state_timers = state_timer_values;
    fsm->num_states = num_states;
    fsm->user_trace = user_trace;
    fsm->tmr_mngr.sched_func = timer_sched;
    fsm->tmr_mngr.unsched_func = timer_unsched;
    fsm_tm_init(&fsm->tmr_mngr);
    fsm_trace(fsm, "\nFsm Framework called %s \n", __FUNCTION__);
    fsm_trace(fsm, "FSM passed to %s state\n\n", fsm->curr_state->name);
    return rc;
}

int
fsm_deinit(fsm_base *fsm)
{
    fsm->state_timers = NULL;
    fsm->num_states = 0;
    fsm->user_trace = NULL;
    fsm_tm_deinit(&fsm->tmr_mngr);
    return 0;
}

void
fsm_set_timer_value(fsm_base *fsm, int state, int value)
{
    /* to add validation that condition state cannot have timeouts !!  and state without timers transition also cannot accept timer value !=0 TODO (generate bit additional in state) */
    if (state < fsm->num_states) {
        fsm->state_timers[state] = value;
    }
}

int
fsm_get_timer_value(fsm_base *fsm, int state)
{
    if (state < fsm->num_states) {
        return fsm->state_timers[state];
    }
    return 0;
}

/*----------------------------------------------------------------------
                  timer_trigger_operation
   Purpose:     called when timer message arrived to FSM
   Input  :     <id>  Id of the timer
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
void
fsm_timer_trigger_operation(fsm_base *fsm, unsigned short id)
{
    fsm_timer_event ev;

    if (id == AUX_TIMEOUT_ID) {
        ev.opcode = AUX_TIMER_EVENT;
        ev.name = "AUX_TIMER_EVENT";
    }else {
        ev.opcode = TIMER_EVENT;
        ev.name = "TIMER_EVENT";
    }
    ev.id = id; /* id of timer = the name of the state  where timer  was scheduled */
    fsm_handle_event(fsm, (fsm_event_base*) &ev);
}

/*----------------------------------------------------------------------
                       print_fsm
   Purpose: debug print of FSM
          prints current event,current state of FSM, and previous event and State
          called user_trace function that specifies generic trace with other FSM attributes
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
void
print_fsm(fsm_base* fsm)
{
    int len = 0;
    char tracer_buffer[300];

    len += sprintf(tracer_buffer,
                   "Curr. state = %s, Prev. state = %s",
                   fsm->curr_state->name, fsm->prev_state->name);
    if (fsm->user_trace) {
        fsm->user_trace(tracer_buffer, len);
    }
}

/*----------------------------------------------------------------------
                       fsm_trace
   Purpose: if defined  trace mode for FSm called
          UserTrace function redefined in  FSM

   Input  : formatted string
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
void
fsm_trace(fsm_base *fsm, const char *fmt, ...)
{
    int len;
    va_list ap;
    char tracer_buffer[300];

    va_start(ap, fmt);
    len = vsprintf(tracer_buffer, fmt, ap);
    if (fsm->user_trace) {
        fsm->user_trace(tracer_buffer, len);
    }
    va_end(ap);
}

/*----------------------------------------------------------------------
                       fsm_handle_event

   Purpose: Handling of event by FSM:

   - call HandleEvent method of the Current State. Find target state
   while target state = ConditionState continue to call fsm_handle_event of target state
   - process AUX timer event
   - Trace FSM print events (timer or normal), current state, target state

   Input  : ev - pointer to  incoming event
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
fsm_consume_results
fsm_handle_event(fsm_base *fsm, fsm_event_base *ev)
{
	fsm_state_base      *target_state = NULL;
	fsm_consume_results  consume_event_result;
    if (!ev) {
        return FSM_CONSUME_ERROR;
    }
    fsm_trace(fsm, "Event %s arrived to state %s (%d)\n", ev->name,
              fsm->curr_state->name, fsm->curr_state->id);

    if (ev->opcode == TIMER_EVENT) {
        fsm_trace(fsm, "  timer_id = %d ", (( fsm_timer_event*)ev)->id);
    }
    if (fsm->busy_flag) {
        fsm_trace(fsm,
                  "FSM Framework Error: try to recursive entry in \n");
        return FSM_CONSUME_ERROR;
    }

    fsm->current_event = ev;
    fsm->busy_flag = 1;
    consume_event_result = FSM_CONSUMED;

    if (ev->opcode == AUX_TIMER_EVENT) {
        fsm_unschedule_aux_timer(fsm);
    }

    do {
        fsm->reaction_in_state = 0;
        if ((target_state =
                 fsm_state_handle_event(fsm, fsm->curr_state, ev)) != NULL) {
            if (consume_event_result == FSM_CONSUME_ERROR) {
                break; /* returns with consume error */
            }

            if (target_state->type == SIMPLE) {
                fsm->last_simple_state = target_state;
            }
            fsm->prev_state = fsm->curr_state;
            fsm->curr_state = target_state;
        }else {
            if (fsm->curr_state->type == CONDITION) {
                fsm_trace(fsm,
                          "FSM_framework: Error :event %s is not consumed in condition state %s\n",
                          fsm->current_event->name, fsm->curr_state->name);
                consume_event_result = FSM_CONSUME_ERROR;
            }else {
                consume_event_result = FSM_NOT_CONSUMED;
            }
            fsm_trace(fsm, "Event %s is not consumed in state %s (%d)\n\n",
                      ev->name, fsm->curr_state->name, fsm->curr_state->id);
            break;
        }
        if (target_state->type == CONDITION) { /* m_pCurrentEvent = &Null_ev ; // need to preserve this event for pass its parameters to reactions */
            fsm_trace(fsm, "Check Condition %s state\n",
                      fsm->curr_state->name);
        }
    } while ((target_state->type == CONDITION) &&
             (consume_event_result == FSM_CONSUMED));

    if (target_state) {
        fsm_trace(fsm, "FSM passed to %s state\n\n", fsm->curr_state->name);
    }
    fsm->busy_flag = 0;
    fsm->current_event = NULL;

    return consume_event_result;
}

/*----------------------------------------------------------------------
                       fsm_schedule_aux_timer
   Purpose: Scheduler AUX Timeout of FSM - called by user
   Input  : timeout - timeout in ms
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
int
fsm_schedule_aux_timer(fsm_base *fsm, int timeout)
{
    int rc = 1;

    rc = fsm_tm_sched(fsm, timeout, AUX_TIMEOUT_ID, 0);
    if (rc == 0) {
        fsm_trace(fsm, "Auxiliary timer %d was not scheduled in state %s ",
                  timeout, fsm->curr_state->name);
    }
    return rc;
}
/*----------------------------------------------------------------------
                       fsm_unschedule_aux_timer
   Purpose: Scheduler AUX Timeout of FSM - called by user
   Input  :
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
int
fsm_unschedule_aux_timer(fsm_base* fsm)
{
    int rc = 1;

    rc = fsm_tm_unsched(fsm, AUX_TIMEOUT_ID);
    if (rc == 0) {
        fsm_trace(fsm, "Auxiliary timer was not unscheduled in state %s ",
                  fsm->curr_state->name);
    }
    return rc;
}

/****************************************************************************
        fsm_state_base functions
****************************************************************************/

/*----------------------------------------------------------------------
                       fsm_state_handle_event
   Called for Current state of the FSM to check whether Event consumed, perform State reactions
   call Exit  actions  of the Current state (if need )
   define Target state , call Entry actions of Target State(if need)
   Precondition:

   Returns:  target state
   ----------------------------------------------------------------------*/
fsm_state_base *
fsm_state_handle_event(fsm_base *fsm, fsm_state_base *state,
                       fsm_event_base *ev)
{
    fsm_state_base *target_state = NULL;

    target_state =
        ((fsm_state_dispatch)(state->state_dispatcher))(fsm, state->id);
    if (target_state == NULL) {
        if (state->composed) {
            target_state =
                ((fsm_state_dispatch)(state->composed->state_dispatcher))(fsm,
                                                                          state
                                                                          ->
                                                                          composed
            ->id);
        }
    }
    if (target_state && (target_state->type != CONDITION)) {
        //fsm->consumed_in_composed = (target_state->default_substate) ? 1 : 0;

        /* if arc on composed state(CurrentState == targetState ) and also "reaction in state" - nothing to do .
         * By this we will support arcs on composed states */
        if ((fsm->curr_state) && (state->composed) &&
            (state->composed == target_state) && (fsm->reaction_in_state)) {
            target_state = fsm->curr_state;
        }else {
            if (target_state->default_substate) {
                target_state = target_state->default_substate;
            }
            fsm_state_simple_exit(fsm, target_state);
            fsm_state_simple_entry(fsm, target_state);
        }
    }
    return target_state;
}

/*----------------------------------------------------------------------
                       fsm_schedule_timeout
   Purpose:  Scheduler of Timeout Automatically called by framework
   Input  :  fsm -  pointer to user's FSM
   Output :
   Precondition:
   Returns:  status
   ----------------------------------------------------------------------*/
int
fsm_schedule_timeout(fsm_base *fsm, fsm_timer_index timer_index, int timeout,
                     unsigned short id)
{
    int rc = 1;

    if (fsm->reaction_in_state || (timeout == 0)) {
        return 0;
    }
    fsm_trace(fsm, "fsm_tm_sched in state for %d msec\n", timeout);

    rc = fsm_tm_sched(fsm, timeout, timer_index, id);
    if (rc == 0) {
        fsm_trace(fsm, " Timer %d was not scheduled \n", timer_index);
    }
    return rc;
}

/*--------------------------------------------------------------
                       fsm_unschedule_timeout
   Purpose: Unscheduler Timeout . automatic framework call
   Input  :  fsm -  pointer to user's FSM
   Output :
   Precondition:

   Returns:  status
   ----------------------------------------------------------------------*/

int
fsm_unschedule_timeout(fsm_base * fsm, fsm_timer_index timer_index)
{
    int rc = 1;
    unsigned short id;
    const char *name;

    id =
        (timer_index ==
         SIMPLE_STATE_TMR) ? (fsm->curr_state->id) : fsm->curr_state->composed
        ->id;
    name =
        (timer_index ==
         SIMPLE_STATE_TMR) ? (fsm->curr_state->name) : fsm->curr_state->
        composed->
        name;

    if (fsm->reaction_in_state || (fsm->state_timers[id] == 0)) {
        return 0;
    }
    fsm_trace(fsm, "UnsnchedlTm in state %s for %d msec \n", name,
              fsm->state_timers[id]);

    rc = fsm_tm_unsched(fsm, id);
    if (rc == 0) {
        fsm_trace(fsm, "Timer was not unscheduled in state %s \n", name);
    }
    return rc;
}

/*--------------------------------------------------------------
                       fsm_state_base_entry
   Purpose:
         Entry function called upon entry each  state
         possible actions - scheduler timer , call entryState function
   Input  :  fsm -  pointer to user's FSM
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
void
fsm_state_base_entry(fsm_base *fsm, fsm_state_base *state)
{
    if (state->type == COMPOSED) {
        fsm_schedule_timeout(fsm, COMPOSED_STATE_TMR,
                             fsm->state_timers[state->id], state->id);
    }else {
        fsm_schedule_timeout(fsm, SIMPLE_STATE_TMR,
                             fsm->state_timers[state->id], state->id);
    }
    if (state->entry_func) {
        ((fsm_state_entry)(state->entry_func))(fsm);
    }
}

/*--------------------------------------------------------------
                       fsm_state_base_exit

   Purpose: Exit function called upon exit from each  state
          possible actions - unscheduler timer , call exitState function

   Input  : fsm -  pointer to user's FSM
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
void
fsm_state_base_exit(fsm_base *fsm, fsm_state_base *state)
{
    if (state->type == COMPOSED) {
        fsm_unschedule_timeout(fsm, COMPOSED_STATE_TMR);
    }else {
        fsm_unschedule_timeout(fsm, SIMPLE_STATE_TMR);
    }
    if (state->exit_func) {
        ((fsm_state_exit)(state->exit_func))(fsm);
    }
}

/*---------------------------------------------------
   State Simple functions
   --------------------------------------------------*/

/*--------------------------------------------------------------------
           fsm_state_simple_entry
   redefinition of base behavior: first call Base Entry function  and
   then - call Entry function of Parent Composed state(if parent defined for this state)
   --------------------------------------------------------------------*/
void
fsm_state_simple_entry(fsm_base *fsm, fsm_state_base *target_state)
{
    fsm_state_base_entry(fsm, target_state);
    if (target_state->composed) {
        fsm_state_composed_entry(fsm, target_state->composed);
    }
}

/*--------------------------------------------------------------------
           fsm_state_simple_exit
   redefinition of base behavior: first call Base Exit function  and
   then - call Exit function of Parent Composed state(if parent defined for this state)itsReactionInState
   --------------------------------------------------------------------*/
void
fsm_state_simple_exit(fsm_base *fsm, fsm_state_base *target_state)
{
    fsm_state_base_exit(fsm, fsm->curr_state);
    if (fsm->curr_state->composed) {
        fsm_state_composed_exit(fsm, target_state);
    }
}

/*--------------------------------------------------
   State Composed functions
   ----------------------------------------------------*/

/*--------------------------------------------------------------------
           fsm_state_composed_entry
   if Composed state was changed as a result of event consumption
   then call Entry from the Base class
   --------------------------------------------------------------------*/
void
fsm_state_composed_entry(fsm_base *fsm, fsm_state_base *target_state)
{
    if (!(fsm->last_simple_state->composed) ||
        (fsm->last_simple_state->composed != target_state)) {
        fsm_state_base_entry(fsm, target_state);
    }
}

/*--------------------------------------------------------------------
           fsm_state_composed_exit
   if Composed state was changed as a result of event consumption then called Exit method
   from the Base class

   --------------------------------------------------------------------*/
void
fsm_state_composed_exit(fsm_base *fsm, fsm_state_base *target_state)
{
    if (target_state->type != CONDITION) {
        if (!(target_state->composed) ||
            (target_state->composed != fsm->curr_state->composed) /* use LastSimpleState */
            /* ((target_state->itsComposed == fsm->itsCurrStateP)&&(fsm->itsLastConsumed == fsm->itsCurrStateP->itsComposed )) New condition ! to validate arc on composed */
            ) {
            fsm_state_base_exit(fsm, fsm->curr_state->composed);
        }
    }
}

/******************************************Timer functions**************************************************************/

int
fsm_tm_sched(fsm_base *fsm, int timeout, fsm_timer_index timer_index,
             unsigned short id)
{
    timer_mngr *tmr = (timer_mngr*) &(fsm->tmr_mngr);
    int rc = 0;

    if (timer_index < LAST_INDEX_TMR) {
        tmr->client_data[timer_index].fsm_timer_id = id;
        tmr->client_data[timer_index].scheduled_status = 1;
        tmr->client_data[timer_index].fsm = fsm;

        /*return lew_event_reg_timer(tmr->tmr_context, &tmr->itsClientData[theTimerIndex].itsTimerRef, tmr->tmrMngr.itsCallback ,  (void *)&tmr->itsClientData[theTimerIndex], theTimeout); */
        if (tmr->sched_func) {
            rc =
                ((real_tm_sched)tmr->sched_func)(timeout,
                                                 (void *)&tmr->client_data[
                                                     timer_index],
                                                 &tmr->client_data[timer_index]
                                                 .timer_ref);
        }
    }
    return rc;
}
/* ----------------------------------------------------------------------------------- */
int
fsm_tm_unsched(fsm_base *fsm, unsigned short id)
{
    int i;
    int rc = 0;
    timer_mngr *tmr = (timer_mngr*) &(fsm->tmr_mngr);

    for (i = SIMPLE_STATE_TMR; i < LAST_INDEX_TMR; i++) {
        if (tmr->client_data[i].fsm_timer_id == id) {
            if (tmr->unsched_func) {
                rc =
                    ((real_tm_unsched)tmr->unsched_func)(&tmr->client_data[i].
                                                         timer_ref);
            }
            tmr->client_data[i].scheduled_status = 0;
            break;
        }
    }
    if (i == LAST_INDEX_TMR) {
        /* error! */
    }
    return rc;
}

/* ----------------------------------------------------------------------------------- */
int
fsm_tm_init(timer_mngr *timer_manager)
{
    int i, rc = 0;

    for (i = 0; i < LAST_INDEX_TMR; i++) {
        timer_manager->client_data[i].fsm = NULL;
        timer_manager->client_data[i].scheduled_status = 0;
        timer_manager->client_data[i].timer_ref = 0;
    }
    return rc;
}

/* ----------------------------------------------------------------------------------- */
int
fsm_tm_deinit(timer_mngr *timer_manager)
{
    int i;

    for (i = SIMPLE_STATE_TMR; i < LAST_INDEX_TMR; i++) {
        if (timer_manager->client_data[i].scheduled_status) {
            /*rc = IPC_removeTimerByRef(itsClientData[i].itsTimerRef);//lew_event_cancel(tmr->tmr_context ,&tmr->itsClientData[theTimerIndex].itsTimerRef);*/
            if (timer_manager->unsched_func) {
                ((real_tm_unsched)timer_manager->unsched_func)(&timer_manager->
                                                               client_data[i].
                                                               timer_ref);
            }
            timer_manager->client_data[i].scheduled_status = 0;
            timer_manager->client_data[i].timer_ref = 0;
            break;
        }
    }
    timer_manager->unsched_func = NULL;
    timer_manager->sched_func = NULL;
    return 0;
}
