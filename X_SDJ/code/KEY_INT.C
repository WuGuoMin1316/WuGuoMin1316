#include "cms8s6990.h"
#include "user_h.h"

#define NUM_SYHS_MODE_LOW 80   //同步信号低电平时间
#define NUM_SYHS_MODE_HIGH 40  //同步信号高电平时间
#define NUM_BIT_0_MODE_LOW 10  //数据0低电平时间
#define NUM_BIT_0_MODE_HIGH 10 //数据0高电平时间
#define NUM_BIT_1_MODE_LOW 5   //数据1低电平时间
#define NUM_BIT_1_MODE_HIGH 5  //数据1高电平时间
#define NUM_STOP_MODE_LOW 20   //停机信号高电平
#define NUM_STOP_MODE_HIGH 10  //停机信号低电平

typedef enum
{
    KEY_INT_MODE_NC,
    KEY_INT_MODE_READ,
    KEY_INT_MODE_WRITE,
};

enum
{
    NUM_SYHS_MODE, //同步头模式
    NUM_BIT_MODE,  //数据模式
    NUM_STOP_MODE, //停止位模式
};

struct
{
    uchar num_mode;
    uchar bit_num;
} read_wirte_mode;

uchar key_data;
uchar key_num[2];
uchar *key_num_add;
static uchar key_int_mode, time_key_int;


/**************************************************
*函数名称：void time0_claen_on(void)
*函数功能：初始化time0和计数器
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void time0_claen_on(void)
{
    time_key_int = 0;
    TMR_ConfigTimerPeriod(TMR0, (0xFFFF - 400) >> 8, 0xFF - 400);
    TMR_Start(TMR0);
}
/**************************************************
*函数名称：uchar key_write_start(uchar num)
*函数功能：启动发送数据到按键板
*入口参数：需要发送的数据
*出口参数：是否成功
*备   注：无
**************************************************/
uchar key_write_start(uchar num)
{
    if (key_int_mode == KEY_INT_MODE_NC)
    {
        key_int_mode = KEY_INT_MODE_WRITE;
        key_num[0] = num;
        key_num[1] = ~num;
        time0_claen_on();
        EXTINT_DisableInt(EXTINT0);
        read_wirte_mode.num_mode = NUM_SYHS_MODE;
        key_num_add = key_num;
        return 1;
    }
    else
    {
        return 0;
    }
}

