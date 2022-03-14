#include "X_KEY.h"
#include "FUNCTION.H"


extern unsigned char RunMode;
extern void TX_write2buff(unsigned char UARTn,char dat);
extern void close_Display(void);
 /****************************
按键,机体
****************************/
//长按标志位

unsigned char  KEY_VALUE;

unsigned char Get_number_most(unsigned char *a,unsigned char num)
{
	unsigned char Get_n_i,Get_n_j,num_data[4] = {0,0,0,0},OutNum = 0;
	for(Get_n_i = 0;Get_n_i < num;Get_n_i++)
    {
		if(a[Get_n_i] == 1)	num_data[0]++;
		if(a[Get_n_i] == 2)	num_data[1]++;
		if(a[Get_n_i] == 3)	num_data[2]++;
		if(a[Get_n_i] == 4)	num_data[3]++;
    }
	for(Get_n_j = 0;Get_n_j < 4;Get_n_j++)
    {
		if(num_data[Get_n_j] >= 4)
			OutNum = Get_n_j+1;
	}
	return OutNum;
}
/*************************************************
function name: unsigned char Get_key_value()
function work: 3个实体按键值
input data   : null
output data  : null
note：
k1 :  	0.5*vdd; 
k2 :	0.36*vdd;
k3 :	0.24*vdd;
*************************************************/
void Get_key_value()
{	
	unsigned char Get_key_i,Get_key_temp = 0;

	for(Get_key_i = ACMP_VREF_17;Get_key_i > 0;Get_key_i--)
	{
		ACMP_ConfigNegative(ACMP1, ACMP_NEGSEL_VREF_VDD,Get_key_i);//VREF_K
		if(ACMP_GetResult(ACMP1)) //正端电压 > 参考电压,输出1,否则输出0;
		{
			if(Get_key_i == ACMP_VREF_17)//正端电压 > vdd*17/20
				Get_key_temp = 0;//松开按键		
		}
		else
		{
			if(Get_key_i == ACMP_VREF_14)
				Get_key_temp++;
			if(Get_key_i == ACMP_VREF_9)
				Get_key_temp++;
			if(Get_key_i == ACMP_VREF_5)
				Get_key_temp++;
			if(Get_key_i == ACMP_VREF_1)
				Get_key_temp++;
		}
	}
	KEY_VALUE = Get_key_temp;
}
/***********************************************
函数名称：key_flag_clear
函数功能：清空按键标志位
入口参数：1：清空按键长按标志位/0：清空按键单点标志位
出口参数：无
备注：
************************************************/
void key_flag_clear(unsigned char a)
{
	if(a)
	{
		KEY_LONG_K1_Flag = 0;
		KEY_LONG_K2_Flag = 0;
		KEY_LONG_K3_Flag = 0;
	}
	else
	{
		KEY_CLICK_K1_Flag = 0;
		KEY_CLICK_K2_Flag = 0;
		KEY_CLICK_K3_Flag = 0;
	}
}
/*************************************************
function name: void KEY()
function work: 3个实体按键的长按和短按
input data   : null
output data  : null
note：10MS执行一次
*************************************************/
void KEY_Search()
{
	#define DoubleClick_Time 	15
	#define OneClick_Time 		4
	#define LongClick_Time 		80	//1.5 秒
	
	unsigned char  KEY_temp1;
	static unsigned char xdata KEY_TIME;			
	static unsigned char xdata KEY_VALUE_Last[7];	
	static unsigned char  KEY_temp;
	
	Get_key_value();
	
	if (KEY_TIME < 0xf0)	KEY_TIME++;
	if (KEY_TIME == LongClick_Time) //长按键检测
	{
		key_flag_clear(1);
		key_flag_clear(0);
		switch (KEY_VALUE)
        {
        	case KEY1:KEY_LONG_K1_Flag = 1;break;        		
        	case KEY2:KEY_LONG_K2_Flag = 1;break;        		
			case KEY3:KEY_LONG_K3_Flag = 1;break;  
			case KEY4:KEY_LONG_K4_Flag = 1;break; 			
        }
	}
	if(KEY_VALUE == 0)		//按键释放，或者切换了别的按键后复位
	{
		if ((KEY_TIME < LongClick_Time) && (KEY_TIME > OneClick_Time))
		{
			key_flag_clear(1);
			key_flag_clear(0);
			
			//短按检测
			switch(KEY_temp)//按键释放后，与上一个值为；
			{
				case KEY1:KEY_CLICK_K1_Flag = 1;break;				
				case KEY2:KEY_CLICK_K2_Flag = 1;break;
				case KEY3:KEY_CLICK_K3_Flag = 1;break;	
				case KEY4:KEY_CLICK_K4_Flag = 1;break;				
			}
		}
		if (KEY_LONG_K1_Flag)	KEY_LONG_RELEASE_K1_Flag=1;	
		if (KEY_LONG_K2_Flag)	KEY_LONG_RELEASE_K2_Flag=1;	
		if (KEY_LONG_K3_Flag)	KEY_LONG_RELEASE_K3_Flag=1;	
		if (KEY_LONG_K4_Flag)	KEY_LONG_RELEASE_K4_Flag=1;			
		KEY_TIME = 0;
	}

	KEY_VALUE_Last[0] = KEY_VALUE_Last[1];
	KEY_VALUE_Last[1] = KEY_VALUE_Last[2];
	KEY_VALUE_Last[2] = KEY_VALUE_Last[3];
	KEY_VALUE_Last[3] = KEY_VALUE_Last[4];
	KEY_VALUE_Last[4] = KEY_VALUE;
	
	KEY_temp1 = Get_number_most(KEY_VALUE_Last,5);
	if(KEY_temp1 > 0)
		KEY_temp = KEY_temp1;
}
/**************************************************
*函数名称：void KEY_change_RunMode(unsigned char num)
*函数功能：按键改变运行模式
*入口参数：num 对应的按键
*出口参数：void
*备   注：
**************************************************/

