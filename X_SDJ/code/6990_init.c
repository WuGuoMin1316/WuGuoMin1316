#include "cms8s6990.h"
#include "user_h.h"


extern unsigned int WORKING_PWMPeriod_MAX;
/**************************************************
*函数名称：void ADC_Config(void)
*函数功能：ADC初始化
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void ADC_Config(void)
{
	//设置ADC的运行模式
	ADC_ConfigRunMode(ADC_CLK_DIV_8, ADC_RESULT_LEFT); //设置ADC时钟为系统时钟的256分频，ADC结果为左对齐
	//设置ADC转换通道
	ADC_EnableChannel(ADC_CH_0);
	GPIO_SET_MUX_MODE(P00CFG, GPIO_P00_MUX_AN0);
	GPIO_SET_MUX_MODE(P01CFG, GPIO_P01_MUX_AN1);
	GPIO_SET_MUX_MODE(P13CFG, GPIO_P13_MUX_AN6);
	GPIO_SET_MUX_MODE(P36CFG, GPIO_P36_MUX_AN17);
	//设置ADC LDO
	ADC_EnableLDO();
	ADC_ConfigADCVref(ADC_VREF_2P4V); //ADC_VREF_1P2V, ADC_VREF_2V, ADC_VREF_2P4V, ADC_VREF_3V

	ADC_ConfigAN31(ADC_CH_31_PGA_ANA);

	//设置ADC中断
	ADC_EnableInt();
	IRQ_SET_PRIORITY(IRQ_ADC, IRQ_PRIORITY_HIGH);
	//开启ADC
	ADC_Start();
}

void TMR0_Config(void)
{

	/*
	(1)设置Timer的运行模式
	*/
	TMR_ConfigRunMode(TMR0, TMR_MODE_TIMING,TMR_TIM_16BIT);	
	/*
	(2)设置Timer 运行时钟
	*/
	TMR_ConfigTimerClk(TMR0, TMR_CLK_DIV_12);						/*Fsys = 48Mhz，Ftimer = 4Mhz,Ttmr=0.25us*/
	/*
	(3)设置Timer周期
	*/	
	TMR_ConfigTimerPeriod(TMR0, (0xFFFF-400)>>8, 0xFF-400); 				// (0xFFFF-400)*0.25us = 100us,递增计数
		
	/*
	(4)开启中断
	*/
	TMR_EnableOverflowInt(TMR0);

	/*
	(5)设置Timer中断优先级
	*/	
	IRQ_SET_PRIORITY(IRQ_TMR0,IRQ_PRIORITY_LOW);


	/*
	(6)开启Timer
	*/
	// TMR_Start(TMR0);
}


/**************************************************
*函数名称：void TMR1_Config(void)
*函数功能：定时器2初始化，用于系统滴答
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void TMR2_Config(void)
{
	/*
	(1)设置Timer的运行模式
	*/
	TMR2_ConfigRunMode(TMR2_MODE_TIMING, TMR2_AUTO_LOAD);
	/*
	(2)设置Timer 运行时钟
	*/
	TMR2_ConfigTimerClk(TMR2_CLK_DIV_12); /*Fsys = 48Mhz，Ftimer = 4Mhz, Ttmr=0.25us*/
	/*
	(3)设置Timer定时
	*/
	TMR2_ConfigTimerPeriod((65536 - 4000)); /* Ttimer * 4000 = 1000us, 向上计数溢出*/
	/*
	(4)开启中断
	*/
	TMR2_EnableOverflowInt();
	/*
	(5)设置Timer中断优先级
	*/
	IRQ_SET_PRIORITY(IRQ_TMR2, IRQ_PRIORITY_LOW);

	TMR2_AllIntEnable();
	IRQ_ALL_ENABLE();
	/*
	(6)开启Timer
	*/
	TMR2_Start();
}


