#ifndef __CHARGE_H__
#define __CHARGE_H__
typedef enum
{
    CHARGE_START,
    CHARGE_PRECHARGE, //预充电
    CHARGE_CC,        //constant current
    CHARGE_CV,        //constant voltage
    CHARGE_FULL,
    CHARGE_OC, //过充
    CHARGE_ERROR,
    CHARGE_OVER, //充电未满而结束
} ENUM_CHARGE_STATUS;

typedef enum
{
    BAT_OK = 0,
    BAT_OPEN,  //电池开路
    BAT_SHORT, //电池短路
} ENUM_BAT_ERROR;

/* Exported constants --------------------------------------------------------*/
#define CHARGEFULLPWM 450
#define SMALLCUR 200 //预充电PWM

#define BAT_FCV 3372 //充满电压
#define BAT_OVP 3392 //过冲电压
#define BAT_OVPV 3453 //过冲保护电压
#define BAT_DCOV 2409 //过放电压
#define BAT_SHORTV 401 //短路电压
#define BAT_ADD 3292 //补电电压
#define STARTCVVOL 3356 //未充电进入CV充电电压
#define CVVOL 3372 //CC充电进入CV充电电压
#define CV_VOL_MIN 3332 //CV模式最小充电电压
#define CV_VOL_MAX 3388 //CV模式最大充电电压
#define CC_CUR_MIN 1502 //标准最小充电电流
#define CC_CUR_MAX 1775 //标准最大充电电流
#define CV_SOTP_VUR 273 //充电截止电流
#define BAT_OCP 2184 //过流保护电流
#define POWER_CUT 137 //检查电源是否断开电流

#define battery_current() adc_avg_data(BAT_CURR_ADC)
#define battery_voltage() adc_avg_data(BAT_VOL_ADC)

#define charge_battery_pwm(x) EPWM_ConfigChannelSymDuty(EPWM2, x)
//数组battery_electricity_meter[21] 用于计算电量
// const unsigned short  battery_electricity_meter[21] = {
//2610,
//2746,
//2784,
//2824,
//2834,
//2854,
//2871,
//2885,
//2901,
//2917,
//2935,
//2957,
//2985,
//3019,
//3057,
//3097,
//3142,
//3186,
//3234,
//3288,
//3332,
//};



extern void charge_start_mode(void);
extern void charge_end_mode(void);
extern void change_battery(void);
extern uchar return_State_of_charge(void);

#endif