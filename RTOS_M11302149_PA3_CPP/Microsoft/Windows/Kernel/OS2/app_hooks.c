/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                          Application Hooks
*
* Filename : app_hooks.c
* Version  : V2.92.13
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <os.h>


/*
*********************************************************************************************************
*                                      EXTERN  GLOBAL VARIABLES
*********************************************************************************************************
*/


#define TASK_STACKSIZE      2048;

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

void OutFileInit()
{
    /*Clear the file */
    if ((Output_err = fopen_s(&Output_fp, OUTPUT_FILE_NAME, "w"))!= 0)
        printf("error to clear output file");
}






void InputFile(void) {
    R1_CEILING_PRIORITY = 255;
    R2_CEILING_PRIORITY = 255;

    errno_t err;
    if ((err = fopen_s(&fp, INPUT_FILE_NAME, "r")) == 0) {
        printf("The file 'TaskSet.txt' was opened\n");
    }
    else {
        printf("The file 'TaskSet.txt' was not opened\n");
        return;
    }

    char str[MAX];
    char* ptr;
    char* pTmp = NULL;
    int TaskInfo[INFO];  // Expecting INFO >= 8
    int i, j = 0;
    TASK_NUMBER = 0;

    // --- Temporary mapping to sort by period ---
    typedef struct {
        int idx;
        int period;
    } TaskMap;
    TaskMap task_map[OS_MAX_TASKS];

    // --- Step 1: Read all task entries ---
    while (fgets(str, sizeof(str), fp)) {
        if (str[0] == '\n' || str[0] == '\0') continue;
        i = 0;
        ptr = strtok_s(str, " \t\r\n", &pTmp);
        while (ptr != NULL && i < INFO) {
            TaskInfo[i++] = atoi(ptr);
            ptr = strtok_s(NULL, " \t\r\n", &pTmp);
        }
        if (i < 8) continue;

        TaskParameter[j].TaskID = TaskInfo[0];
        TaskParameter[j].TaskArriveTime = TaskInfo[1];
        TaskParameter[j].TaskExecutionTime = TaskInfo[2];
        TaskParameter[j].TaskPeriodic = TaskInfo[3];  // Will use for RM
        TaskParameter[j].R1_LockTime = TaskInfo[4];
        TaskParameter[j].R1_UnlockTime = TaskInfo[5];
        TaskParameter[j].R2_LockTime = TaskInfo[6];
        TaskParameter[j].R2_UnlockTime = TaskInfo[7];

        task_map[j].idx = j;
        task_map[j].period = TaskInfo[3];

        TASK_NUMBER++;
        j++;
    }

    fclose(fp);

    // Step 2: Sort tasks by period (RM): smaller period ¡÷ higher priority
    for (int x = 0; x < TASK_NUMBER - 1; x++) {
        for (int y = 0; y < TASK_NUMBER - 1 - x; y++) {
            if (task_map[y].period > task_map[y + 1].period) {
                // swap
                TaskMap temp = task_map[y];
                task_map[y] = task_map[y + 1];
                task_map[y + 1] = temp;
            }
        }
    }


    // --- Step 3: Assign priority = multiples of 3 (3, 6, 9, ...) ---
    for (int k = 0; k < TASK_NUMBER; k++) {
        int idx = task_map[k].idx;
        TaskParameter[idx].TaskPriority = (k + 1) * 3;  // priority: 3,6,9,...
        p2id[TaskParameter[idx].TaskPriority] = TaskParameter[idx].TaskID;
    }

    // --- Step 4: Compute Resource Ceiling Priorities ---
    for (int i = 0; i < TASK_NUMBER; i++) {
        int prio = TaskParameter[i].TaskPriority;

        if (TaskParameter[i].R1_LockTime > 0 || TaskParameter[i].R1_UnlockTime > 0) {
            if (prio < R1_CEILING_PRIORITY) {
                R1_CEILING_PRIORITY = prio;
            }
        }

        if (TaskParameter[i].R2_LockTime > 0 || TaskParameter[i].R2_UnlockTime > 0) {
            if (prio < R2_CEILING_PRIORITY) {
                R2_CEILING_PRIORITY = prio;
            }
        }
    }

    // Optional: Print results for debug
    printf("R1 ceiling priority = %d - 1 = %d\n", R1_CEILING_PRIORITY, R1_CEILING_PRIORITY - 1);
    printf("R2 ceiling priority = %d - 2 = %d\n", R2_CEILING_PRIORITY, R2_CEILING_PRIORITY - 2);
}



   
/*
*********************************************************************************************************
*********************************************************************************************************
**                                        uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)

/*
*********************************************************************************************************
*                                  TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
    printf("Task[%3d] created, TCB Address\t%06x\n", p2id[ptcb->OSTCBPrio], ptcb);

    
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}


/*
*********************************************************************************************************
*                                  TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}


/*
*********************************************************************************************************
*                                    IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif


/*
*********************************************************************************************************
*                                  STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}


/*
*********************************************************************************************************
*                                   TASK RETURN HOOK (APPLICATION)
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : ptcb      is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
*********************************************************************************************************
*/


#if OS_VERSION >= 289
void  App_TaskReturnHook (OS_TCB  *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                   TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
    
    
#if (APP_CFG_PROBE_OS_PLUGIN_EN > 0) && (OS_PROBE_HOOKS_EN > 0)
    printf("Tick: %d, CurrentTask Prio: %d, NextTask Prio: %d, ## Number of ctx switch: %d\n",
        OSTime,
        OSPrioCur,
        OSPrioHighRdy,
        OSCtxSwCtr);
    OSProbe_TaskSwHook();
#endif
}
#endif


/*
*********************************************************************************************************
*                                   OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                       TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
    //printf("tick: %2d task %u \n",OSTime,OSTCBCur->TaskID);
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
