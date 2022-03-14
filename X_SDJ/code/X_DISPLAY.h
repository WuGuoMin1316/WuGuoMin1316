#ifndef  _X_DISPLAY_H_
#define  _X_DISPLAY_H_
#include "cms8s6990.h"

#define	SetBit(x,y)			x |= (1<<y)//set 1
#define ClrBit(x,y)			x &= ~(1<<y)//set 0



extern unsigned char battery_display();
extern void TX_write2buff(unsigned char UARTn,char dat) ;
extern void hex_to_uart(unsigned char UARTn, uchar  * s,uchar i);
unsigned char breath_effect();
void led_Scan_Display();
void TM1652_Light_init(unsigned char Light_duty,unsigned char Light_cur,unsigned char Light_mode);

#endif