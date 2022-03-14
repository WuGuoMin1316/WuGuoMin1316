#include "cms8s6990.h"


#define ADC_avg_num 3 //平均次数
#define ADC_CH_MAX sizeof(ADC_CH_NUM) / sizeof(uchar)//数组中包含的元素个数

uchar code ADC_CH_NUM[] = {ADC_CH_1, ADC_CH_6,ADC_CH_9,ADC_CH_10,ADC_CH_31}; //定义采样通道
typedef struct
{
	u16 adc_ave;			  //平均值
	u16 adc_num[ADC_avg_num]; //记录采集数
} ADC_NUM;

ADC_NUM xdata ADC_NUM_SHUJU[ADC_CH_MAX]; //定义AD采样后的临时数据和平均数据

static uchar i, j;

/**************************************************
*函数名称：void ADC_int_interrupt(void)
*函数功能：采集各通道的数据，每次采集存入对应数组，达到指定次数后求平均
*入口参数：void
*出口参数：void
*备注	 ：在AD中断中调用
**************************************************/
void ADC_int_interrupt(void)
{
	
	uchar a, b;
	ADC_NUM_SHUJU[i].adc_num[j] = ADC_GetADCResult();
	i = (++i) % ADC_CH_MAX;
	ADC_EnableChannel(ADC_CH_NUM[i]);
	if (i)
	{
		ADC_GO();
	}
	else
	{
		j = (++j) % ADC_avg_num;
		if (j == 0)
		{
			for (a = 0; a < ADC_CH_MAX; a++)
			{
				ADC_NUM_SHUJU[a].adc_ave = 0;
				for (b = 0; b < ADC_avg_num; b++)
				{
					ADC_NUM_SHUJU[a].adc_ave += ADC_NUM_SHUJU[a].adc_num[b];
				}
				ADC_NUM_SHUJU[a].adc_ave /= ADC_avg_num;
			}
		}
	}
}

/**************************************************
*函数名称：uint adc_avg_data(uchar CH)
*函数功能：返回需求通道平均后的AD值
*入口参数：AD通道
*出口参数：平均后的值
*备注	 ：
**************************************************/
uint adc_avg_data(uchar CH)
{
	return ADC_NUM_SHUJU[CH].adc_ave;
}
/*************************************************
function name: return_adc_wroking(void)
function work: 
input data   : null
output data  : null
note：
*************************************************/

unsigned char return_adc_wroking(void)
{
	if(i|j|ADC_IS_BUSY)
	{
		return 1;
	}
	return 0;
}
// void pfintf_adc(void)
// {
// 	printf("adc=%u\n",ADC_NUM_SHUJU[0].adc_ave);
// }