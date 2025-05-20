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
    if (fopen_s(&Output_fp, OUTPUT_FILE_NAME, "w") != 0)
        printf("error to clear output file");
}

void InputFile() {
    errno_t err;
    FILE* fp;
    char str[MAX];
    char* ptr, * pTmp = NULL;
    int values[4];  // 4 column
    int count;
    int j = 0;
    TASK_NUMBER = 0;
    Aperiod_NUMBER = 0;

    if ((err = fopen_s(&fp, INPUT_FILE_NAME, "r")) != 0) {
        printf("The file 'TaskSet.txt' was not opened\n");
        return;
    }

    printf("The file 'TaskSet.txt' was opened\n");

    while (fgets(str, sizeof(str), fp)) {
        // blank row ,skip
        if (str[0] == '\n' || str[0] == '\0') continue;

        count = 0;
        ptr = strtok_s(str, " \t\r\n", &pTmp);
        while (ptr != NULL && count < 4) {
            values[count++] = atoi(ptr);
            ptr = strtok_s(NULL, " \t\r\n", &pTmp);
        }

        if (count == 4) {
            // Periodic task
            TaskParameter[TASK_NUMBER].TaskID = values[0];
            TaskParameter[TASK_NUMBER].TaskPriority = values[0];
            TaskParameter[TASK_NUMBER].TaskArriveTime = values[1];
            TaskParameter[TASK_NUMBER].TaskExecutionTime = values[2];
            TaskParameter[TASK_NUMBER].TaskPeriodic = values[3];
            p2id[values[0]] = values[0];
            TASK_NUMBER++;
        }
        else if (count == 2) {
            // Server
            Server_Para.TaskID_server= values[0];
            TaskParameter[TASK_NUMBER].TaskID = values[0];
            TaskParameter[TASK_NUMBER].TaskPriority = values[0];
            TaskParameter[TASK_NUMBER].TaskArriveTime = 0;
            TaskParameter[TASK_NUMBER].TaskExecutionTime = 0;
            TaskParameter[TASK_NUMBER].TaskPeriodic = 0; // aperiodic
            server_cur = 0;
            Server_Para.Size = values[1];
            Server_Para.Deadline = 0;
            TASK_NUMBER++;
        }
    }
    fclose(fp);


    // aperiodic jobs
    if ((err = fopen_s(&fp, "AperiodicJobs.txt", "r")) != 0) {
        printf("The file 'AperiodicJobs.txt' was not opened\n");
        return;
    }

    printf("The file 'AperiodicJobs.txt' was opened\n");

    while (fgets(str, sizeof(str), fp)) {
        if (str[0] == '\n' || str[0] == '\0') continue;

        count = 0;
        ptr = strtok_s(str, " \t\r\n", &pTmp);
        while (ptr != NULL && count < 4) {
            values[count++] = atoi(ptr);
            ptr = strtok_s(NULL, " \t\r\n", &pTmp);
        }

        if (count == 4) {
            AperiodList[Aperiod_NUMBER].TaskID = values[0];
            AperiodList[Aperiod_NUMBER].TaskArriveTime = values[1];
            AperiodList[Aperiod_NUMBER].TaskExecutionTime = values[2];
            AperiodList[Aperiod_NUMBER].TaskDeadLine = values[3];
            Aperiod_NUMBER++;
        }
    }

    fclose(fp);

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
    //printf("tick: %2d =====================\n",OSTime);
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif



#endif
