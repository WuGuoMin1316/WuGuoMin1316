/****************************************************************************/
/*	include files
*****************************************************************************/
#include "cms8s6990.h"
#include "xxdduq.h"
#include "user_h.h"

/****************************************************************************/
/*	Local pre-processor symbols('#define')
*****************************************************************************/

/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/


/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/
// uint32_t Systemclock = 48000000;

volatile unsigned char timers[MAXTASKS];//������ʹ�ö�ʱ��
/****************************************************************************/
/*	Local function prototypes('static')  ���ܱ���һ���ļ�����
*****************************************************************************/


/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/

/*****************************************************************************
 ** \brief	 main
 **
 ** \param [in]  none   
 **
 ** \return 0
 *****************************************************************************/


static unsigned char task0(void)
{
    _SS 
    while (1)
    {
        WaitX(255U);
        duge_uart_test();	
    }
    _EE
}
static unsigned char task1(void)
{
    _SS 
    while (1)
    {
        WaitX(5U);
        ADC_GO();			
    }
    _EE
}

static unsigned char task2(void)
{
    _SS 	
	while (1)
    {
        WaitX(20);
        DC_IN_MODE_CHANGEING();
        change_battery();
		RunSpeed_GradualChange();
		Robot_RunMode();
		sys_sweep_time();
    }
    _EE
}
static unsigned char task3_KeyAndDisplay(void)
{
    _SS 
	led_Scan_Display();
	WaitX(10U);
	TM1652_Light_init(15,4,0);
    while (1)
    {
        WaitX(10U);
		KEY_Serve();
		
		if(LED_DISPLAY_OFF)
			TM1652_Light_init(breath_effect(),4,0);//��ɹ���ʱ����Ҫ���ȵ���			
		else		
			led_Scan_Display();
		if(Robot_On_Flag)
			LED_DISPLAY_OFF = ~LED_DISPLAY_OFF;
    }
    _EE
}

int main(void)
{
    x_6990_all_init();
    InitTasks();
	Data_init();
    while (1)
    {
        RunTaskA(task0, 0);
        RunTaskA(task1, 1); //����һ���б����������ִ�е�Ȩ��
        RunTaskA(task2, 2);
		RunTaskA(task3_KeyAndDisplay,3);
		RunTaskA(task4_sys_stop, 4);
    }
}
