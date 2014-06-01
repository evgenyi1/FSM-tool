
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
#endif
#include "fsm_tool_framework.h"

int fsm_state_base_entry     (struct fsm_base *fsm,  struct fsm_state_base *target_state, struct fsm_event_base * ev);
int fsm_state_base_exit      (struct fsm_base *fsm,  struct fsm_state_base *state,        struct fsm_event_base * ev);
int fsm_state_simple_entry   (struct fsm_base *fsm, struct fsm_state_base *current_state,  struct fsm_state_base *target_state, struct fsm_event_base * ev);
int fsm_state_simple_exit    (struct fsm_base *fsm, struct fsm_state_base *current_state,  struct fsm_state_base *target_state, struct fsm_event_base * ev);
int fsm_state_composed_entry (struct fsm_base *fsm, struct fsm_state_base *current_state,  struct fsm_state_base *target_state, struct fsm_event_base * ev);
int fsm_state_composed_exit  (struct fsm_base *fsm, struct fsm_state_base *current_state,  struct fsm_state_base *target_state, struct fsm_event_base * ev);

int fsm_schedule_timeout(struct fsm_base *fsm, enum fsm_timer_index timer_index,
                         int timeout, unsigned char id);
int fsm_unschedule_timeout(struct fsm_base *fsm, enum fsm_timer_index timer_index);

int fsm_tm_sched(struct fsm_base *fsm, int timeout, enum fsm_timer_index timer_index,
                 unsigned char id);
int fsm_tm_unsched(struct fsm_base *fsm, unsigned char id);
int fsm_tm_init(struct timer_mngr *timer_manager);
int fsm_tm_deinit(struct timer_mngr *timer_manager, void* unsched);
int fsm_state_handle_event(struct fsm_base *fsm, struct fsm_state_base *state,
                           struct fsm_event_base *ev, struct fsm_state_base** target_state);
struct fsm_state_base * get_state_by_id(struct fsm_base *fsm, unsigned char id);

/*------------------------------------------------------------------------
                       fsm_init

   Purpose:  init of struct fsm_base
           initialized  attributes , current state assigned to Default state of FSM ,
           passed pointer to real Timer Manager class
   Output :
   Input  :
   Precondition:
   Returns:  none
   ------------------------------------------------------------------------*/
int
fsm_init(                                 struct fsm_base *fsm, 
                                          struct fsm_state_base *curr_state_entry, 
                                          uint8 num_states,
                                          int *state_timer_values, fsm_user_trace user_trace,
                                          void *timer_sched, void *timer_unsched, 
                                          struct fsm_static_data * static_data)
{
    int rc = 0;

    fsm->busy_flag = 0;
    fsm->curr_state = curr_state_entry->id;
	fsm->prev_state = fsm->curr_state ;
	
	static_data->num_states = num_states;
	static_data->tm_sched_func = timer_sched;
	static_data->tm_unsched_func = timer_unsched;
	static_data->user_trace = user_trace;
	//static_data->timer_used = (static_data->tm_sched_func !=NULL);
	if(static_data->timer_used) {
		struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
		tm->state_timers = state_timer_values;
        fsm_tm_init(&tm->tmr_mngr);
	}
	fsm->gen_data = static_data;

    fsm_trace(fsm, "Fsm Framework called %s\n", __FUNCTION__);
    fsm_trace(fsm, "FSM passed to %s state\n", curr_state_entry->name);
    return rc;
}

int
fsm_deinit(struct fsm_base *fsm)
{
   
	fsm_trace(fsm, "\nFsm Framework: called %s \n", __FUNCTION__);
	if(fsm->curr_state)
	{
	   fsm->gen_data->num_states = 0;
	   fsm->gen_data->user_trace = NULL;
	   fsm->gen_data->tm_sched_func = NULL;
	   fsm->gen_data->tm_unsched_func = NULL;
	}
	if(fsm->gen_data->timer_used)
	{
		struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
		tm->state_timers = NULL;
		fsm_tm_deinit(&tm->tmr_mngr, fsm->gen_data->tm_unsched_func);
		fsm->gen_data->tm_unsched_func = NULL;
		fsm->gen_data->tm_sched_func = NULL;
	}
    return 0;
}

