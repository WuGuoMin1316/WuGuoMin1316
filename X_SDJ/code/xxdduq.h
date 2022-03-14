/****调度器开始**********************************************/
#ifndef __XXDDUQ_H__
#define __XXDDUQ_H__

#define MAXTASKS 5U
extern unsigned char timers[MAXTASKS];
#define _SS                        \
    static unsigned char _lc = 0U; \
    switch (_lc)                   \
    {                              \
    case 0U:
#define _EE       \
    break;        \
    default:      \
        break;    \
        }         \
        _lc = 0U; \
        return 255U;
#define WaitX(tickets)                           \
    _lc = (unsigned char)(__LINE__) % 255U + 1U; \
    if (_lc)                                     \
    {                                            \
        return (tickets);                        \
    }                                            \
    break;                                       \
    case ((unsigned char)(__LINE__) % 255U) + 1U:

#define RunTask(TaskName, TaskID)            \
    do                                       \
    {                                        \
        if (timers[(TaskID)] == 0U)          \
        {                                    \
            timers[(TaskID)] = (TaskName)(); \
        }                                    \
    } while (0)
#define RunTaskA(TaskName, TaskID)           \
    {                                        \
        if (timers[(TaskID)] == 0U)          \
        {                                    \
            timers[(TaskID)] = (TaskName)(); \
            continue;                        \
        }                                    \
    } /*前面的任务优先保证执行*/

#define CallSub(SubTaskName)                         \
    do                                               \
    {                                                \
        unsigned char currdt;                        \
        _lc = (unsigned char)(__LINE__) % 255U + 1U; \
        if (_lc)                                     \
        {                                            \
            return 0U;                               \
        }                                            \
        break;                                       \
    case (unsigned char)(__LINE__) % 255U + 1U:      \
        currdt = (SubTaskName)();                    \
        if (currdt != 255U)                          \
        {                                            \
            return currdt;                           \
        }                                            \
    } while (0);

#define InitTasks()                     \
    do                                  \
    {                                   \
        unsigned char i;                \
        for (i = MAXTASKS; i > 0U; i--) \
        {                               \
            timers[i - 1U] = 0U;        \
        }                               \
    } while (0)
    
#define UpdateTimers()                      \
    do                                      \
    {                                       \
        unsigned char i;                    \
        for (i = MAXTASKS; i > 0U; i--)     \
        {                                   \
            if (timers[i - 1U] != 0U)       \
            {                               \
                if (timers[i - 1U] != 255U) \
                {                           \
                    timers[i - 1U]--;       \
                }                           \
            }                               \
        }                                   \
    } while (0)
    
/*使task_id号任务里的\延时完成或者信号到达*/
#define set_ready(task_id)   \
    do                 \
    {                  \
        timers[task_id] = 0; \
    } while (0);

    /*使task_id号任务停止*/
#define set_end(task_id)   \
    do                 \
    {                  \
        timers[task_id] = 255U; \
    } while (0);	
	
	
#define SEM unsigned int
/*初始化信号量*/
#define InitSem(sem) (sem) = 0;
/*等待信号量*/
#define WaitSem(sem)   \
    do                 \
    {                  \
        (sem) = 1;     \
        WaitX(0);      \
        if ((sem) > 0) \
            return 1;  \
    } while (0);
/*等待信号量或定时器溢出， 定时器tickets 最大为0xFFFE*/
#define WaitSemX(sem, tickets) \
    do                         \
    {                          \
        (sem) = (tickets) + 1; \
        WaitX(0);              \
        if ((sem) > 1)         \
        {                      \
            (sem)--;           \
            return 1;          \
        }                      \
    } while (0);
/*发送信号量*/
#define SendSem(sem) \
    do               \
    {                \
        (sem) = 0;   \
    } while (0);
#endif

// static unsigned char task0(void)
// {
//     static int i;
//     _SS for (i = 0; i < 11; i++)
//     {
//         WaitX(10U);
//     }
//     _EE
// }

// static unsigned char task1(void)
// {

//     _SS while (1)
//     {
//         WaitX(10U);
//     }
//     _EE
// }

// static unsigned char task2(void)
// {

//     _SS while (1)
//     {
//         WaitX(10U);
//     }
//     _EE
// }

// void InitT0()
// {
//     TMOD = 0x21;
//     IE |= 0x82; // 12t
//     TL0 = 0Xff;
//     TH0 = 0XDB;
//     TR0 = 1;
// }

// void INTT0(void) interrupt 1 using 1
// {
//     TL0 = 0Xff; //10ms 重装
//     TH0 = 0XDB; //b7;
//     UpdateTimers();

//     RunTask(task0, 0); //任务0具有精确按时获得执行的权限，要求：task0每次执行消耗时间<0.5个 ticket
// }

// int main(void)
// {
//     InitT0();
//     InitTasks();
//     while (1)
//     {
//         RunTaskA(task1, 1); //任务一具有比任务二优先执行的权限
//         RunTaskA(task2, 2);
//     }
// }