void KEY_change_RunMode(unsigned char num)
{	
	static unsigned char Clean_mode = 0;
	if (num == KEY1)//一键开关机
    {	
		RunMode += RUNMODE_STANDBY;
		if(RunMode > RUNMODE_STANDARD)	RunMode = RUNMODE_STANDBY;
		
		if(RunMode == RUNMODE_STANDBY)//待机模式,即关机
		{
			Clean_mode = 0;
			Robot_On_Flag = 0;	
			LED_DISPLAY_OFF = 0;
			Ico_Standard_Flag = 1;
			//Ico_Deodorize_Flag = 0;
			Ico_Dry_Mopping_Flag = 0;
			Ico_Wet_Mopping_Flag = 0;			
			close_Display();
			my_printf("Standby (off):");				
		}
		else//开机
		{			
			Robot_On_Flag = 1;
			LED_DISPLAY_OFF = 1;
			Ico_Dry_Mopping_Flag = 0;
			Ico_Wet_Mopping_Flag = 0;
			my_printf("Standard (on):");				
		}
		
	}
	else if (num == KEY2)//自清洁
    {
		//设置自清洁标志位,等机器到底座上后才执行自清洁程序
		if(Robot_On_Flag)
		{
			Ico_Self_Cleaning_Flag = ~Ico_Self_Cleaning_Flag;
		}
	}
	else if (num == KEY3)//标准拖/干拖/加强湿拖三种模式切换
	{
		if(Robot_On_Flag)
		{
			if(++Clean_mode > 2)
				Clean_mode = 0;
			RunMode = Clean_mode + RUNMODE_STANDARD;
			Ico_Standard_Flag = 0;
			Ico_Dry_Mopping_Flag = 0;
			Ico_Wet_Mopping_Flag = 0;
			if(RUNMODE_STANDARD == RunMode)//标准扫地
			{
				Ico_Standard_Flag = 1;
				my_printf("Standard:");	
			}
			else if(RUNMODE_DRY_MOPPING == RunMode)//干拖
			{
				Ico_Dry_Mopping_Flag = 1;
				my_printf("dry mopping:");
			}
			else if(RUNMODE_WET_MOPPING == RunMode)//湿拖
			{
				Ico_Wet_Mopping_Flag = 1;
				my_printf("wet mopping:");
			}
		}	
	}
	my_printf("RunMode=%d\n\r",(uint)RunMode);	
}
/*************************************************
function name: void KEY_LONG_Handler()
function work: 按键长按服务函数
input data   : null
output data  : null
note：
	所有按键均为AD检测
*************************************************/
void KEY_LONG_Handler()//
{
	static bit LONG_K1_Cnt = 0;//长按的次数
	static bit LONG_K2_Cnt = 0;//长按的次数
	static bit LONG_K3_Cnt = 0;//长按的次数
	
	/* ************** 长按K1 ************** *///长按功能
	if((KEY_LONG_K1_Flag)&&(!LONG_K1_Cnt))
	{
		KEY_change_RunMode(KEY1);
		LONG_K1_Cnt = 1;	
	}
	if(KEY_LONG_RELEASE_K1_Flag)
	{
		LONG_K1_Cnt = 0;
		KEY_LONG_K1_Flag = 0;
		KEY_LONG_RELEASE_K1_Flag = 0;
	}
	/* ************** 长按K2 ************** *///长按功能
	if((KEY_LONG_K2_Flag)&&(!LONG_K2_Cnt))
	{
		KEY_change_RunMode(KEY2);
		if(Robot_On_Flag)
			Ico_Deodorize_Flag = ~Ico_Deodorize_Flag;
		LONG_K2_Cnt = 1;	
	}
	if(KEY_LONG_RELEASE_K2_Flag)
	{
		LONG_K2_Cnt = 0;
		KEY_LONG_K2_Flag = 0;
		KEY_LONG_RELEASE_K2_Flag = 0;
	}
	/* ************** 长按K3 ************** *///长按功能
	if((KEY_LONG_K3_Flag)&&(!LONG_K3_Cnt))
	{
		KEY_change_RunMode(KEY3);
		LONG_K3_Cnt = 1;
	}	
	if(KEY_LONG_RELEASE_K3_Flag)
	{
		LONG_K3_Cnt = 0;
		KEY_LONG_K3_Flag = 0;
		KEY_LONG_RELEASE_K3_Flag = 0;
	}
	
}
/*************************************************
function name: void KEY_CLICK_Handler()
function work: 按键短按服务函数
input data   : null
output data  : null
note：
*************************************************/
void KEY_CLICK_Handler()//
{
	if(KEY_CLICK_K1_Flag)
	{
		KEY_change_RunMode(KEY1);
		KEY_CLICK_K1_Flag = 0;
	}
	if(KEY_CLICK_K2_Flag)
	{
		KEY_change_RunMode(KEY2);	
		KEY_CLICK_K2_Flag = 0;
	}
	if(KEY_CLICK_K3_Flag)
	{
		KEY_change_RunMode(KEY3);	
		KEY_CLICK_K3_Flag = 0;
	}
	if(KEY_CLICK_K4_Flag)
	{
		if(Robot_On_Flag)
			Ico_Deodorize_Flag = ~Ico_Deodorize_Flag;
		KEY_CLICK_K4_Flag = 0;
	}
}
/*************************************************
function name: void KEY_Serve()
function work: 按键服务函数
input data   : null
output data  : null
note：
*************************************************/
void KEY_Serve()
{
	KEY_Search();
	KEY_LONG_Handler();
	KEY_CLICK_Handler();
}