void
fsm_set_timer_value(struct fsm_base *fsm, int state, int value)
{
    /* to add validation that condition state cannot have timeouts !!  and state without timers transition also cannot accept timer value !=0 TODO (generate bit additional in state) */
	if (state < fsm->gen_data->num_states) {
		struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
        tm->state_timers[state] = value;
    }
}

int
fsm_get_timer_value(struct fsm_base *fsm, int state)
{
    if (state < fsm->gen_data->num_states) {
		struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
         return tm->state_timers[state];
    }
    return 0;
}


tbool
fsm_is_in_state (struct fsm_base *fsm, unsigned short state)
{
  tbool res = 0;
  if(fsm && (state <fsm->gen_data->num_states))
  {
	  if (fsm->gen_data->states[state]->default_substate != NULL){ /* state - composed !*/
		  res = (fsm->gen_data->states[fsm->curr_state]->composed->id == state);
	  }
	  else{
	     res = (state == fsm->curr_state);
	  }
  }
  return res;
}

const char * 
fsm_get_state(struct fsm_base *fsm, unsigned short *state)
{
   if (fsm) {
	   *state = fsm->curr_state;
	   return (fsm->gen_data->states[fsm->curr_state]->name);
   }
   else {
	   return NULL;
   }
}

/*----------------------------------------------------------------------
                  timer_trigger_operation
   Purpose:     called when timer message arrived to FSM
   Input  :     <id>  Id of the timer
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
int
fsm_timer_trigger_operation(struct timer_params *timer_data)
{
    struct fsm_timer_event ev;

    if (timer_data->fsm_timer_id == AUX_TIMEOUT_ID) {
        ev.opcode = AUX_TIMER_EVENT;
        ev.name = "AUX_TIMER_EVENT";
    }else {
        ev.opcode = TIMER_EVENT;
        ev.name = "TIMER_EVENT";
    }
    ev.id = timer_data->fsm_timer_id; /* id of timer = the name of the state  where timer  was scheduled */
    return fsm_handle_event(timer_data->fsm, (struct fsm_event_base*) &ev);
}

