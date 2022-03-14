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
	(1)���ö�ʱ����ʱ��
	*/
	SYS_ConfigWUTCLK(WUT_CLK_DIV_256);   //Fwut = LSI(125Khz)/32  
	/*
	(2)���ö�ʱ����ʱ��
	*/
	SYS_ConfigWUTTime(49);				//256us * 4000 = 1.024s
	/*
	(3)������ʱ����
	*/
	SYS_EnableWakeUpTrig();	
}
/**************************************************
*�������ƣ�unsigned char get_dc_in_state(void)
*�������ܣ���ȡ����Ƿ�������״̬,���ܿ����Ƿ���
*��ڲ�����void
*���ڲ�����0/1  0������;1������
*��   ע����
**************************************************/
bit get_dc_in_state(void)
{
	if(power_working_mode.dc_in_mode == DC_IN_ON)
		return 1;
	else
		return 0;
}

/**************************************************
*�������ƣ�void DC_IN_MODE_CHANGEING(void)
*�������ܣ�����ⲿ��Դ�Ƿ����
*��ڲ�����void
*���ڲ�����void
*��   ע����
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
		if (CHANGE_DET_IO() == 0)//���ⲿ��Դ����
		{
			if (num == 5)
			{
				power_working_mode.dc_in_mode = DC_IN_OFF;
				PGA_ConfigInput(PGA_DIF_H_PGAGND_L_PGAP);
                EPWM_working_Config();
				charge_end_mode();
				my_printf("DC_IN_OFF\n\r");
				RobotInBaseFlag = 0;
				RunMode = RUNMODE_STANDBY;//�������ģʽ
			}
		}
		else
		{
			num = 0;
		}
	}
	else
	{
		if (CHANGE_DET_IO())//���ⲿ��Դ����
		{
			if (num == 5)
			{
				my_printf("DC_IN_ON\n\r");
				PGA_ConfigInput(PGA_DIF_H_PGAP_L_PGAGND);
			}
			else if (num == 10)
			{
				power_working_mode.dc_in_mode = DC_IN_ON;//������ģʽ
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
*�������ƣ�unsigned char return_battery_vol_percentage(void)
*�������ܣ����ص�ص����ٷֱ�
*��ڲ�����void
*���ڲ�����uchar   �����ٷֱ� 0~100
*��   ע����
**************************************************/
unsigned char return_battery_vol_percentage()
{
	unsigned char per_i;
    unsigned short adc_vol;
	unsigned short battery_electricity_meter[21] = 	//��ض�GND�ĵ�ѹ  
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
 *�������ƣ�void Calculate_and_display_power(void)
 *�������ܣ����µ����ʾ����,���ص�ص����ٷֱ�
 *��ڲ�����void
 *���ڲ��������ص�ص����ٷֱ�  0~100
 *��ע	 ��
 **************************************************/
unsigned char Calculate_and_display_power(void)
{
	static unsigned char xdata battery_percentage;//�����ٷֱ�(0~100)
	unsigned char battery_vol_DATA;
	/* ���ж��Ƿ��ڳ��*/
	battery_vol_DATA = return_battery_vol_percentage();
	if (battery_percentage == 0)
	{
		battery_percentage = battery_vol_DATA;
	}
	else
	{	
		if (battery_percentage < battery_vol_DATA)
		{
			if (power_working_mode.dc_in_mode == DC_IN_ON)//���״̬
			{
				battery_percentage++;//���ʱ,������������
			}
		}
		else if (battery_percentage > battery_vol_DATA)
		{
			if (power_working_mode.dc_in_mode == DC_IN_OFF)
			{
				battery_percentage--;//�����ʱ,�������ܼ�С.
			}
		}
	}
	
	return battery_percentage;
}
/*************************************************
function name: void battery_display()
function work: ������ʾ
input data   : null
output data  : null
note��
*************************************************/
unsigned char battery_display()
{
	unsigned char power_display_data;
	power_display_data = Calculate_and_display_power();
	power_display_data = 90;
	if(power_display_data <= 10)//��������10% �͵�����Ҫ��˸����
	{
		return 1;
	}
	else 
	{	
		LED_num[0] = 1;
		LED_num[1] = 1;
		LED_num[2] = 1;
		if(power_display_data <= 35)//��������35%  ������ʾ1��
		{
			LED_num[1] = 0;
			LED_num[2] = 0;
		}
		else if(power_display_data <= 70)//��������70%  ������ʾ2��
		{
			LED_num[2] = 0;
		}
	}
	return 0;
}
/*************************************************
function name: void sys_sweep_time()
function work: ��¼ɨ��ʱ��
input data   : null
output data  : null
note��
��¼�ϵ�ʱ���Ƿ�>30��,��>30��,�������ϵ������ж��Ƿ���������
��׼��/����/��ǿʪ���������ϵ�ģʽ�Ž��м�ʱ
����������ʱ���,       
RUNMODE_STANDARD,//��׼�ϵ�ģʽ
	RUNMODE_DRY_MOPPING,//����
	RUNMODE_WET_MOPPING,//��ǿʪ��

20msһ��
*************************************************/
void sys_sweep_time()
{
	static unsigned char sweep_count;
	if((RunMode == RUNMODE_STANDARD)||(RunMode == RUNMODE_DRY_MOPPING)||(RunMode == RUNMODE_WET_MOPPING))//��׼��/����/��ǿʪ��
	{
		if(sweep_count < 250)	sweep_count++;
	}
	if((sweep_count >= 250)&&(RobotInBaseFlag == 1))////�����ϵ�ʱ�����50s,���ڵ�����
	{
		Self_Cleaning_Flag = 1;
	}
//	if(Self_Cleaning_Flag )//��������
//	{
//		Self_Cleaning_Flag = 0;
//	}
}

extern unsigned char return_adc_wroking(void);
extern void ACMP1_Config(unsigned char VREF_K);
/*************************************************
function name: unsigned char task4_sys_stop(void)
function work: ����4
input data   : null
output data  : null
note��
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
			WaitX(200U);	//��ʱ,�ȴ�����ָ������
			
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
				if ((CHANGE_DET_IO())||(KEY_VALUE))//����:ֻ�ܳ�������
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
		if ((CHANGE_DET_IO() == 0)&&(KEY_VALUE == 0)&&(RunMode == 1))//�ڴ���ģʽ��,û���ⲿ��Դ����,��,����δ����
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