/**************************************************
*函数名称：void EXTINT_Config(void)
*函数功能：外部中断输入初始化，用于接收按键板编码
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void EXTINT_Config(void)
{
	/*
	(1)设置EXTINT功能
	*/
	
	EXTINT_ConfigInt(EXTINT0, EXTINT_TRIG_FALLING);		//INT0 下降沿触发中断
	/*
	(2)设置EXTINT IO口
	*/	

	GPIO_ENABLE_OUTPUT(P3TRIS,GPIO_PIN_0);
	GPIO_ENABLE_OD(P3OD,GPIO_PIN_0); //P30开漏输出
	GPIO_ENABLE_UP(P3UP,GPIO_PIN_0);//上拉开启
	P3DS |= (1<<0); //一直读取引脚状态
	KEY_INT_OUT_HIGH();


	GPIO_SET_PS_MODE(PS_INT0, GPIO_P30);			//复用为INT0 输入功能
	/*
	(3)设置EXTINT中断
	*/		
	EXTINT_EnableInt(EXTINT0);
	IRQ_SET_PRIORITY(IRQ_EXINT0, IRQ_PRIORITY_HIGH);
	
}

/**************************************************
*函数名称：void GPIO_Config(void)
*函数功能：GPIO输出口初始化
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void GPIO_Config(void)
{
	
	GPIO_ENABLE_OUTPUT(P1TRIS, GPIO_PIN_4);//MCU_POWER_DET_EN
	GPIO_ENABLE_OUTPUT(P0TRIS, GPIO_PIN_0);//消毒模块
	GPIO_ENABLE_OUTPUT(P2TRIS, GPIO_PIN_6);//消毒模块
	GPIO_SET_MUX_MODE(P26CFG, GPIO_MUX_GPIO);
	GPIO_SET_MUX_MODE(P14CFG, GPIO_MUX_GPIO);		
	GPIO_SET_MUX_MODE(P00CFG, GPIO_MUX_GPIO);
	//外部电源输入检测  P25
	GPIO_SET_MUX_MODE(P25CFG, GPIO_MUX_GPIO);
	GPIO_ENABLE_INPUT(P2TRIS, GPIO_PIN_5);
	GPIO_ENABLE_RD(P2RD, GPIO_PIN_5);
	GPIO_SET_INT_MODE(P25EICFG, GPIO_INT_BOTH_EDGE);
	GPIO_EnableInt(GPIO2, GPIO_PIN_5_MSK);
	IRQ_SET_PRIORITY(IRQ_P2, IRQ_PRIORITY_LOW);
	P26 = 0;
	POWER_DET_OFF();
	STERILIZE_OFF();
}

/**************************************************
*函数名称：void EPWM_change_Config(void)
*函数功能：充电模式PWM初始化
*入口参数：void
*出口参数：void
*备   注：无//频率：480:100khz//1900:25khz
**************************************************/
void EPWM_charge_Config(void)
{
	WORKING_PWMPeriod_MAX = 480;
	// EPWM_Stop(EPWM_CH_0_MSK);
	//设置EPWM运行模式
	// EPWM_ConfigRunMode(EPWM_WFG_COMPLEMENTARY | EPWM_OC_INDEPENDENT | EPWM_OCU_SYMMETRIC | EPWM_COUNT_DOWN); //互补模式+输出独立模式+对称计数模式+向下计数(边沿对齐)模式
	EPWM_ConfigRunMode(EPWM_WFG_INDEPENDENT|EPWM_OC_INDEPENDENT|EPWM_OCU_SYMMETRIC|EPWM_COUNT_DOWN); //独立模式+输出独立模式+对称计数模式+向下计数(边沿对齐)模式
	//设置EPWM运行时钟
	EPWM_ConfigChannelClk(EPWM0, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM1, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM2, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM3, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM4, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM5, EPWM_CLK_DIV_1);
	//设置EPWM 周期以及占空比
	EPWM_ConfigChannelPeriod(EPWM0, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM1, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM2, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM3, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM4, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM5, WORKING_PWMPeriod_MAX);
	//占空比    PWMD0H +  PWMD0L  +  PWMLOADEN
	EPWM_ConfigChannelSymDuty(EPWM0, 0);
	EPWM_ConfigChannelSymDuty(EPWM1, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelSymDuty(EPWM2, 0);
	EPWM_ConfigChannelSymDuty(EPWM3, 0);
	EPWM_ConfigChannelSymDuty(EPWM4, 0);
	EPWM_ConfigChannelSymDuty(EPWM5, 0);

	//设置EPWM的加载方式为自动加载
	 EPWM_EnableAutoLoadMode(EPWM_CH_5_MSK | EPWM_CH_4_MSK | EPWM_CH_3_MSK | EPWM_CH_2_MSK |EPWM_CH_1_MSK |  EPWM_CH_0_MSK);
	// //设置EPWM的输出极性
	 EPWM_DisableReverseOutput(EPWM_CH_0_MSK |EPWM_CH_1_MSK|EPWM_CH_2_MSK | EPWM_CH_3_MSK | EPWM_CH_4_MSK | EPWM_CH_5_MSK); //关闭反向输出
	// //设置EPWM输出
	 EPWM_EnableOutput(EPWM_CH_0_MSK | EPWM_CH_1_MSK | EPWM_CH_2_MSK | EPWM_CH_3_MSK | EPWM_CH_4_MSK | EPWM_CH_5_MSK); //开启输出
	// //设置EPWM死区
	EPWM_DisableDeadZone(EPWM0);
	EPWM_DisableDeadZone(EPWM1);
	EPWM_DisableDeadZone(EPWM2);
	EPWM_DisableDeadZone(EPWM3);
	EPWM_DisableDeadZone(EPWM4);
	EPWM_DisableDeadZone(EPWM5);

	//设置EPWM中断
	// EPWM_EnableZeroInt(EPWM_CH_0_MSK);				//开启EPWM零点中断
	// EPWM_AllIntEnable();							//开启EPWM总中断
	// IRQ_SET_PRIORITY(IRQ_PWM,IRQ_PRIORITY_HIGH);
	// IRQ_ALL_ENABLE();								//开启总中断

	//设置EPWM输出IO复用
	
	GPIO_SET_MUX_MODE(P16CFG, GPIO_MUX_PG0);//WATER_MOTOR_1 水泵
	GPIO_SET_MUX_MODE(P17CFG, GPIO_MUX_PG1);//BRUSH_PWM_1 ��ˢ	滚刷
	GPIO_SET_MUX_MODE(P05CFG, GPIO_MUX_PG2);//���
	GPIO_SET_MUX_MODE(P22CFG, GPIO_MUX_PG3);//电磁阀2
	GPIO_SET_MUX_MODE(P30CFG, GPIO_MUX_PG4);//电磁阀1
	GPIO_SET_MUX_MODE(P15CFG, GPIO_MUX_PG5);//WATER_MOTOR_2  气泵
	//开启EPWM
	 EPWM_Start(EPWM_CH_5_MSK | EPWM_CH_4_MSK | EPWM_CH_3_MSK | EPWM_CH_2_MSK |EPWM_CH_1_MSK |  EPWM_CH_0_MSK);
}

/**************************************************
*函数名称：void EPWM_working_Config(void)
*函数功能：拖地模式PWM初始化
*入口参数：void
*出口参数：void
*备   注：无//频率：480:103khz//1900:25khz
**************************************************/
void EPWM_working_Config(void)
{
	WORKING_PWMPeriod_MAX = 1900;
	//设置EPWM运行模式
	//EPWM_ConfigRunMode(EPWM_WFG_COMPLEMENTARY | EPWM_OC_INDEPENDENT | EPWM_OCU_SYMMETRIC | EPWM_COUNT_DOWN); //互补模式+输出独立模式+对称计数模式+向下计数(边沿对齐)模式
	EPWM_ConfigRunMode(EPWM_WFG_INDEPENDENT|EPWM_OC_INDEPENDENT|EPWM_OCU_SYMMETRIC|EPWM_COUNT_DOWN);//独立模式+输出独立模式+对称计数模式+向下计数(边沿对齐)模式
	//设置EPWM运行时钟
	EPWM_ConfigChannelClk(EPWM0, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM1, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM2, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM3, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM4, EPWM_CLK_DIV_1);
	EPWM_ConfigChannelClk(EPWM5, EPWM_CLK_DIV_1);
	//设置EPWM 周期以及占空比
	EPWM_ConfigChannelPeriod(EPWM0, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM1, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM2, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM3, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM4, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelPeriod(EPWM5, WORKING_PWMPeriod_MAX);


	EPWM_ConfigChannelSymDuty(EPWM0, 0);
	EPWM_ConfigChannelSymDuty(EPWM1, WORKING_PWMPeriod_MAX);
	EPWM_ConfigChannelSymDuty(EPWM2, 0);
	EPWM_ConfigChannelSymDuty(EPWM3, 0);
	EPWM_ConfigChannelSymDuty(EPWM4, 0);
	EPWM_ConfigChannelSymDuty(EPWM5, 0);

	//设置EPWM的加载方式为自动加载
	EPWM_EnableAutoLoadMode(EPWM_CH_5_MSK | EPWM_CH_4_MSK | EPWM_CH_3_MSK | EPWM_CH_2_MSK |EPWM_CH_1_MSK |  EPWM_CH_0_MSK);
	//设置EPWM的输出极性
	//关闭反向输出
	EPWM_DisableReverseOutput(EPWM_CH_0_MSK |EPWM_CH_1_MSK|EPWM_CH_2_MSK | EPWM_CH_3_MSK | EPWM_CH_4_MSK | EPWM_CH_5_MSK); 
	//打开反向输出
	//EPWM_EnableReverseOutput( EPWM_CH_1_MSK ); //滚刷需要方向输出
	//设置EPWM输出
	EPWM_EnableOutput(EPWM_CH_0_MSK | EPWM_CH_1_MSK | EPWM_CH_2_MSK | EPWM_CH_3_MSK | EPWM_CH_4_MSK | EPWM_CH_5_MSK);
	//设置EPWM死区
	EPWM_DisableDeadZone(EPWM0);
	EPWM_DisableDeadZone(EPWM1);
	EPWM_DisableDeadZone(EPWM2);
	EPWM_DisableDeadZone(EPWM3);
	EPWM_DisableDeadZone(EPWM4);
	EPWM_DisableDeadZone(EPWM5);
	//设置EPWM中断
	// EPWM_EnableZeroInt(EPWM_CH_0_MSK);				//开启EPWM零点中断
	// EPWM_AllIntEnable();							//开启EPWM总中断
	// IRQ_SET_PRIORITY(IRQ_PWM,IRQ_PRIORITY_HIGH);
	// IRQ_ALL_ENABLE();								//开启总中断

	//设置EPWM输出IO复用
	GPIO_SET_MUX_MODE(P16CFG, GPIO_MUX_PG0);//WATER_MOTOR_1 水泵
	GPIO_SET_MUX_MODE(P17CFG, GPIO_MUX_PG1);//BRUSH_PWM_1 ��ˢ	滚刷
	GPIO_SET_MUX_MODE(P02CFG, GPIO_MUX_PG2);//BRUSH_PWM_2  Сëˢ	小毛刷
	GPIO_SET_MUX_MODE(P22CFG, GPIO_MUX_PG3);//电磁阀2
	GPIO_SET_MUX_MODE(P30CFG, GPIO_MUX_PG4);//电磁阀1
	GPIO_SET_MUX_MODE(P15CFG, GPIO_MUX_PG5);//WATER_MOTOR_2  气泵
	//开启EPWM
	EPWM_Start(EPWM_CH_5_MSK | EPWM_CH_4_MSK | EPWM_CH_3_MSK | EPWM_CH_2_MSK |EPWM_CH_1_MSK |  EPWM_CH_0_MSK);
}

/**************************************************
*函数名称：void UART0_Config(void)
*函数功能：串口0 初始化
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void UART0_Config(void)
{
	//使用BRT作为UART模块的波特率时钟发生器
	#ifdef USE_FORMULA
	 uint16_t  BRTValue = 0;
	 uint32_t  BaudRateVlue = 9600;
	#endif 
	 /*
	 (1)设置UARTx的运行模式
	 */
	 UART_ConfigRunMode(UART0,UART_MOD_ASY_8BIT, UART_BAUD_BRT);
	 UART_EnableReceive(UART0);
	 /*
	 (2)配置UARTx的波特率
	 */
	 UART_ConfigBRTClk(BRT_CLK_DIV_1);	
	 UART_EnableDoubleFrequency(UART0); 							/*波特率使能倍频：SMOD =1*/
	 
  #ifdef USE_FORMULA			//使用公式计算定时器的加载值(需要对Systemclock赋值(main.c))，USE_FORMULA 在 选项Option->C51->Preporcessor Symbols->Define中定义
	 BRTValue = UART_ConfigBaudRate(UART0, BaudRateVlue) ;
	 UART_ConfigBRTPeriod(BRTValue);						/*配置重装值*/
  #else 
	UART_ConfigBRTPeriod(65224); 				///*配置重装值,使用数据手册上推荐的加载值(BRT章节)  模式4中Fsys = 48Mhz 对应的值
  #endif
  
	 UART_EnableBRT();										/*使能定时器*/
	 /*
	 (3)配置IO口
	 */
	 GPIO_SET_MUX_MODE(P35CFG,GPIO_MUX_TXD0);			/*TXD0*/
	 GPIO_SET_MUX_MODE(P21CFG,GPIO_MUX_RXD0);	 		/*RXD0*/
	/*
	(4)设置UART中断
	 */
	 UART_EnableInt(UART0);
	 IRQ_SET_PRIORITY(IRQ_UART0,IRQ_PRIORITY_LOW);

}
/**************************************************
*函数名称：void UART1_Config(void)
*函数功能：串口1 初始化
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void UART1_Config(void)
{
	 uint8_t  THxValue = 0;
	 uint32_t  BaudRateVlue = 19200;	 
	/*
	 (1)设置UARTx的运行模式
	 */
	 UART_ConfigRunMode(UART1,UART_MOD_ASY_9BIT, UART_BAUD_TMR4);
	 UART_EnableReceive(UART1);
	/*
	 (2)配置UARTx的波特率
	 */
	 TMR_ConfigTimerClk(TMR4,TMR_CLK_DIV_4);							/*TM =1*/
	 TMR_ConfigRunMode(TMR4, TMR_MODE_TIMING, TMR_TIM_AUTO_8BIT); 		/*设置为8位重装模式*/
	
	 UART_EnableDoubleFrequency(UART1); 							/*波特率使能倍频：SMOD =1*/
	
	 THxValue = 217; //使用数据手册上推荐的加载值(UART章节)	模式2中Fsys = 48Mhz 对应的值

	 TMR_ConfigTimerPeriod(TMR4, THxValue, THxValue);			/*配置重装值*/
	 TMR_Start(TMR4);											/*使能定时器*/
	 /*
	 (3)配置IO口
	 */
	 GPIO_SET_MUX_MODE(P36CFG,GPIO_MUX_TXD1);			/*TXD1*/
	 /*
	(4)设置UART中断
	 */
	 UART_EnableInt(UART1);
	 IRQ_SET_PRIORITY(IRQ_UART1,IRQ_PRIORITY_LOW);
}

