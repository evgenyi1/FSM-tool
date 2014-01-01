
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

/***************************************************************
File: FsmFramework.h 
 Class/Module: C Fsm Framework 

 Purpose: 
Description: 

 Modification history:
         9/1/2009   [LEONIDC]  Created 

*************************************************************** */
#ifndef FSM_FRAMEWORK_H
#define FSM_FRAMEWORK_H

#ifndef WIN32
#include "ttypes.h"
#endif

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(PARAM) ((void)(PARAM))
#endif

#define SET_EVENT(class_name, event) \
    class_name ## _ ## event ## _t * ev = \
        (class_name ## _ ## event ## _t *)(fsm->base.current_event); \
    UNUSED_PARAM(ev);

#define NULL_EVENT        10001  /* for condition state - state used only for check conditions */
#define TIMER_EVENT       10002

/* auxiliary timer */
#define AUX_TIMER_EVENT   10006
#define AUX_TIMEOUT_ID    10007

#define NO_STATE          250
#define DEFAULT_PARAMS_D  0xffdf
#define TRACER_BUF_LEN_D  300

#ifdef WIN32
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned int tbool;
#endif

/* Timer functions definition */
typedef int (*real_tm_unsched) (void **timer_handler);
typedef int (*real_tm_sched  ) (int timeout, void *data, void **timer_handler);

/* ------------------------------------------------------------------------ */
/* enum defines 3 type of states  derived from the Base class StateBase */
/* ------------------------------------------------------------------------ */
typedef enum fsm_state_types {
    CONDITION,
    SIMPLE,
    COMPOSED
} fsm_state_types;

/* ------------------------------------------------------------------------ */
/* enum defines indexes for all simultaneously scheduled timers for FSM */
/* ------------------------------------------------------------------------ */
typedef enum fsm_timer_index {
    AUX_TMR,
    SIMPLE_STATE_TMR,
    COMPOSED_STATE_TMR,
    LAST_INDEX_TMR
} fsm_timer_index;

/* ------------------------------------------------------------------------ */
/* enum defines types of consume results */
/* ------------------------------------------------------------------------ */
typedef enum fsm_consume_results {
    FSM_CONSUMED,     /* event consumed ok */
    FSM_NOT_CONSUMED, /* event not handled by fsm : normal condition */
    FSM_CONSUME_ERROR /* event erroneously consumed - example: stucked in condition state */
} fsm_consume_results;

/* ********************************************************************************************************** */
/* TimerMngr */
/* Defines interfaces to Timer Manager */
/* User have to define module that interfaces with Real Timer manager of the system. */
/*  */
/* Goals of TimerMngr : 1.FSM Automatically Scheduler timer upon entering state with timeout. */
/*                      2.FSM framework also unschedule timer upon exit from state with timer. */
/*                      3.User defined Auxiliary timer have to be explicitly scheduled and unscheduled. */
/* ********************************************************************************************************** */
typedef struct timer_params {
    unsigned short fsm_timer_id;
    void           *fsm;
    tbool scheduled_status;
    void           *timer_ref;
} timer_params;

typedef struct timer_mngr {
    void         *sched_func;
    void         *unsched_func;
    timer_params client_data[LAST_INDEX_TMR];
} timer_mngr;

/* ------------------------------------------------------------------------ */
/* Class EventBase - base class for events of real FSM */
/* ------------------------------------------------------------------------ */
typedef struct fsm_event_base {
    int opcode;
    const char *name;
} fsm_event_base;

/* ------------------------------------------------------------------------ */
/* Specific type of FSM events - Timer event */
/* ------------------------------------------------------------------------ */
typedef struct fsm_timer_event {
    int opcode;
    const char     *name;
    unsigned short id;
} fsm_timer_event;

/* ################################################## */
/* Base struct for State . Defines default behavior */
/* ################################################## */
typedef struct fsm_state_base {
    const char            *name;
    unsigned short id;
    fsm_state_types type;
    struct fsm_state_base *composed;         /* points to its Composed state if parent state exist for this state */
    struct fsm_state_base *default_substate; /* if state is Composed it has default substate */
    void                  *state_dispatcher;
    void                  *entry_func;
    void                  *exit_func;
} fsm_state_base;

typedef void (*fsm_user_trace) (char *buf, int len);
typedef void (*fsm_state_entry)(void *fsm);
typedef void (*fsm_state_exit) (void *fsm);
typedef fsm_state_base* (*fsm_state_dispatch) (void *fsm,
                                               unsigned short state_id);

/* ################################################## 
  Base struct for FSM. Defines default behavior 
 all user's FSMs include it 
 ################################################## */
typedef struct fsm_base {
    fsm_state_base      *curr_state;          /*  points to current state static Class */
    fsm_state_base      *prev_state;          /*  points to the previous state */
    fsm_state_base      *last_simple_state;   /*  points to the last simple state (to get rid of all condition states between 2 real states) */
    fsm_event_base      *current_event;
    fsm_user_trace      user_trace;

    unsigned char        reaction_in_state:1;
    unsigned char        busy_flag:2;              /* to avoid recursive entry the FSMs */
	unsigned char        consumed_in_composed:3; /*  flag risen if last event was consumed in composed state */
    int                  err;
	unsigned char        num_states;             /*  number states in the state machine */

	timer_mngr    tmr_mngr;                      /*  integrated timer manager */
	int*          state_timers;         /*  pointer to the array of timers */
} fsm_base;

#ifndef WIN32
void fsm_trace(fsm_base *fsm, const char *format,
               ...) __attribute__((format(printf, 2, 3)));
#else
void fsm_trace(fsm_base *fsm, const char *format, ...);
#endif

int fsm_init(fsm_base *fsm, fsm_state_base *curr_state, uint8 num_states,
             int *state_timer, fsm_user_trace user_trace, void *timer_sched,
             void *timer_unsched);
int fsm_deinit(fsm_base *fsm);
int fsm_schedule_aux_timer(fsm_base *fsm, int timeout);
int fsm_unschedule_aux_timer(fsm_base *fsm);
void fsm_set_timer_value(fsm_base *fsm, int state, int timer_value);
int fsm_get_timer_value(fsm_base *fsm, int state);
void fsm_timer_trigger_operation(fsm_base *fsm, unsigned short id);
fsm_consume_results fsm_handle_event(fsm_base *fsm, fsm_event_base *ev);
fsm_state_base* fsm_state_handle_event(fsm_base *fsm, fsm_state_base *state,
                                       fsm_event_base *ev);
#endif
