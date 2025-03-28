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
    if ((Output_err = fopen_s(&Output_fp, OUTPUT_FILE_NAME, "w")) == 0)
        fclose(Output_fp);
    else
        printf("error to clear output file");
}






void InputFile() {
    /*Read File
    * Task Information :
    *Task ID ArriveTime Execution Time Periodic
    */
    errno_t err;
    if ((err = fopen_s(&fp, INPUT_FILE_NAME, "r")) == 0)
    {
        printf("The file 'TaskSet.txt' was opened\n");
    }
    else
    {
        printf("The file 'TaskSet.txt' was not opened\n");
    }
    char str[MAX];
    char* ptr;
    char* pTmp = NULL;
    int TaskInfo[INFO], i, j = 0;
    TASK_NUMBER = 0;


    while (!feof(fp))
    {
        i = 0;
        memset(str, 0, sizeof(str));
        fgets(str, sizeof(str) - 1, fp);
        ptr = strtok_s(str, " ", &pTmp);
        while (ptr != NULL)
        {
            TaskInfo[i] = atoi(ptr);
            ptr = strtok_s(NULL, " ", &pTmp);
            /*printf("Info: %d\n", task_inf[i]);*/
            if (i == 0) {
                TASK_NUMBER++;
                TaskParameter[j].TaskID = TaskInfo[i];
            }
            else if (i == 1)
                TaskParameter[j].TaskArriveTime = TaskInfo[i];
            else if (i == 2)
                TaskParameter[j].TaskExecutionTime = TaskInfo[i];
            else if (i == 3)
            {
                TaskParameter[j].TaskPeriodic = TaskInfo[i];
                TaskParameter[j].TaskPriority = TaskInfo[i];
                p2id[TaskParameter[j].TaskPriority] = TaskParameter[j].TaskID;
            }

            i++;
        }

        j++;
    }
    fclose(fp);
    /*read file*/

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
    INT32U now = OSTime;
    //printf("TaskSW\n");
    OS_TCB* ptcb = OSTCBCur;

    if (OSTCBCur->state == 3) //done
    {
        printf("%2u\tCompletion\ttask(%2d)(%2d)\ttask(%2d)(%2d)\t%d\t%d\t%d\n",
            OSTime,
            OSTCBCur->TaskID, OSTCBCur->TaskNumber,
            OSTCBHighRdy->TaskID, OSTCBHighRdy->TaskNumber,
            OSTime - OSTCBCur->ArriveTime,
            OSTime - OSTCBCur->ArriveTime - OSTCBCur->execution_time,
            OSTCBCur->deadline - OSTime);

        ptcb->ArriveTime += ptcb->period;
        //ptcb->remaining = ptcb->execution_time;
        ptcb->state = 4;
        //ptcb->deadline += ptcb->period;
    }


    if (OSTCBHighRdy->state == 1 &&OSTime != 0) //arrival , and run 
    {
        OSTCBHighRdy->start_time = OSTime;
        OSTCBHighRdy->state = 2;
        OSTCBHighRdy->remaining = OSTCBHighRdy->execution_time;
    }
    if (OSTCBCur->state == 2 && OSTime>0 && OSTCBHighRdy != OSTCBCur) {
        printf("%2u\tPreemption\ttask(%2d)(%2d)\ttask(%2d)(%2d)\n",
            OSTime,
            OSTCBCur->TaskID, OSTCBCur->TaskNumber,
            OSTCBHighRdy->TaskID, OSTCBHighRdy->TaskNumber);
    }
    
    
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
    printf("tick: %2d =====================\n",OSTime);
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