/**************************************************
*函数名称：void PGA_Config(void)
*函数功能：初始化PGA
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void PGA_Config(void)
{
	uint8_t AdjValue;
	/*
	(1)开启PGA修调模式，获取修调值
	*/
	AdjValue = PGA_GetOffsetAdjValue(PGA_ADJ_MODE0);	
	/*
	(2)设置PGA运行模式
	*/
	PGA_EnableNormalMode(PGA_NORM_SHAM_DIFFERENCE);		//差分模式
	/*
	(3)设置PGA的信号输入
	*/
	PGA_ConfigInput(PGA_DIF_H_PGAP_L_PGAGND);	
	/*
	(4)设置PGA的放大增益
	*/
	PGA_ConfigGain(PAG_GAIN_32);
	/*
	(5)设置PGA的采样保持
	*/	
	PGA_DisableSHA();
	/*
	(6)设置PGA的调节值
	*/		
	PGA_ConfigOffsetAdj(PGA_OFFSET_PGAADJ, AdjValue);
	/*
	(7)设置PGA的输出
	*/		
	// PGA_EnableOutput();
	/*
	(8)设置PGA的IO口
	*/		
	GPIO_SET_MUX_MODE(P31CFG,GPIO_P31_MUX_PGAP);
	GPIO_SET_MUX_MODE(P32CFG,GPIO_P32_MUX_PGAGND);	
	// GPIO_SET_MUX_MODE(P26CFG,GPIO_P26_MUX_PGAO);	
	/*
	(9)开启PGA
	*/	
	PGA_Start();
}
/*************************************************
function name: void ACMP1_Config()
function work: 模拟比较器初始化
input data   : null
output data  : null
note：用于按键检测(3个按键),休眠唤醒比较省电
//P04---C1P0
正端电压 > 负端电压 比较器输出 1 ,否则输出 0 ;
当比较器 **输出值** 发生改变时，每路比较器都可产生中断。
实际应用: 正端:按键电压;负端:1.2V*系数K;
休眠后系数K为1;
未按下:正端 < 1.2V
按下后:正端 > 1.2V,出发中断唤醒,改变系数K,判断是哪个按键按下.判断结束后,系数K为1;
*************************************************/