/*----------------------------------------------------------------------
                       fsm_print
   Purpose: debug print of FSM
          prints current event,current state of FSM, and previous event and State
          called user_trace function that specifies generic trace with other FSM attributes
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
void
fsm_print(struct fsm_base* fsm)
{
    int len = 0;
    char tracer_buffer[300];

    len += sprintf(tracer_buffer,
                   "Curr. state = %s, Prev. state = %s\n",
				   fsm->gen_data->states[fsm->curr_state]->name,
				   fsm->gen_data->states[fsm->prev_state]->name);
	if (fsm->gen_data->user_trace) {
        fsm->gen_data->user_trace(tracer_buffer, len);
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
fsm_trace(struct fsm_base *fsm, const char *fmt, ...)
{
    int len;
    va_list ap;
    char tracer_buffer[300];

    va_start(ap, fmt);
    len = vsprintf(tracer_buffer, fmt, ap);
    if (fsm->gen_data->user_trace) {
        fsm->gen_data->user_trace(tracer_buffer, len);
    }
    va_end(ap);
}


/*----------------------------------------------------------------------
                       get_state_by_state_id
   Purpose:   get state  structure by state ID 

   Input  : 
   Output :
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
struct fsm_state_base * 
get_state_by_id(struct fsm_base *fsm, unsigned char id)
{
    if(!fsm) {
	    return NULL;
    }
    return fsm->gen_data->states[id];
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
int
fsm_handle_event(struct fsm_base *fsm, struct fsm_event_base *ev)
{
	int err = FSM_CONSUMED;
	struct fsm_state_base *target_state = NULL;
	struct fsm_state_base *curr_state_p = get_state_by_id(fsm, fsm->curr_state);

    if (!ev) {
        return FSM_CONSUME_ERROR;
    }

    fsm_trace(fsm, "Event %s arrived to state %s (%d)\n", ev->name,
              curr_state_p->name, curr_state_p->id);

    if (ev->opcode == TIMER_EVENT) {
        fsm_trace(fsm, "  timer_id = %d\n", (( struct fsm_timer_event*)ev)->id);
    }

    if (fsm->busy_flag) {
        fsm_trace(fsm, "FSM Framework Error: try to recursive entry in \n");
        return FSM_CONSUME_ERROR;
    }

    /* fsm->current_event = ev; */
    fsm->busy_flag = 1;

    if (ev->opcode == AUX_TIMER_EVENT) {
        fsm_unschedule_aux_timer(fsm);
    }

    do {
        fsm->reaction_in_state = 0;

        err = fsm_state_handle_event(fsm, curr_state_p, ev, &target_state);
        if (err > FSM_LAST_FRAMEWORK_ERROR) {
        	// user returned error from fsm reaction
        	goto bail;
        }

        if (target_state != NULL) {
			fsm->prev_state = curr_state_p->id;
			curr_state_p    = target_state;
        }
        else {
            if (curr_state_p->type == CONDITION) {
                fsm_trace(fsm, "FSM Framework Error: event %s is not consumed in condition state %s\n",
                          ev->name, curr_state_p->name);
                err = FSM_CONSUME_ERROR;
            }
            fsm_trace(fsm, "Event %s is not consumed in state %s (%d)\n\n",
                      ev->name, curr_state_p->name, curr_state_p->id);
            err = 0; /* no error on event is not consumed, only log print */
            break;
        }
        if (target_state->type == CONDITION) { /* m_pCurrentEvent = &Null_ev ; need to preserve this event for pass its parameters to reactions */
            fsm_trace(fsm, "Check Condition %s state\n", curr_state_p->name);
			fsm->curr_state = target_state->id;
        }
    } while ((target_state->type == CONDITION) && (err == FSM_CONSUMED));

    if (target_state) {
        fsm_trace(fsm, "FSM passed to %s state\n\n", target_state->name);
		fsm->curr_state = target_state->id;
    }

