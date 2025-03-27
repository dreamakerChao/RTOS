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
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include  "app_cfg.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
#define TASK_STACKSIZE      2048
static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
extern FILE* Output_fp;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask (void  *p_arg);
static void task(void* p_arg);
void TraverseTCBList(void);
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : none
*********************************************************************************************************
*/

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif
    

    p2id[63] = 63;

    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();                                                   /* Initialize uC/OS-II                                  */

    /*lnitialize Output Fi1e*/
    OutFileInit();
    /*lnput Fi1e*/
    InputFile();
    
    /* Dynamic Create the Stack size */
    Task_STK = malloc(TASK_NUMBER * sizeof(int*));

    /* for each pointer, allocate storage for an array of ints */
    int n;
    for (n = 0; n < TASK_NUMBER; n++) {
        Task_STK[n] = malloc(TASK_STACKSIZE * sizeof(int));
    }

/*
    OSTaskCreateExt( StartupTask,                               /* Create the startup task                      
                     0,
                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u],
                     APP_CFG_STARTUP_TASK_PRIO,
                     APP_CFG_STARTUP_TASK_PRIO,
                    &StartupTaskStk[0u],
                     APP_CFG_STARTUP_TASK_STK_SIZE,
                     0u,
                    (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(         APP_CFG_STARTUP_TASK_PRIO,
                  (INT8U *)"Startup Task",
                           &os_err);
#endif
*/


    /* Creat Task Set */
    for (int i =0 ;i<TASK_NUMBER; i++)
    {
        OSTaskCreateExt(task,
            &TaskParameter[i],
            &Task_STK[i][TASK_STACKSIZE - 1],
            TaskParameter[i].TaskPriority,
            TaskParameter[i].TaskID,
            &Task_STK[i][0],
            TASK_STACKSIZE,
            &TaskParameter[i],
            (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    }

    /*Create Task Set*/

    TraverseTCBList();
    OSTimeSet(0);
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}



/*go thru TCB*/
void TraverseTCBList(void) {
    OS_TCB* ptcb = OSTCBList;
    printf("=================TCB linked list===================\n");
    printf("Task\tPrev_TCB_addr\t    TCB_addr\t Next_TCB_addr\n");

    while (ptcb != (OS_TCB*)0) {
        printf("%2d\t     %6x\t      %6x\t      %6x\n",
            p2id[ptcb->OSTCBPrio],
            (unsigned int)ptcb->OSTCBPrev,
            (unsigned int)ptcb,
            (unsigned int)ptcb->OSTCBNext);
        ptcb = ptcb->OSTCBNext;
    }
}
/*go thru TCB*/



/*
*********************************************************************************************************
*                                            MY TASK
*
* Description : Hw1 tasks
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void task(void* p_arg) {
    task_para_set* task_data = (task_para_set*)p_arg;
    INT8U id = task_data->TaskID;

    OS_TCB* ptcb = OSTCBPrioTbl[OSPrioCur];

    while (1) {

        //printf("Task %d running at tick %d, ramaining: %d\n", ptcb->TaskID, OSTime, ptcb->remaining);

        BusyWait(1, task_data);

    }
}
