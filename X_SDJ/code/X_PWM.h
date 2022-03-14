#ifndef _X_PWM_H_
#define _X_PWM_H_
#include "cms8s6990.h"
#include "6990_init.h"
#include "printf_uart.h"
#include "X_ADC.h"
#include "KEY_ENUM.h"
#include "RUNMODE_ENUM.h"


#define CHANGE_PWMPeriod_MAX	480//100Khz

#define	CHARGER						EPWM0//充电模式使用
#define	WATER_MOTOR_1				EPWM0//水泵
#define	BRUSH_PWM_1					EPWM1//滚刷  低电平驱动
#define	BRUSH_PWM_2					EPWM2//小毛刷
#define	electromagnetic_1			EPWM3//电磁阀1(清水箱)
#define	electromagnetic_2			EPWM4//电磁阀2(污水箱)  未上电时堵死,上电后气泵和污水箱相通
#define	WATER_MOTOR_2				EPWM5//气泵


	


#define CLEAN_ADC_MAX		2100
#define CLEAN_ADC_MIDDLE	2048
#define CLEAN_ADC_MIN		2200
#define	CLEAN_ADC_DIF		50//误差50

#define DIRTY_ADC_MAX		3000
#define DIRTY_ADC_MIN		180
#define DIRTY_ADC_DIF		50//误差50

#define UART_Com_Charger		0xB1//充电/待机
#define UART_Com_AddWater		0xB6//补充清水，底座：打开水泵，检测是否有水
#define UART_Com_DEODORIZE		0xB5//烘干


#define RUNTIME25				250






/**************************************************
*函数名称：void PWM_ModeSelection(uchar mode)
*函数功能：选择工作模式 ro 充电模式
*入口参数：mode： 非0/0 ： 工作模式/充电模式
*出口参数：void
*备   注：无
**************************************************/
#define PWM_ModeChoose(mode)  		\
    do                 				\
    {                  				\
        if(mode == 0)				\
			EPWM_charge_Config();	\
		else						\
			EPWM_working_Config();	\
    } while (0)				
/**************************************************/
#define EPWM_SetChannelSymDuty(ChannelNum,Duty)			\
	do                 									\
    {   												\
			EPWM_ConfigChannelSymDuty(ChannelNum,Duty);	\
    } while (0)
/**************************************************/
	
	

//Charger,Hair,Roller,PumpAIR,PumpWater,SolenoidValveCLEAN,SolenoidValveDIRTY;

	
typedef struct 
{
	unsigned int RunSpeed;//当前运行速度
	unsigned int TargetSpeed;//设置的速度
}RunTypeDef;	
extern RunTypeDef xdata Charger,Hair,Roller,PumpAIR,PumpWater,SolenoidValveCLEAN,SolenoidValveDIRTY;

extern void LED_icon();

void Robot_RunMode();//0,1,2,3,4,5,6,7:充电,待机,拖地,排污,补充清水，自清洁,抽真空2s
void Data_init();	
void RunSpeed_GradualChange();
#endif