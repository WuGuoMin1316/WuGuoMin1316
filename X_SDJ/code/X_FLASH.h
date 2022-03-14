#ifndef _X_FLASH_H_
#define _X_FLASH_H_
#include "cms8s6990.h"


#define Start_Address   0
#define Address_1       Start_Address+1
#define Address_2       Start_Address+2
#define Address_3       Start_Address+3
#define Address_4       Start_Address+4
#define Address_5       Start_Address+5
#define Address_6       Start_Address+6
#define Address_7       Start_Address+7
#define Address_8       Start_Address+8
#define Address_9       Start_Address+9
#define Address_10      Start_Address+10
#define Address_11      Start_Address+11
#define Address_12      Start_Address+12
#define Address_13      Start_Address+13
#define Address_14      Start_Address+14
#define Address_15      Start_Address+15

#define End_Address     Start_Address+30

//
void FLASH_DownData(char en);
void FLASH_UpData(char en);


#endif