void ACMP1_Config(unsigned char VREF_K)
{
#define LL  0
	uint8_t AdjValue;
	/*
	(1)获取ACMP模块修调值
	*/
	AdjValue = ACMP_GetOffsetAdjValue(ACMP1,ACMP_ADJ_GND,LL,LL);
	/*
	(2)设置ACMP正端输入
	*/
	ACMP_ConfigPositive(ACMP1, ACMP_POSSEL_P0);
	/*
	(3)设置ACMP负端输入
	*/
	ACMP_ConfigNegative(ACMP1, ACMP_NEGSEL_VREF_VDD,VREF_K); //vdd*17/20
	/*
	(4)设置输出相位
	*/
	ACMP_DisableReverseOutput(ACMP1);		
	/*
	(5)设置输出滤波
	*/
	ACMP_EnableFilter(ACMP1,ACMP_NFCLK_3_TSYS);	
	/*
	(6)设置输出迟滞
	*/	
	ACMP_EnableHYS(ACMP1,ACMP_HYS_SEL_P,ACMP_HYS_10);
	/*
	(7)设置调节
	*/			
	ACMP_ConfigOffsetAdj(ACMP1,ACMP_OFFSET_ACMPDJ,AdjValue);
	/*
	(8)设置中断
	*/		
	ACMP_EnableInt(ACMP1);
	IRQ_SET_PRIORITY(IRQ_ACMP, IRQ_PRIORITY_LOW);		//设置中断优先级
	IRQ_ALL_ENABLE();									//开启总中断	
	/*
	(11)设置ACMP IO
	*/		
	GPIO_SET_MUX_MODE(P04CFG, GPIO_P04_MUX_C1P0);
//	GPIO_SET_MUX_MODE(P05CFG, GPIO_P05_MUX_C1N);
//	GPIO_SET_MUX_MODE(P15CFG, GPIO_MUX_C1O);
	/*
	(10)开启ACMP
	*/		
	ACMP_Start(ACMP1);
	//ACMP_GetResult(ACMP1);
}

/**************************************************
*函数名称：void x_6990_all_init(void)
*函数功能：开机统一初始化程序
*入口参数：void
*出口参数：void
*备   注：无
**************************************************/
void x_6990_all_init(void)
{
	TMR2_Config();
	ACMP1_Config(0);
	ADC_Config();
	GPIO_Config();
	EPWM_working_Config();
	UART0_Config();//与底座通讯,串口调试,
	UART1_Config();//显示屏
	PGA_Config();
	SYS_EnableWakeUp();
	IRQ_ALL_ENABLE();	
	POWER_DET_ON();
}
