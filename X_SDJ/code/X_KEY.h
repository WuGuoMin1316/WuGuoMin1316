#ifndef  _X_KEY_H_
#define  _X_KEY_H_
#include "cms8s6990.h"
#include "KEY_ENUM.h"
#include "RUNMODE_ENUM.h"
#include "printf_uart.h"

typedef enum
{
	ACMP_VREF_0,
    ACMP_VREF_1,//
    ACMP_VREF_2,//
    ACMP_VREF_3,//
	ACMP_VREF_4,//
	ACMP_VREF_5,//
	ACMP_VREF_6,//
	ACMP_VREF_7,//
	ACMP_VREF_8,//
	ACMP_VREF_9,//
	ACMP_VREF_10,//
	ACMP_VREF_11,//
	ACMP_VREF_12,//
	ACMP_VREF_13,//
	ACMP_VREF_14,//
	ACMP_VREF_15,//
	ACMP_VREF_16,//
	ACMP_VREF_17,
	ACMP_VREF_18
};

void KEY_Serve();
#endif
