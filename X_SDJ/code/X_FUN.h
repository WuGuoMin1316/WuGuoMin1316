#ifndef  _X_FUN_H_
#define  _X_FUN_H_
#include "cms8s6990.h"
#include "xxdduq.h"
#include "printf_uart.h"
#include "charge.h"
#include "X_ADC.h"
#include "RUNMODE_ENUM.h"

#define CHANGE_DET_IO() P25//外部电源是否插入


extern unsigned char  KEY_VALUE;
extern unsigned char RunMode;
extern unsigned char xdata LED_num[35];
extern void Get_key_value();
extern void EPWM_working_Config(void);
extern void charge_end_mode(void);
extern void EPWM_charge_Config(void);
extern void charge_start_mode(void);
extern void WUT_Config(void);
void sys_sweep_time();
unsigned char task4_sys_stop(void);
void DC_IN_MODE_CHANGEING(void);

#endif