/**************************************************
*函数名称：void key_write_bit_end(void)
*函数功能：发送完1bit的数据
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void key_write_bit_end(void)
{
    KEY_INT_OUT_LOW();
    time_key_int = 0;
    read_wirte_mode.bit_num++;
    *key_num_add = *key_num_add << 1;
    if ((read_wirte_mode.bit_num | 0xF8) == 0xF8)
    {
        key_num_add++;
    }
}

/**************************************************
*函数名称：void key_write_mode_working(void)
*函数功能：发送按键板数据
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void key_write_mode_working(void)
{
    if (key_int_mode == KEY_INT_MODE_WRITE)
    {
        if (read_wirte_mode.num_mode == NUM_SYHS_MODE)
        {
            if (time_key_int == 0)
            {
                KEY_INT_OUT_LOW();
            }
            else if (time_key_int == NUM_SYHS_MODE_LOW)
            {
                KEY_INT_OUT_HIGH();
            }
            else if (time_key_int == (NUM_SYHS_MODE_HIGH + NUM_SYHS_MODE_LOW))
            {
                KEY_INT_OUT_LOW();
                time_key_int = 0;
                read_wirte_mode.bit_num = 0;
                read_wirte_mode.num_mode = NUM_BIT_MODE;
            }
        }
        else if (read_wirte_mode.num_mode == NUM_BIT_MODE)
        {
            if ((*key_num_add) & 0x80)
            {
                if (time_key_int == NUM_BIT_1_MODE_LOW)
                {
                    KEY_INT_OUT_HIGH();
                }
                else if (time_key_int == (NUM_BIT_1_MODE_HIGH + NUM_BIT_1_MODE_LOW))
                {
                    key_write_bit_end();
                }
            }
            else
            {
                if (time_key_int == NUM_BIT_0_MODE_LOW)
                {
                    KEY_INT_OUT_HIGH();
                }
                else if (time_key_int == (NUM_BIT_0_MODE_HIGH + NUM_BIT_0_MODE_LOW))
                {
                    key_write_bit_end();
                }
            }
            if (read_wirte_mode.bit_num == 16)
            {
                read_wirte_mode.num_mode = NUM_STOP_MODE;
            }
        }
        else
        {
            if (time_key_int == NUM_STOP_MODE_LOW)
            {
                KEY_INT_OUT_HIGH();
            }
            else if (time_key_int == (NUM_STOP_MODE_HIGH + NUM_STOP_MODE_LOW))
            {
                time_key_int = 0;
                key_int_mode = KEY_INT_MODE_NC;
                // EXTINT_ClearIntFlag(EXTINT0);
                // EXTINT_EnableInt(EXTINT0);
                // TMR_Stop(TMR0);
            }
        }
    }
    else if (key_int_mode == KEY_INT_MODE_NC)
    {
        EXTINT_ClearIntFlag(EXTINT0);
        EXTINT_EnableInt(EXTINT0);
        TMR_Stop(TMR0);
    }
    time_key_int++;
}

/**************************************************
*函数名称：void key_read_mode_working(void)
*函数功能：接收按键板数据
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void key_read_mode_working(void)
{
    if (key_int_mode == KEY_INT_MODE_NC)
    {
        key_int_mode = KEY_INT_MODE_READ;
        read_wirte_mode.bit_num = 0;
        time0_claen_on();
    }
    else if (key_int_mode == KEY_INT_MODE_READ)
    {
        if (read_wirte_mode.bit_num == 0)
        {
            key_num_add = key_num;
        }
        *key_num_add = *key_num_add << 1;
        read_wirte_mode.bit_num++;
        if ((time_key_int < (NUM_BIT_0_MODE_HIGH + NUM_BIT_0_MODE_LOW + 3)) && (time_key_int > (NUM_BIT_0_MODE_HIGH + NUM_BIT_0_MODE_LOW - 3)))
        {
            *key_num_add &= 0xFE;
        }
        else if ((time_key_int < (NUM_BIT_1_MODE_HIGH + NUM_BIT_1_MODE_LOW + 3)) && (time_key_int > (NUM_BIT_1_MODE_HIGH + NUM_BIT_1_MODE_LOW - 3)))
        {
            *key_num_add |= 0x01;
        }
        else if ((time_key_int < (NUM_SYHS_MODE_HIGH + NUM_SYHS_MODE_LOW + 10)) && (time_key_int > (NUM_SYHS_MODE_HIGH + NUM_SYHS_MODE_LOW - 10)))
        {
            read_wirte_mode.bit_num = 0;
        }
        else
        {
            key_int_mode = KEY_INT_MODE_NC;
            TMR_Stop(TMR0);
        }
        if (read_wirte_mode.bit_num == 16)
        {
            key_int_mode = KEY_INT_MODE_NC;
            if ((key_num[0] + key_num[1]) == 255)
            {
                key_data = key_num[0];
            }
        }
        if ((read_wirte_mode.bit_num | 0xf8) == 0xf8)
        {
            key_num_add++;
        }
    }
    time_key_int = 0;
			
}
/*
20ms 一次
*/
void key_data_to_led(void)
{
	//static uchar cnt;
   // static uchar y = 1;
	//static uchar x = 2;
    if (key_data & 0x80)
    {
//        y = y << 1;
//        if (y == 0x40)
//        {
//            y = 1;
//        }
//        key_write_start(y);
//        my_printf("x_main_S1\n\r");
		//ERROR_ROLLER = 0;
		
	
			//KEY_Serve(1);

    }
    else if (key_data & 0x40)
    {
//        y = y >> 1;
//        if (y == 0x00)
//        {
//            y = 0x40;
//        }
//        key_write_start(y);
//        my_printf("x_main_S2\n\r");
	
			//KEY_Serve(2);
	
		
    }
    key_data = 0;
	//my_printf("key_data = %d\n\r",(uint)key_data);
}
