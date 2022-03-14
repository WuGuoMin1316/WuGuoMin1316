#include "X_Fun.h"
#include "FUNCTION.H"


typedef enum
{
	DC_IN_ON,
	DC_IN_OFF,
} DC_IN_MODE_TypeDef;

typedef enum
{
	DC_OUT_ON,
	DC_OUT_OFF,
} DC_OUT_MODE_TypeDef;

struct
{
	DC_IN_MODE_TypeDef dc_in_mode;
	DC_OUT_MODE_TypeDef dc_out_mode;
}power_working_mode;

Union_Flag Ico,Key,Sys_Flag,Other_Flag,Open_Flag;

void WUT_Config(void)
{
	/*
	(1)设置定时唤醒时钟
	*/
	SYS_ConfigWUTCLK(WUT_CLK_DIV_256);   //Fwut = LSI(125Khz)/32  
	/*
	(2)设置定时唤醒时间
	*/
	SYS_ConfigWUTTime(49);				//256us * 4000 = 1.024s
	/*
	(3)开启定时唤醒
	*/
	SYS_EnableWakeUpTrig();	
}
/**************************************************
*函数名称：unsigned char get_dc_in_state(void)
*函数功能：获取电池是否在输入状态,即总开关是否按下
*入口参数：void
*出口参数：0/1  0无输入;1有输入
*备   注：无
**************************************************/
bit get_dc_in_state(void)
{
	if(power_working_mode.dc_in_mode == DC_IN_ON)
		return 1;
	else
		return 0;
}

/**************************************************
*函数名称：void DC_IN_MODE_CHANGEING(void)
*函数功能：检测外部电源是否插入
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void DC_IN_MODE_CHANGEING(void)
{
	static uchar num;
	if (num < 255)
	{
		num++;
	}
	if (power_working_mode.dc_in_mode == DC_IN_ON)
	{
		if (CHANGE_DET_IO() == 0)//无外部电源输入
		{
			if (num == 5)
			{
				power_working_mode.dc_in_mode = DC_IN_OFF;
				PGA_ConfigInput(PGA_DIF_H_PGAGND_L_PGAP);
                EPWM_working_Config();
				charge_end_mode();
				my_printf("DC_IN_OFF\n\r");
				RobotInBaseFlag = 0;
				RunMode = RUNMODE_STANDBY;//进入待机模式
			}
		}
		else
		{
			num = 0;
		}
	}
	else
	{
		if (CHANGE_DET_IO())//有外部电源输入
		{
			if (num == 5)
			{
				my_printf("DC_IN_ON\n\r");
				PGA_ConfigInput(PGA_DIF_H_PGAP_L_PGAGND);
			}
			else if (num == 10)
			{
				power_working_mode.dc_in_mode = DC_IN_ON;//进入充电模式
                EPWM_charge_Config();
				charge_start_mode();
				RobotInBaseFlag = 1;
				RunMode = RUNMODE_CHARGER;
			}
		}
		else
		{
			num = 0;
		}
	}
}


/**************************************************
*函数名称：unsigned char return_battery_vol_percentage(void)
*函数功能：返回电池电量百分比
*入口参数：void
*出口参数：uchar   电量百分比 0~100
*备   注：无
**************************************************/
unsigned char return_battery_vol_percentage()
{
	unsigned char per_i;
    unsigned short adc_vol;
	unsigned short battery_electricity_meter[21] = 	//电池对GND的电压  
	{
		2610,
		2746,
		2784,
		2824,
		2834,
		2854,
		2871,
		2885,
		2901,
		2917,
		2935,
		2957,
		2985,
		3019,
		3057,
		3097,
		3142,
		3186,
		3234,
		3288,
		3332
	};

    adc_vol = battery_voltage();
	for (per_i = 0; per_i < 20; per_i++)
    {
        if (adc_vol < battery_electricity_meter[per_i])
        {	
            break;
        }
    }
    per_i = per_i * 5;
    return per_i;
}
/**************************************************
 *函数名称：void Calculate_and_display_power(void)
 *函数功能：更新电池显示电量,返回电池电量百分比
 *入口参数：void
 *出口参数：返回电池电量百分比  0~100
 *备注	 ：
 **************************************************/
