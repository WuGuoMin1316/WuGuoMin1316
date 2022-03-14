#include "user_h.h"
#include "cms8s6990.h"
#include "xxdduq.h"


#define RX_Length 20
#define TX_Length 100

typedef struct
{
	unsigned char TX_read;  //发送读指针
	unsigned char TX_write; //发送写指针
	unsigned char RX_read;  //接收读指针
	unsigned char RX_write; //接收写指针
	unsigned char TX_Busy;		//忙标志
    unsigned char RX_Buffer[RX_Length];//接收缓冲
	unsigned char TX_Buffer[TX_Length];//发送缓冲
} COMx_Define;
COMx_Define xdata COM0,COM1;

/* 从data中获取第n bit的值 注：data只能为uchar类型 */
#define GET_BIT_N_VAL(entry_data, n)  (0x1 & (entry_data>>n))


/*************** 奇偶校验 *******************************/
/*************************************************
function name: unsigned char TX_parity_crc(char odd_data,unsigned char entry_len)
function work: 奇偶校验
input data   : odd_data 数据,entry_len
output data  : 0/1;奇数为0 偶数为1
note：获取odd_data的二进制数中的1的个数,并判断奇偶
*************************************************/
unsigned char TX_parity_crc(char odd_data,unsigned char entry_len)
{
	unsigned char  dat_i;
	unsigned char  odd_parity = 0;

    for(dat_i = 0; dat_i < entry_len; dat_i++)                  
    {                                                                              
        odd_parity += GET_BIT_N_VAL(odd_data, dat_i);                 
    }

    if (odd_parity % 2 == 0)
        return 1;
    else
        return 0;
}
/*************** 装载串口发送缓冲 *******************************/
void TX_write2buff(unsigned char UARTn,char dat) //写入发送缓冲，指针+1
{
	if(UART0 == UARTn)
	{
		if (COM0.TX_Busy == 0) //空闲
		{
			COM0.TX_Busy = 1; //标志忙
			// TI0 = 1;			  //触发发送中断
			//UART_SendNinthBit(UART0, TX_parity_crc(dat,8));//奇偶校验
			SBUF = dat;
		}
		else
		{
			while ((COM0.TX_write + 1) % TX_Length == COM0.TX_read)
			;							 //如果缓存满就等待发送一个字节
			COM0.TX_Buffer[COM0.TX_write] = dat; //装发送缓冲
			COM0.TX_write = (++COM0.TX_write) % TX_Length;
		}
	}
	if(UART1 == UARTn)
	{
		if (COM1.TX_Busy == 0) //空闲
		{
			COM1.TX_Busy = 1; //标志忙
			// TI0 = 1;			  //触发发送中断
			UART_SendNinthBit(UART1, TX_parity_crc(dat,8));//奇偶校验
			SBUF1 = dat;
		}
		else
		{
			while ((COM1.TX_write + 1) % TX_Length == COM1.TX_read)
			;							 //如果缓存满就等待发送一个字节
			COM1.TX_Buffer[COM1.TX_write] = dat; //装发送缓冲
			COM1.TX_write = (++COM1.TX_write) % TX_Length;
		}
	}
}

/**************************************************
*函数名称：char uart0_read_fifo_num(void)
*函数功能：获取读缓存中的数据数量
*入口参数：viod
*出口参数：fifo数据数量
*备注	 ：
**************************************************/
uchar uart0_read_fifo_num(void)
{
    return (COM0.RX_write - COM0.RX_read + RX_Length) % RX_Length;
}

/**************************************************
*函数名称：char uart0_read_data(uchar *s, uchar j)
*函数功能：获取读缓存中的数据
*入口参数：uchar *s 缓存指针，j取数据个数
*出口参数：uchar i,取出数据个数
*备注	 ：
**************************************************/
uchar uart0_read_data(uchar *s, uchar j)
{
    uchar i = 0;
    while (COM0.RX_read != COM0.RX_write)
    {
        *s = COM0.RX_Buffer[COM0.RX_read];
        COM0.RX_read = (++COM0.RX_read) % RX_Length;
        s++;
        i++;
        if (i == j)
        {
            break;
        }
    }
    return i;
}

//重写putchar函数
char putchar(char num)
{
	TX_write2buff(UART0,num);
	return num;
}

/**************************************************
*函数名称：void hex_to_uart( unsigned char UARTn, uchar  * s,uchar i)
*函数功能：发送数据流到串口
*入口参数：*s，数组指针，i，数据个数
*出口参数：void
*备注	 ：在AD中断中调用
**************************************************/
void hex_to_uart(unsigned char UARTn, uchar  * s,uchar i)
{
	while(i)
    {
        i--;
		if(UART0 == UARTn)
			putchar(*s);
		if(UART1 == UARTn)
			TX_write2buff(UARTn,*s);
        s++;
    }
}

/**************************************************
*函数名称：void UART0_IRQHandler(void) interrupt UART0_VECTOR
*函数功能：串口中断函数
*入口参数：void
*出口参数：void
*备注	 ：在AD中断中调用
**************************************************/
void UART0_IRQHandler(void) interrupt UART0_VECTOR
{
    if (RI0)
    {
        RI0 = 0;
        COM0.RX_Buffer[COM0.RX_write] = SBUF;
        if (++COM0.RX_write >= RX_Length)
            COM0.RX_write = 0;
        set_ready(0);
    }
    if (TI0)
    {
        TI0 = 0;
        if (COM0.TX_read != COM0.TX_write)
        {		
			//UART_SendNinthBit(UART0, TX_parity_crc(COM0.TX_Buffer[COM0.TX_read],8));//奇偶校验
            SBUF = COM0.TX_Buffer[COM0.TX_read];
            COM0.TX_read = (++COM0.TX_read) % TX_Length;
        }
        else
            COM0.TX_Busy = 0;
    }
}
/**************************************************
*函数名称：void UART1_IRQHandler(void) interrupt UART1_VECTOR
*函数功能：串口中断函数
*入口参数：void
*出口参数：void
*备注	 ：
**************************************************/
void UART1_IRQHandler(void)  interrupt UART1_VECTOR 
{
    if (UART_GetSendIntFlag(UART1))//TI0
    {
        UART_ClearSendIntFlag(UART1);
        if (COM1.TX_read != COM1.TX_write)
        {		
			UART_SendNinthBit(UART1, TX_parity_crc(COM1.TX_Buffer[COM1.TX_read],8));//奇偶校验
            SBUF1 = COM1.TX_Buffer[COM1.TX_read];
            COM1.TX_read = (++COM1.TX_read) % TX_Length;
        }
        else
            COM1.TX_Busy = 0;
    }
}
