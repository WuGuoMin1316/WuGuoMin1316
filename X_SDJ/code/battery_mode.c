#include "cms8s6990.h"
#include "xxdduq.h"
#include "user_h.h"


uint Battery_impedance;
uint code battery_electricity_meter[] = {
    2986,
    3142,
    3186,
    3232,
    3243,
    3266,
    3285,
    3301,
    3319,
    3337,
    3358,
    3383,
    3415,
    3455,
    3498,
    3544,
    3595,
    3645,
    3700,
    3762,
    3813,
};



/**************************************************
*函数名称：void return_battery_vol_percentage(void)
*函数功能：返回电池电量百分比
*入口参数：void
*出口参数：uchar 电量百分比
*备   注：无
**************************************************/
uchar return_battery_vol_percentage(void)
{
    uint vol;
    uchar per;
    vol = battery_voltage();
    for (per = 0; per < 20; per++)
    {
        if (vol < battery_electricity_meter[per])
        {
            break;
        }
    }
    per = per * 5;
    return per;
}

/**************************************************
*函数名称：return_battery_current_adc(void)
*函数功能：返回校正后电池电流
*入口参数：void
*出口参数：uint
*备注	 ：
**************************************************/
uint return_battery_current_adc(void)
{
	if (battery_current() > current_parameter.current_correction)
	{
		return (battery_current() - current_parameter.current_correction);
	}
	else
	{
		return 0;
	}
}

/**************************************************
*函数名称：void Correct_battery_current_in(void)
*函数功能：检测电池电流校正参数
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void Correct_battery_current_in(void)
{
	current_parameter.in_current_correction = battery_current();
}