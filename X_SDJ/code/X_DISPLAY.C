// File Name: X_DISPLAY
// Author:Ming
// note：使用tm1652 LED驱动芯片
//波特率需要 19200 ;数据位 8  ;校验位 odd  ;停止位 1  ; hex进制发送接收数据;
//使用步骤:1,发送地址指令;2,发送显示数据;等待3ms后;3,发送显示控制指令;4,发送显示控制数据;等待3ms后.
#include "X_DISPLAY.h"
#include "FUNCTION.H"

unsigned char xdata LED_num[35];//
volatile char xdata LED_data[5];//



/**************************************************
*函数名称：void UART_CommandToTM1652()
*函数功能：
*入口参数：
*出口参数：
*备   注：给驱动芯片发送指令数据
* 波特率 19200 ;数据位 8  ;校验位 odd  ;停止位 1  ; hex进制发送接收数据;
* 发送数据包8段5位: 显示的起始地址(0x08)+显示的数据(5byte)+亮度控制的指令(ox18)+亮度控制的数据(ox1e)
* 发送数据包7段6位: 显示的起始地址(0x08)+显示的数据(6byte)+亮度控制的指令(ox18)+亮度控制的数据(ox1f)
* 例如,8段5位:0x08 01 00 00 00 00 + 0x18 1e (若已经设置过亮度,可以不发送0x181e)
**************************************************/
void UART_CommandToTM1652()
{
	TX_write2buff(UART1,0x08);
	hex_to_uart(UART1,LED_data,5);		
}
/*************************************************
function name: void TM1652_Light_init()
function work: 初始化,设置亮度(占空比和驱动电流)
input data   : Light_duty:位驱动占空比(1~5)   ;Light_cur:段驱动电流(1~4);Light_mode 
output data  : null
note：
*************************************************/
void TM1652_Light_init(unsigned char Light_duty,unsigned char Light_cur,unsigned char Light_mode)
{
	unsigned char Light_data;
	switch (Light_duty)
	{
		case 0:Light_data = 0;			
			break;
		case 1:Light_data = 0x80;			
			break;
		case 2:Light_data = 0x40;		
			break;
		case 3:Light_data = 0xc0;
			break;
		case 4:Light_data = 0x20;
			break;
		case 5:Light_data = 0xa0;
			break;
		case 6:Light_data = 0x60;			
			break;
		case 7:Light_data = 0xe0;			
			break;
		case 8:Light_data = 0x10;		
			break;
		case 9:Light_data = 0x90;
			break;
		case 10:Light_data = 0x50;
			break;
		case 11:Light_data = 0xd0;
			break;
		case 12:Light_data = 0x30;		
			break;
		case 13:Light_data = 0xb0;
			break;
		case 14:Light_data = 0x70;
			break;
		case 15:Light_data = 0xf0;
			break;
		default:Light_data = 0xf0;
    		break;
	}
	switch (Light_cur)
	{
		case 0:Light_data += 0;
			break;
		case 1:Light_data += 0x08;
			break;
		case 2:Light_data += 0x0c;
			break;
		case 3:Light_data += 0x0a;
			break;
		case 4:Light_data += 0x0e;
			break;
		default:Light_data += 0x0e;
    		break;
	}
	TX_write2buff(UART1,0x18);//显示控制命令
	TX_write2buff(UART1,(Light_data + Light_mode));//  1/2占空比,1/2驱动电流,8段5位
}
/*************************************************
function name: void close_Display(void)
function work: 
input data   : null
output data  : null
note：
*************************************************/
void close_Display(void)	//关闭显示
{		
	TM1652_Light_init(0,0,0);
}

