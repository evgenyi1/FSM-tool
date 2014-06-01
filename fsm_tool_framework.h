
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
#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef uint16
typedef unsigned short uint16;
#endif
#ifndef uint32
typedef unsigned int uint32;
#endif
#ifndef tbool
typedef unsigned int tbool;
#endif
#endif

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(PARAM) ((void)(PARAM))
#endif

#define SET_EVENT(class_name, spec_event) \
    struct class_name ## _ ## spec_event ## _t * ev = \
        (struct class_name ## _ ## spec_event ## _t *)(event); \
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
 enum fsm_state_types {
    CONDITION,
    SIMPLE,
    COMPOSED
} fsm_state_types;

/* ------------------------------------------------------------------------ */
/* enum defines indexes for all simultaneously scheduled timers for FSM */
/* ------------------------------------------------------------------------ */
 enum fsm_timer_index {
    AUX_TMR,
    SIMPLE_STATE_TMR,
    COMPOSED_STATE_TMR,
    LAST_INDEX_TMR
} fsm_timer_index;

/* ------------------------------------------------------------------------ */
/* enum defines types of consume results */
/* ------------------------------------------------------------------------ */
 enum fsm_consume_results {
    FSM_CONSUMED             = 0, /* event consumed */
    FSM_NOT_CONSUMED         = 1, /* event not handled (ignored) by fsm : normal condition */
    FSM_CONSUME_ERROR        = 2, /* event erroneously consumed - example: stucked in condition state */
    FSM_PARAMS_ERROR         = 3,
    FSM_TIMER_USING_ERROR    = 4,

    FSM_LAST_FRAMEWORK_ERROR = 100
    /* User errors should be added after that */
} ;

/* ********************************************************************************************************** */
/* TimerMngr */
/* Defines interfaces to Timer Manager */
/* User have to define module that interfaces with Real Timer manager of the system. */
/*  */
/* Goals of TimerMngr : 1.FSM Automatically Scheduler timer upon entering state with timeout. */
/*                      2.FSM framework also unschedule timer upon exit from state with timer. */
/*                      3.User defined Auxiliary timer have to be explicitly scheduled and unscheduled. */
/* ********************************************************************************************************** */
struct timer_params {
    void           *timer_ref;
    void           *fsm;
    unsigned char  scheduled_status;
	unsigned char  fsm_timer_id;
};

 struct timer_mngr {
    struct timer_params client_data[LAST_INDEX_TMR];
} ;

/* ------------------------------------------------------------------------ */
/* Class EventBase - base class for events of real FSM */
/* ------------------------------------------------------------------------ */
struct fsm_event_base {
    int opcode;
    const char *name;
} ;

/* ------------------------------------------------------------------------ */
/* Specific type of FSM events - Timer event */
/* ------------------------------------------------------------------------ */
struct fsm_timer_event {
    int opcode;
    const char     *name;
    unsigned short id;
} ;


typedef void (*fsm_user_trace) (char *buf, int len);
struct fsm_state_base;

struct fsm_static_data{
	fsm_user_trace         user_trace;             /* user trace callback*/ 
	unsigned char          num_states;             /*  number states in the state machine */
	uint8                  timer_used;             /* flag*/
	void                   *tm_sched_func;       
    void                   *tm_unsched_func;
	struct fsm_state_base  * states[60];
} ;

/*##################################################
 * Base struct for State . Defines default behavior
 *##################################################*/
struct fsm_state_base {
    const char            *name;
    unsigned char          id;
    enum fsm_state_types   type;
    struct fsm_state_base  *composed;         /* points to its Composed state if parent state exist for this state */
    struct fsm_state_base  *default_substate; /* if state is Composed it has default substate */
    void                   *state_dispatcher;
    void                   *entry_func;
    void                   *exit_func;
} ;


typedef int (*fsm_state_entry)(void *fsm, struct fsm_event_base *ev);
typedef int (*fsm_state_exit) (void *fsm, struct fsm_event_base *ev);
typedef int (*fsm_state_dispatch) (void *fsm, unsigned short state_id,
		                           struct fsm_event_base *ev,
		                           struct fsm_state_base** target_state);

/*#################################################
 * Base struct for FSM. Defines default behavior
 * all user's FSMs include it
 ##################################################*/
 struct fsm_base {
    struct fsm_static_data    *gen_data;
	unsigned char             curr_state;                 /* id */
	unsigned char             prev_state;                 /* id */
    
	unsigned short            reaction_in_state:1;
    unsigned short            busy_flag:1;                /* to avoid recursive entry to FSM */
	unsigned short            consumed_in_composed:1;     /* flag risen if last event was consumed in composed state */
};


 struct fsm_tm_base {
	struct timer_mngr         tmr_mngr;                   /* integrated timer manager */
	int                       *state_timers;              /* pointer to the array of timers */
};


#ifndef WIN32
void fsm_trace(struct fsm_base *fsm, const char *format,
               ...) __attribute__((format(printf, 2, 3)));
#else
void fsm_trace(struct fsm_base *fsm, const char *format, ...);
#endif

int fsm_init(struct fsm_base *fsm, struct fsm_state_base *curr_state, uint8 num_states,
             int *state_timer, fsm_user_trace user_trace, void *timer_sched,
             void *timer_unsched, struct fsm_static_data * static_data);
int fsm_deinit(struct fsm_base *fsm);
int fsm_schedule_aux_timer(struct fsm_base *fsm, int timeout);
int fsm_unschedule_aux_timer(struct fsm_base *fsm);
void fsm_set_timer_value(struct fsm_base *fsm, int state, int timer_value);
int fsm_get_timer_value(struct fsm_base *fsm, int state);
const char *fsm_get_state(struct fsm_base *fsm, unsigned short *state);
tbool fsm_is_in_state(struct fsm_base *fsm, unsigned short state);
int fsm_timer_trigger_operation(struct timer_params *timer_data);
int fsm_handle_event(struct fsm_base *fsm, struct fsm_event_base *ev);
void fsm_print(struct fsm_base *fsm);

#endif
