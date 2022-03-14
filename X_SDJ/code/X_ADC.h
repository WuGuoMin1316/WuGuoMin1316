#ifndef _X_ADC_H_
#define _X_ADC_H_
#include "cms8s6990.h"
enum
{	
	BRUSH_ADC,
	CLEAN_WATER_ADC,  
	DIRTY_WATER_ADC,
	BAT_VOL_ADC,
	BAT_CURR_ADC	
};

#define get_roller_adc() adc_avg_data(BRUSH_ADC)
#define get_clean_adc() adc_avg_data(CLEAN_WATER_ADC)
#define get_dirty_adc() adc_avg_data(DIRTY_WATER_ADC)

extern void ADC_int_interrupt(void);
extern uint adc_avg_data(uchar CH);
extern void pfintf_adc(void);

#endif