/**************************************************
*函数名称：void Scan_Display(uchar en)
*函数功能：//LED显示
*入口参数：en:使能
*出口参数：
*备   注：有数据更新就执行一次
**************************************************/
void Scan_Display()	//扫描显示
{	
	unsigned char c_i = 0;
	unsigned char c_j = 0;
	for( c_i = 0;c_i < 5;c_i++)
	{
		for( c_j = 0 ;c_j < 7;c_j++)
        {
			if(LED_num[(7*c_i + c_j)] == 1)
			{
				SetBit(LED_data[c_i],c_j);
			}
			else
			{
				ClrBit(LED_data[c_i],c_j);
			}
        }			
	}
}
/*************************************************
function name: void LED_icon(unsigned char ico,unsigned en)
function work: 
input data   : ico 选择图标; en  是否点亮
output data  : 
note：
*************************************************/
void LED_icon()
{
	LED_num[3] = Ico_Battery_Flag;
	LED_num[4] = Ico_Battery_Flag;
	LED_num[5] = Ico_Battery_Flag;
	LED_num[6] = Ico_Battery_Flag;
	
	LED_num[7] = Ico_Standard_Flag;
	LED_num[8] = Ico_Standard_Flag;
	LED_num[9] = Ico_Standard_Flag;

	LED_num[14] = Ico_Dry_Mopping_Flag;
	LED_num[15] = Ico_Dry_Mopping_Flag;
	LED_num[16] = Ico_Dry_Mopping_Flag;

	LED_num[21] = Ico_Wet_Mopping_Flag;
	LED_num[22] = Ico_Wet_Mopping_Flag;
	LED_num[23] = Ico_Wet_Mopping_Flag;

	LED_num[28] = Ico_Deodorize_Flag;
	LED_num[29] = Ico_Deodorize_Flag;
	LED_num[30] = Ico_Deodorize_Flag;

	LED_num[10] = Ico_Self_Cleaning_Flag;
	LED_num[11] = Ico_Self_Cleaning_Flag;
	LED_num[12] = Ico_Self_Cleaning_Flag;

	LED_num[17] = Ico_Clean_Water_Flag;
	LED_num[18] = Ico_Clean_Water_Flag;
	LED_num[19] = Ico_Clean_Water_Flag;
	
	LED_num[24] = Ico_Dirty_Water_Flag;
	LED_num[25] = Ico_Dirty_Water_Flag;
	LED_num[26] = Ico_Dirty_Water_Flag;
	
	LED_num[31] = Ico_Brush_Flag;
	LED_num[32] = Ico_Brush_Flag;
	LED_num[33] = Ico_Brush_Flag;
	
	LED_num[20] = Ico_Logo_Flag;
	LED_num[27] = Ico_Logo_Flag;
	LED_num[34] = Ico_Logo_Flag;
	LED_num[13] = Ico_Logo_Flag;

}
/*************************************************
function name: unsigned char breath_effect()
function work: 呼吸效果
input data   : null
output data  : null
note：
烘干功能下进入亮度调节--呼吸效果
*************************************************/
unsigned char breath_effect()
{
	static unsigned char breath_i,breath_j = 15;
	static unsigned char breath_count;
	if(Ico_Deodorize_Flag)//烘干功能时候需要亮度调节
	{	
		if((++breath_count > 2)&&(Robot_On_Flag))
		{
			breath_count = 0;		
			if(++breath_j > 29)
				breath_j = 1;
			if(breath_j > 15)
				 breath_i = (30 - breath_j);
			else
				 breath_i = breath_j;			
		}	
	}
	else
	{
		breath_j = 15;
		breath_i = breath_j;
	}	
	return breath_i;
}	

/**************************************************
*函数名称：led_Scan_Display()
*函数功能：连接接口函数
*入口参数：
*出口参数：
*备   注：在main.c文件中调用 10ms
**************************************************/
void led_Scan_Display()
{
	static unsigned char scan_count,scan_count2;
	
	//获取电池电量
	if(battery_display())
	{
		if(++scan_count2 > 50)//500ms执行一次,做低电量的闪烁提醒
		{
			scan_count2 = 0;
			//Ico_Battery_Flag = ~Ico_Battery_Flag;
			Ico_Battery_Flag = 1;
			LED_num[0] = Ico_Battery_Flag;
		}
	}
	else
	{
		Ico_Battery_Flag = 1;
	}
	if(++scan_count > 15)
	{
		scan_count = 0;	
		LED_icon();		
		Scan_Display();
		UART_CommandToTM1652();
	}

}
