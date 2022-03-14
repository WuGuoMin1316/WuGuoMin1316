#include "cms8s6990.h"
#include "user_h.h"
#include "FUNCTION.h"

/**************************************************
*函数名称：void duge_uart_test(void)
*函数功能：串口调试命令
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void duge_uart_test(void)
{
    uchar uart_data;
    uchar uart_code[4];
    if (uart0_read_fifo_num() >= 4)
    {
        uart0_read_data(uart_code, 1);	
        if (uart_code[0] == '@') 
        {		
            uart0_read_data(uart_code + 1, 3);
            uart_data = (uart_code[1]-0x30)*100+(uart_code[2]-0x30)*10+uart_code[3]-0x30;
            switch (uart_data)
            {
            case 101:
                printf("battery_vol=%dmV\n\r", (uint)((adc_avg_data(BAT_VOL_ADC) * 4982L) / 1000));
                break;
            case 102:
                printf("battery_cur=%dmA\n\r", (uint)((battery_current()*366L)/1000));
                break;
            default:
                break;
            }
        }
        else if(uart_code[0] == 0xAA)//0xaa data1 data2 ef
        {
			uart0_read_data(uart_code + 1, 3);
			if(uart_code[3] == 0xef)
			{
				//hex_to_uart(uart_code,4);
				 switch (uart_code[1])
				{
					case 0xDE://0xaa de 01/02 ef//模拟按下按键,K1,K2
						//KEY_Serve(uart_code[2]);
						break;
					case 0xBE://设置or读 机器是否在底座上
						if(uart_code[2]<2)	RobotInBaseFlag = uart_code[2];
						my_printf("RobotInBaseFlag = %d\n\r",(uint)RobotInBaseFlag);
						break;
					case 0xCC://设置or读 清水箱状态
						if(uart_code[2]<2)	CleanCistern_Min = uart_code[2];
						my_printf("CleanCistern_Min = %d\n\r",(uint)CleanCistern_Min);
						break;
					case 0xDC://设置or读 污水箱状态
						if(uart_code[2]<2)	DirtyCistern_Max = uart_code[2];
						my_printf("DirtyCistern_Max = %d\n\r",(uint)DirtyCistern_Max);
						break;
					default:
						break;
				}
			}	
        }
    }
}
