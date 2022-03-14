#include "cms8s6990.h"
#include "user_h.h"

ENUM_CHARGE_STATUS charge_status;
ENUM_BAT_ERROR battery_status;
static int battery_voltage_adc, battery_current_adc;

uint32_t charge_time;
uint charge_duty;
uchar charge_pre;

/**************************************************
*函数名称：void charge_start_mode()
*函数功能：启动充电功能
*入口参数：void
*出口参数：void
*备注	 ：
**************************************************/
void charge_start_mode(void)
{
    charge_status = CHARGE_START;
    my_printf("change_on\n\r");
}

/**************************************************
*函数名称：uchar charge_end_mode()
*函数功能：关闭充电功能
*入口参数：void
*出口参数：void
*备注	 ：
**************************************************/
void charge_end_mode(void)
{
    charge_status = CHARGE_OVER;
    charge_duty = 0;
    charge_battery_pwm(charge_duty);
    my_printf("change_off\n\r");
}

/**************************************************
*函数名称：void change_pwm_duty_mid(void)
*函数功能：限定duty幅度
*入口参数: uint x
*出口参数：uint x
*备注	 ：
**************************************************/
uint change_pwm_duty_mid(uint x)
{
    if (x > CHARGEFULLPWM)
    {
        x = CHARGEFULLPWM;
    }
    if (x < 0)
    {
        x = 0;
    }
    return x;
}
/**************************************************
*函数名称：void change_cc_mode(void)
*函数功能：恒流充电
*入口参数：void
*出口参数：void
*备注	 ：
**************************************************/
void change_cc_mode(void)
{
    if (battery_current_adc < CC_CUR_MIN)
    {
        charge_battery_pwm(change_pwm_duty_mid(++charge_duty));
    }
    else if (battery_current_adc > CC_CUR_MAX)
    {
        --charge_duty;
        charge_battery_pwm(change_pwm_duty_mid(charge_duty));
    }
    if (battery_voltage_adc > CVVOL)
    {
        charge_pre++;
        if (charge_pre == 100)
        {
            charge_status = CHARGE_CV;
            my_printf("CHARGE_CV\n\r");
        }
    }
    else
    {
        charge_pre = 0;
    }
}

/**************************************************
*函数名称：void change_cv_mode(void)
*函数功能：恒压充电
*入口参数：void
*出口参数：void
*备注	 ：
**************************************************/
void change_cv_mode(void)
{
    if (battery_voltage_adc > CV_VOL_MAX)
    {
        --charge_duty;
        charge_battery_pwm(change_pwm_duty_mid(charge_duty));
    }
    else if (battery_voltage_adc < CV_VOL_MIN)
    {
        charge_battery_pwm(change_pwm_duty_mid(++charge_duty));
    }
    if (battery_current_adc < CV_SOTP_VUR)
    {
        charge_pre++;
        if (charge_pre == 100)
        {
            charge_status = CHARGE_FULL;
            charge_duty = 0;
            charge_battery_pwm(charge_duty);
            my_printf("CHARGE_END\n\r");
        }
    }
    else
    {
        charge_pre = 0;
    }
}

/**************************************************
*函数名称：void charge_battery_protect(void)
*函数功能：充电保护
*入口参数：void
*出口参数：void
*备注	 ：
**************************************************/
void charge_battery_protect(void)
{
    if (battery_voltage_adc > BAT_OVPV) //超压保护
    {
        charge_status = CHARGE_FULL;
        charge_duty = 0;
        charge_battery_pwm(charge_duty);
        my_printf("bat_ovp\n\r");
    }
    else if (battery_voltage_adc < BAT_SHORTV) //短路保护
    {
        charge_status = CHARGE_OVER;
        charge_duty = 0;
        charge_battery_pwm(charge_duty);
        my_printf("bat_short\n\r");
    }
    if (battery_current_adc > BAT_OCP) //过流保护
    {
        charge_status = CHARGE_OVER;
        charge_duty = 0;
        charge_battery_pwm(charge_duty);
        my_printf("bat_ocp\n\r");
        my_printf("charge_pwm=%d,cur=%d\n\r", (uint)charge_duty, (uint)battery_current_adc);
    }
    else if ((battery_current_adc < POWER_CUT)||(CHANGE_DET_IO() == 0))
    {
        charge_status = CHARGE_OVER;
        charge_duty = 0;
        charge_battery_pwm(charge_duty);
        my_printf("PRECHARGE_current_adc=%d\n\r",(uint)battery_current());
        my_printf("put_cut\n\r");
    }
}

/**************************************************
*函数名称：void change_battery(void)
*函数功能：充电程序
*入口参数：void
*出口参数：void
*备注	 ：20ms调用一次
**************************************************/
void change_battery(void)
{
    charge_time++;
    battery_voltage_adc = battery_voltage();
    battery_current_adc = battery_current();
	
	//测电池电量
	
    if (battery_current_adc<0)
    {
        battery_current_adc=0;
    }
    
    switch (charge_status)
    {
    case CHARGE_START:
//      battery_current_adc_one = battery_current();
        charge_time = 0;
        charge_duty = SMALLCUR;
        charge_battery_pwm(charge_duty);
        charge_status = CHARGE_PRECHARGE;
        my_printf("charge_start\n\r");
        break;
    case CHARGE_PRECHARGE:
    {
        if (charge_time > 500)
        {
            charge_time = 0;

            if (battery_voltage_adc > BAT_OVPV)
            {
                battery_status = BAT_OPEN;
                charge_duty = 0;
                charge_battery_pwm(charge_duty);
                charge_status = CHARGE_ERROR;
                my_printf("BAT_OPEN\n\r");
            }
            else if (battery_voltage_adc < BAT_SHORTV)
            {
                battery_status = BAT_SHORT;
                charge_duty = 0;
                charge_battery_pwm(charge_duty);
                charge_status = CHARGE_ERROR;
                my_printf("BAT_SHORT\n\r");
            }
            else if (battery_voltage_adc < BAT_DCOV)
            {
            }
            else if (battery_voltage_adc < BAT_FCV)
            {
                charge_status = CHARGE_CC;
                my_printf("CHARGE_CC\n\r");
            }
            // else if (battery_voltage_adc < BAT_FCV)
            // {
            //     charge_status = CHARGE_CV;
            //     my_printf("CHARGE_CV\n\r");
            // }
            else
            {
                charge_status = CHARGE_OVER;
                my_printf("CHARGE_OVER\n\r");
            }
        }
    }
    break;
    case CHARGE_CC:
        change_cc_mode();
        charge_battery_protect();
        break;
    case CHARGE_CV:
        change_cv_mode();
        charge_battery_protect();
        if (charge_time > (3600000 * 6 / 20))
        {
            charge_status = CHARGE_OVER;
            charge_duty = 0;
            charge_battery_pwm(charge_duty);
            my_printf("cc_time_out");
        }
        break;
    case CHARGE_FULL:
        if (battery_voltage_adc < BAT_ADD) //重新补电
        {
            charge_status = CHARGE_START;
        }
        break;
    case CHARGE_OVER:
        break;
    default:
        break;
    }
}

/**************************************************
*函数名称：uchar return_State_of_charge(void)
*函数功能：返回是否正在充电
*入口参数：void
*出口参数：uchar 1，正在充电，uchar 0 未充电
*备注	 ：20ms调用一次
**************************************************/
uchar return_State_of_charge(void)
{
    if ((charge_status == CHARGE_PRECHARGE) || (charge_status == CHARGE_CC) || (charge_status == CHARGE_CV))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}