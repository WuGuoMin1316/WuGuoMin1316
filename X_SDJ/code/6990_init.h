#ifndef __6990_INIT_H__
#define __6990_INIT_H__

#define STERILIZE_ON() P00=1
#define STERILIZE_OFF() P00=0
#define POWER_DET_ON() P14=0
#define POWER_DET_OFF() P14=1
#define KEY_INT_OUT_HIGH() P04=1
#define KEY_INT_OUT_LOW() P04=0



extern void x_6990_all_init(void);
extern void EPWM_working_Config(void);
extern void EPWM_charge_Config(void);
extern void ACMP1_Config(unsigned char VREF_K);

#endif