bail:
    fsm->busy_flag = 0;
    return err;
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
fsm_schedule_aux_timer(struct fsm_base *fsm, int timeout)
{
    int err = 0;

	if (!fsm->gen_data->timer_used) {
        fsm_trace(fsm, "Auxiliary timer was not scheduled in state %d because timer is not used in this FSM",
                  fsm->curr_state);
				err = FSM_TIMER_USING_ERROR;
		 return err;
	 }
    err = fsm_tm_sched(fsm, timeout, AUX_TIMEOUT_ID, 0);
    if (err != 0) {
        fsm_trace(fsm, "Auxiliary timer %d was not scheduled in state %d",
                  timeout, fsm->curr_state);
    }
    return err;
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
fsm_unschedule_aux_timer(struct fsm_base* fsm)
{
    int err = 0;
	
	  if (!fsm->gen_data->timer_used) {
        fsm_trace(fsm, "Auxiliary timer was not unscheduled in state %d because timer is not used in this FSM",
                  fsm->curr_state );
				err = FSM_TIMER_USING_ERROR;
		    return err;
	  }

    err = fsm_tm_unsched(fsm, (unsigned char)AUX_TIMEOUT_ID);
    if (err != 0) {
        fsm_trace(fsm, "Auxiliary timer was not unscheduled in state %d",
                  fsm->curr_state );
    }
    return err;
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
int
fsm_state_handle_event(struct fsm_base *fsm, struct fsm_state_base *state,
                       struct fsm_event_base *ev, struct fsm_state_base **target_state)
{
	int err = 0;
	struct fsm_state_base *tmp_target_state = NULL;

    err = ((fsm_state_dispatch)(state->state_dispatcher))(fsm, state->id, ev, &tmp_target_state);
    if (err > FSM_LAST_FRAMEWORK_ERROR) {
    	// user returned error from fsm reaction
    	goto bail;
    }

    if (tmp_target_state == NULL) {
        if (state->composed) {
        	err = ((fsm_state_dispatch)(state->composed->state_dispatcher))(fsm, state->composed->id, ev, &tmp_target_state);
            if (err > FSM_LAST_FRAMEWORK_ERROR) {
            	// user returned error from fsm reaction
            	goto bail;
            }
        }
    }

    if (tmp_target_state) {

        /* if arc on composed state(CurrentState == targetState ) and also "reaction in state" - nothing to do .
         * By this we will support arcs on composed states */
        if ( (state->composed) &&
             (state->composed == tmp_target_state) && (fsm->reaction_in_state)) {
        	tmp_target_state = fsm->gen_data->states[fsm->curr_state];
        }
		else {
            if (tmp_target_state->default_substate) {
            	tmp_target_state = tmp_target_state->default_substate;
            }
            if (!fsm->reaction_in_state) {
            	err = fsm_state_simple_exit(fsm, state, tmp_target_state, ev);
	            if (err > FSM_LAST_FRAMEWORK_ERROR) {
         		    goto bail;
				}
            	err = fsm_state_simple_entry(fsm, state, tmp_target_state, ev);
 	            if (err > FSM_LAST_FRAMEWORK_ERROR) {
         		    goto bail;
				}
           }
        }
    }

bail:
    *target_state = tmp_target_state;
    return err;
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
fsm_schedule_timeout(struct fsm_base *fsm, enum fsm_timer_index timer_index,
		                 int timeout, unsigned char id)
{
    int err = 0;

		if (fsm->reaction_in_state ||
    	  (timeout == 0)) {
        return err;
    }

		if(!fsm->gen_data->timer_used) {
	      fsm_trace(fsm, "fsm_tm_sched: timer not used\n");
				err = FSM_TIMER_USING_ERROR;
		    return err;
	  }

    fsm_trace(fsm, "fsm_schedule_timeout in state for %d msec\n", timeout);

    err = fsm_tm_sched(fsm, timeout, timer_index, id);
    if (err != 0) {
        fsm_trace(fsm, " Timer %d was not scheduled\n", timer_index);
    }
    return err;
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
fsm_unschedule_timeout(struct fsm_base *fsm, enum fsm_timer_index timer_index)
{
    int err = 0;
    unsigned char id;
    const char *name;
	struct fsm_state_base *curr_state_p = get_state_by_id(fsm, fsm->curr_state);
    struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
		
	  if (!fsm->gen_data->timer_used) {
	      return err;
	  }
      
	  id   = (timer_index == SIMPLE_STATE_TMR) ? (curr_state_p->id)   : curr_state_p->composed->id;
      name = (timer_index == SIMPLE_STATE_TMR) ? (curr_state_p->name) : curr_state_p->composed->name;
	  
	  if (fsm->reaction_in_state ||
		    (tm->state_timers[id] == 0)) {
        return err;
    }

    fsm_trace(fsm, "fsm_unschedule_timeout in state %s for %d msec\n", name, tm->state_timers[id]);

    err = fsm_tm_unsched(fsm, id);
    if (err != 0) {
        fsm_trace(fsm, "Timer was not unscheduled in state %s\n", name);
    }
    return err;
}

/*--------------------------------------------------------------
                       fsm_state_base_entry
   Purpose:
         Entry function called upon entry each  state
         possible actions - scheduler timer , call entryState function
   Input  :  fsm -  pointer to user's FSM
   Output :  error
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
int
fsm_state_base_entry(struct fsm_base *fsm, struct fsm_state_base *state, struct fsm_event_base *ev)
{
    int err = 0;
	
	if (fsm->gen_data->timer_used) {
	   struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
		
	   if (state->type == COMPOSED) {
          err = fsm_schedule_timeout(fsm, COMPOSED_STATE_TMR,
                                   tm->state_timers[state->id], state->id);
          if (err != 0) {
        	 goto bail;
          }
        }
	    else {
           err = fsm_schedule_timeout(fsm, SIMPLE_STATE_TMR,
                                   tm->state_timers[state->id], state->id);
           if (err != 0) {
        	  goto bail;
           }
      }
	}
	if (state->entry_func) {
        err = ((fsm_state_entry)(state->entry_func))(fsm, ev);
    }

bail:
    return err;
}

/*--------------------------------------------------------------
                       fsm_state_base_exit

   Purpose: Exit function called upon exit from each  state
          possible actions - unscheduler timer , call exitState function

   Input  : fsm -  pointer to user's FSM
   Output : error
   Precondition:

   Returns:  none
   ----------------------------------------------------------------------*/
int
fsm_state_base_exit(struct fsm_base *fsm, struct fsm_state_base *state, struct fsm_event_base *ev)
{
    int err = 0;

	if (fsm->gen_data->timer_used) {
        if (state->type == COMPOSED) {
           err = fsm_unschedule_timeout(fsm, COMPOSED_STATE_TMR);
           if (err != 0) {
        	  goto bail;
          }
        }else {
            err = fsm_unschedule_timeout(fsm, SIMPLE_STATE_TMR);
            if (err != 0) {
        	   goto bail;
         }
       }
	}
    if (state->exit_func) {
        err = ((fsm_state_exit)(state->exit_func))(fsm, ev);
    }

bail:
    return err;
}

/*---------------------------------------------------
   State Simple functions
   --------------------------------------------------*/

/*--------------------------------------------------------------------
           fsm_state_simple_entry
   redefinition of base behavior: first call Base Entry function  and
   then - call Entry function of Parent Composed state(if parent defined for this state)
   --------------------------------------------------------------------*/
int
fsm_state_simple_entry(struct fsm_base *fsm, struct fsm_state_base *current_state,
		               struct fsm_state_base *target_state, struct fsm_event_base *ev)
{
    int err = 0;

    err = fsm_state_base_entry(fsm,  target_state, ev);
    if (err != 0) {
    	goto bail;
    }

    if (target_state->composed) {
        err = fsm_state_composed_entry(fsm, current_state , target_state->composed, ev);
    }

bail:
	return err;
}

/*--------------------------------------------------------------------
           fsm_state_simple_exit
   redefinition of base behavior: first call Base Exit function  and
   then - call Exit function of Parent Composed state(if parent defined for this state)itsReactionInState
   --------------------------------------------------------------------*/
int
fsm_state_simple_exit(struct fsm_base *fsm, struct fsm_state_base *current_state,
		              struct fsm_state_base *target_state, struct fsm_event_base *ev)
{
    int err = 0;

    err = fsm_state_base_exit(fsm, current_state, ev);
    if (err != 0) {
    	goto bail;
    }

    if (current_state->composed) {
        err = fsm_state_composed_exit(fsm, current_state, target_state, ev);
    }

bail:
 	return err;
}

/*-------------------------------------------------
 *  State Composed functions
 *-------------------------------------------------*/

/*--------------------------------------------------------------------
           fsm_state_composed_entry
   if Composed state was changed as a result of event consumption
   then call Entry from the Base class
   --------------------------------------------------------------------*/
int
fsm_state_composed_entry(struct fsm_base *fsm, struct fsm_state_base *current_state,
		                 struct fsm_state_base *target_state, struct fsm_event_base *ev)
{
    int err = 0;

	if (!(current_state->composed) ||
		 (current_state->composed != target_state)) {

		err = fsm_state_base_entry(fsm, target_state, ev);
    }
 	return err;
}

/*--------------------------------------------------------------------
           fsm_state_composed_exit
   if Composed state was changed as a result of event consumption then called Exit method
   from the Base class

   --------------------------------------------------------------------*/
int
fsm_state_composed_exit(struct fsm_base *fsm, struct fsm_state_base *current_state,
		                struct fsm_state_base *target_state, struct fsm_event_base *ev)
{
    int err = 0;

    if (!(target_state->composed) ||
        (target_state->composed != current_state->composed) /* use LastSimpleState */
        /* ((target_state->itsComposed == fsm->itsCurrStateP)&&(fsm->itsLastConsumed == fsm->itsCurrStateP->itsComposed )) New condition ! to validate arc on composed */
       ) {
        err = fsm_state_base_exit(fsm, current_state->composed, ev);
    }
   	return err;
}

/******************************************Timer functions**************************************************************/

int
fsm_tm_sched(struct fsm_base *fsm, int timeout, enum fsm_timer_index timer_index,
             unsigned char id)
{
    int err = 0;
	struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
    struct timer_mngr *tmr = (struct timer_mngr*) &(tm->tmr_mngr);
	void *sched = fsm->gen_data->tm_sched_func;

    if (timer_index < LAST_INDEX_TMR) {
        tmr->client_data[timer_index].fsm_timer_id = id;
        tmr->client_data[timer_index].scheduled_status = 1;
        tmr->client_data[timer_index].fsm = fsm;

        /*return lew_event_reg_timer(tmr->tmr_context, &tmr->itsClientData[theTimerIndex].itsTimerRef, tmr->tmrMngr.itsCallback ,  (void *)&tmr->itsClientData[theTimerIndex], theTimeout); */
        if (sched) {
            err = ((real_tm_sched)sched)(timeout,
                                         (void *)&tmr->client_data[timer_index],
                                         &tmr->client_data[timer_index].timer_ref);
        }
    }
    return err;
}

/* ----------------------------------------------------------------------------------- */
int
fsm_tm_unsched(struct fsm_base *fsm, unsigned char id)
{
    int err = 0;
    int i;
	struct fsm_tm_base * tm = (struct fsm_tm_base *)&fsm[1];
    struct timer_mngr *tmr = (struct timer_mngr*) &(tm->tmr_mngr);
	void *unsched = fsm->gen_data->tm_unsched_func;

    for (i = SIMPLE_STATE_TMR; i < LAST_INDEX_TMR; i++) {
        if (tmr->client_data[i].fsm_timer_id == id) {
            if (unsched) {
                err = ((real_tm_unsched)unsched )(&tmr->client_data[i].timer_ref);
            }
            tmr->client_data[i].scheduled_status = 0;
            break;
        }
    }
    if (i == LAST_INDEX_TMR) {
        /* error */
    }
    return err;
}

/* ----------------------------------------------------------------------------------- */
int
fsm_tm_init(struct timer_mngr *timer_manager)
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
fsm_tm_deinit(struct timer_mngr *timer_manager , void *unsched)
{
    int i;

    for (i = SIMPLE_STATE_TMR; i < LAST_INDEX_TMR; i++) {
        if (timer_manager->client_data[i].scheduled_status) {
            /*rc = IPC_removeTimerByRef(itsClientData[i].itsTimerRef);//lew_event_cancel(tmr->tmr_context ,&tmr->itsClientData[theTimerIndex].itsTimerRef);*/
            if (unsched) {
                ((real_tm_unsched)unsched)(&timer_manager->
                                                               client_data[i].
                                                               timer_ref);
            }
            timer_manager->client_data[i].scheduled_status = 0;
            timer_manager->client_data[i].timer_ref = 0;
            break;
        }
    }
    return 0;
}