unsigned char Calculate_and_display_power(void)
{
	static unsigned char xdata battery_percentage;//电量百分比(0~100)
	unsigned char battery_vol_DATA;
	/* 能判断是否在充电*/
	battery_vol_DATA = return_battery_vol_percentage();
	if (battery_percentage == 0)
	{
		battery_percentage = battery_vol_DATA;
	}
	else
	{	
		if (battery_percentage < battery_vol_DATA)
		{
			if (power_working_mode.dc_in_mode == DC_IN_ON)//充电状态
			{
				battery_percentage++;//充电时,电量才能增加
			}
		}
		else if (battery_percentage > battery_vol_DATA)
		{
			if (power_working_mode.dc_in_mode == DC_IN_OFF)
			{
				battery_percentage--;//不充电时,电量才能减小.
			}
		}
	}
	
	return battery_percentage;
}
/*************************************************
function name: void battery_display()
function work: 电量显示
input data   : null
output data  : null
note：
*************************************************/
unsigned char battery_display()
{
	unsigned char power_display_data;
	power_display_data = Calculate_and_display_power();
	power_display_data = 90;
	if(power_display_data <= 10)//电量低于10% 低电量需要闪烁提醒
	{
		return 1;
	}
	else 
	{	
		LED_num[0] = 1;
		LED_num[1] = 1;
		LED_num[2] = 1;
		if(power_display_data <= 35)//电量低于35%  电量显示1格
		{
			LED_num[1] = 0;
			LED_num[2] = 0;
		}
		else if(power_display_data <= 70)//电量低于70%  电量显示2格
		{
			LED_num[2] = 0;
		}
	}
	return 0;
}
/*************************************************
function name: void sys_sweep_time()
function work: 记录扫地时间
input data   : null
output data  : null
note：
记录拖地时间是否>30秒,若>30秒,机器放上底座后判断是否进入自清洁
标准拖/干拖/加强湿拖这三种拖地模式才进行记时
放入底座后计时清空,       
RUNMODE_STANDARD,//标准拖地模式
	RUNMODE_DRY_MOPPING,//干拖
	RUNMODE_WET_MOPPING,//加强湿拖

20ms一次
*************************************************/
void sys_sweep_time()
{
	static unsigned char sweep_count;
	if((RunMode == RUNMODE_STANDARD)||(RunMode == RUNMODE_DRY_MOPPING)||(RunMode == RUNMODE_WET_MOPPING))//标准拖/干拖/加强湿拖
	{
		if(sweep_count < 250)	sweep_count++;
	}
	if((sweep_count >= 250)&&(RobotInBaseFlag == 1))////机器拖地时间大于50s,且在底座上
	{
		Self_Cleaning_Flag = 1;
	}
//	if(Self_Cleaning_Flag )//自清洁完成
//	{
//		Self_Cleaning_Flag = 0;
//	}
}

extern unsigned char return_adc_wroking(void);
extern void ACMP1_Config(unsigned char VREF_K);
/*************************************************
function name: unsigned char task4_sys_stop(void)
function work: 任务4
input data   : null
output data  : null
note：
*************************************************/
unsigned char task4_sys_stop(void)
{
    static bit stop_mode;
	static unsigned char count_sys;
	 _SS 
    while (1)
    {
		WaitX(200U);
		
		if(count_sys >= 20 )
		{			
			count_sys = 0;
			stop_mode = 1;
			P14 = 1;
			my_printf("go to sleep\n\r");
			WaitX(200U);
			WaitX(200U);	//延时,等待串口指令发送完毕
			
			set_end(0);
			set_end(1);	
			set_end(2);				
			ADC_Stop();
			PGA_Stop();
		
			WUT_Config();		
			while (stop_mode)
			{
				//ACMP_Stop(ACMP1);
				SYS_EnterStop();
				//ACMP_Start(ACMP1);
				//ACMP1_Config(0);				
				Get_key_value();				
				if ((CHANGE_DET_IO())||(KEY_VALUE))//问题:只能长按唤醒
				{
					stop_mode = 0;	
					LED_DISPLAY_OFF = 0;					
					if(CHANGE_DET_IO())
						Ico_Standard_Flag = 0;							
				}			
			}		
			SYS_DisableWakeUpTrig();
			my_printf("get up \n\r");
			PGA_Start();
			ADC_Start();
			set_ready(0);
			set_ready(1);
			set_ready(2);
			P14 = 0;
			
		}
		if ((CHANGE_DET_IO() == 0)&&(KEY_VALUE == 0)&&(RunMode == 1))//在待机模式下,没有外部电源输入,且,按键未按下
		{				
			if(count_sys < 254)	count_sys++;
		}
		else
		{
			count_sys = 0;
		}
	 }
    _EE
}
