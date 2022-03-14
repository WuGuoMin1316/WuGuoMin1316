
#include "X_PWM.h"
#include "FUNCTION.H"

RunTypeDef xdata Charger,Hair,Roller,PumpAIR,PumpWater,SolenoidValveCLEAN,SolenoidValveDIRTY;
//充电，小毛刷，滚刷，气泵，水泵，清水箱电磁阀，污水箱电磁阀


uchar RunMode;
uchar UART_Com;
uchar UART_Data;
unsigned int WORKING_PWMPeriod_MAX;//25Khz
/**************************************************
*函数名称：void Get_BaseState()
*函数功能：检查时候有外部电源输入,(机器时候放在底座上)
*入口参数：	
*出口参数：void
*备   注：无
**************************************************/
void Get_BaseState()
{
	RobotInBaseFlag = 0;
}
/**************************************************
*函数名称：void STERILIZE(bit en)
*函数功能：开关 消毒器
*入口参数：	
*出口参数：void
*备   注：无
**************************************************/
void STERILIZE(bit en)
{
	if(en)	STERILIZE_ON();
	else 	STERILIZE_OFF();
}
/**************************************************
*函数名称：void SolenoidValve_CLEAN(uchar EN,uint Speed)
*函数功能：开关 清水箱电磁阀
*入口参数：	
*出口参数：void
*备   注：无
**************************************************/

void SolenoidValve_CLEAN (uchar EN,uint Speed)
{
	if(EN)		EPWM_SetChannelSymDuty(electromagnetic_1,Speed);
	else		EPWM_SetChannelSymDuty(electromagnetic_1,0);
}

/**************************************************
*函数名称：void SolenoidValve_DIRTY (uchar EN,uint Speed)
*函数功能：开关 污水箱电磁阀 
*入口参数：	
*出口参数：void
*备   注：无
**************************************************/

void SolenoidValve_DIRTY (uchar EN,uint Speed)
{
	if(EN)		EPWM_SetChannelSymDuty(electromagnetic_2,Speed);
	else		EPWM_SetChannelSymDuty(electromagnetic_2,0);
}
/**************************************************
*函数名称：void PumpWater_CLEAN (uchar EN,uint Speed)
*函数功能：抽清水 水泵
*入口参数：	EN:是否打开水泵抽清水  1/0:  打开水泵/关闭水泵
			Speed: 速度，即PWM占空比
*出口参数：void
*备   注：H电平驱动
**************************************************/
void PumpWater_CLEAN (uchar EN,uint Speed)
{
	if(EN)		EPWM_SetChannelSymDuty(WATER_MOTOR_1,Speed);
	else		EPWM_SetChannelSymDuty(WATER_MOTOR_1,0);
}
/**************************************************
*函数名称：void PumpAIR_DIRTY (uchar EN,uint Speed)
*函数功能：气泵，垃圾，污水分离
*入口参数：	EN:是否打开气泵  1/0:  打开气泵/关闭气泵
			Speed: 速度，即PWM占空比
*出口参数：void
*备   注：H电平驱动
**************************************************/
void PumpAIR_DIRTY (uchar EN,uint Speed)
{
	if(EN)		EPWM_SetChannelSymDuty(WATER_MOTOR_2,Speed);
	else		EPWM_SetChannelSymDuty(WATER_MOTOR_2,0);
}
/**************************************************
*函数名称：void ROLLER_BRUSH (uchar EN,uint Speed)
*函数功能：滚刷
*入口参数：	EN:是否打开滚刷  1/0:  打开滚刷/关闭滚刷
			Speed: 速度，即PWM占空比
*出口参数：void
*备   注：//低电平驱动
**************************************************/
void ROLLER_BRUSH (uchar EN,uint Speed)
{
	if(EN)		EPWM_SetChannelSymDuty(BRUSH_PWM_1,Speed);
	else		EPWM_SetChannelSymDuty(BRUSH_PWM_1,WORKING_PWMPeriod_MAX);
}
/**************************************************
*函数名称：void HAIR_BRUSH (uchar EN,uint Speed,uint Time)
*函数功能：毛刷
*入口参数：	EN:是否打开毛刷  1/0:  打开毛刷/关闭毛刷
			Speed: 速度，即PWM占空比
*出口参数：void
*备   注：//H电平驱动
**************************************************/
void HAIR_BRUSH (uchar EN,uint Speed)
{
	if(EN)		EPWM_SetChannelSymDuty(BRUSH_PWM_2,Speed);
	else		EPWM_SetChannelSymDuty(BRUSH_PWM_2,0);
}
/**************************************************
*函数名称：void Set_AllModule()
*函数功能：设置所有模块的  开关 及其 运行速度（即占空比）
*入口参数：void
*出口参数：void
*备   注：
**************************************************/
void Set_AllModule()
{
	STERILIZE(STERILIZE_en);
	if(RunMode != RUNMODE_CHARGER)	HAIR_BRUSH(Hair_Open, Hair.RunSpeed);//小毛刷
	ROLLER_BRUSH(Roller_Open, WORKING_PWMPeriod_MAX - Roller.RunSpeed);//大滚刷
	PumpAIR_DIRTY(PumpAIR_Open, PumpAIR.RunSpeed);//气泵
	PumpWater_CLEAN(PumpWater_Open, PumpWater.RunSpeed);//水泵
	SolenoidValve_DIRTY(SolenoidValveDIRTY_Open, SolenoidValveDIRTY.RunSpeed);
	SolenoidValve_CLEAN(SolenoidValveCLEAN_Open, SolenoidValveCLEAN.RunSpeed);
	
}

/**************************************************
*函数名称：UART_CommandToBase(uchar a,uchar b)
*函数功能：机器给底座发指令
*入口参数：a：命令；b:数据
*出口参数：void
*备   注：将数据放入缓存区
通讯协议 0xAA + a + b + 0xEF
出现新指令,发送1次,重复的指令5S发送一次
**************************************************/
void UART_CommandToBase()
{
	static uchar tempbuff[4];
	static uchar tempcnt;
	uchar start = 0xAA;
	uchar end = 0xEF;
	tempbuff[0] = start;
	tempbuff[3] = end;
	if(RobotInBaseFlag)
	{
		if((tempbuff[1] == UART_Com)&&(tempbuff[2] == UART_Data))
		{	
			if(tempcnt == 0)	
			{
				//hex_to_uart0(tempbuff,4);	
				my_printf("RunMode=%d,Com=%d,Data=%d\n\r",(uint)RunMode,(uint)UART_Com,(uint)UART_Data);		
			}				
			if(++tempcnt >= RUNTIME25)	tempcnt = 0;						
		}
		else
		{
			tempcnt = 0;
			if(tempbuff[1] != UART_Com)		tempbuff[1] = UART_Com;
			if(tempbuff[2] != UART_Data)	tempbuff[2] = UART_Data;
		}		
	}
	else
		tempcnt = 0;
}


/**************************************************
*函数名称：void Data_init()
*函数功能：数据初始化
*入口参数：void
*出口参数：en
*备   注：无
			
**************************************************/
void Data_init()
{	
	uchar a = 7;
	RunMode = RUNMODE_STANDBY;
	CleanCistern_Max = 0;
	CleanCistern_Min = 1;
	DirtyCistern_Max = 0;
	DirtyCistern_Min = 1;
	ROLLER_NormalOperation = 1;
	RobotInBaseFlag = 0;
	ERROR_ROLLER = 0;
	STERILIZE_en = 0;
	
	SolenoidValveDIRTY.RunSpeed = (WORKING_PWMPeriod_MAX/10)*a;
	SolenoidValveCLEAN.RunSpeed = (WORKING_PWMPeriod_MAX/10)*a;
	PumpWater.RunSpeed = WORKING_PWMPeriod_MAX;
	PumpAIR.RunSpeed = WORKING_PWMPeriod_MAX;
	Roller.TargetSpeed = WORKING_PWMPeriod_MAX;
	Hair.TargetSpeed = WORKING_PWMPeriod_MAX;
	Charger.RunSpeed = (WORKING_PWMPeriod_MAX/10)*a;
	LED_DISPLAY_OFF	= 1;
	Ico_Logo_Flag = 1;//显示LOGO

}
/**************************************************
*函数名称：void GetROLLER_BRUSH_current()
*函数功能：获取滚刷的ADC值,并判断滚刷电机工作电流是否正常
*入口参数：void
*出口参数：en
*备   注：无
			滚刷电机的工作电流过大的持续时间大于300ms后开启保护
**************************************************/
void GetROLLER_BRUSH_current()
{
#define ROLLER_BRUSH_CURRENT_HIGH		2500//( Roller.RunSpeed/WORKING_PWMPeriod_MAX)*550
#define ROLLER_BRUSH_CURRENT_MEDIUM		2000//( Roller.RunSpeed/WORKING_PWMPeriod_MAX)*280
//#define ROLLER_BRUSH_CURRENT_LOW		6000//( Roller.RunSpeed/WORKING_PWMPeriod_MAX)*200
	static uchar cut;
	static uchar cut2;
	int ROLLER_ADC,ROLLER_current;
	
	ROLLER_ADC = get_roller_adc();
	ROLLER_current = ((12000L*ROLLER_ADC)/4095);
	//my_printf("ROLLER_adc = %d,ROLLER_current = %d\r\n",(uint)ROLLER_ADC,(uint)ROLLER_current);
	
	if((cut == 0)&&(ERROR_ROLLER == 0))
	{
		cut2++;
		if(ROLLER_current >= ROLLER_BRUSH_CURRENT_HIGH)	//电流超过上限,并且持续一段时间,判断为滚刷坏
		{		
			if(cut2 > 8)
			{
				cut2 = 0;
				ROLLER_NormalOperation = 0;
				ERROR_ROLLER = 1;
			}
		}
		else if(ROLLER_current >= ROLLER_BRUSH_CURRENT_MEDIUM)//电流稍大,并且持续一段时间,关闭滚刷
		{
			if(cut2 > 5)
			{
				cut2 = 0;
				ROLLER_NormalOperation = 0;
			}
		}
		else
		{
			ROLLER_NormalOperation = 1;	
			cut2 = 0;
		}
		Roller_Open = ROLLER_NormalOperation;
	}
	if(ROLLER_NormalOperation == 0)//关闭一段时间后重新打开滚刷
	{
		if(++cut > 20)	cut = 0;
	}
}
/**************************************************
*函数名称：ErrorChecking()
*函数功能：错误校验
*入口参数：void
*出口参数：
*备   注：无
**************************************************/
void ErrorChecking()
{
	if(ERROR_ROLLER)
		Roller_Open = 0;	
}
/**************************************************
*函数名称：void Get_CLEAN_adc()
*函数功能：获取清水检测ADC值,并判断清水桶中水量的状态
*入口参数：void
*出口参数：en
*备   注：无
			AN0_清水
3对红外对管，
**************************************************/
void Get_CLEAN_adc()
{

	uint CLEAN_ADC;
	CLEAN_ADC = get_clean_adc();
	if(CLEAN_ADC < 4000)
	{
		if(CLEAN_ADC < CLEAN_ADC_MAX + CLEAN_ADC_DIF)
		{
			CleanCistern_Max = 1;
			CleanCistern_Min = 1;
		}
		else 
		{
			CleanCistern_Max = 0;
			CleanCistern_Min = 0;
		}
	}
	else
	{
		CleanCistern_Max = 0;
		CleanCistern_Min = 1;
	}
}
/**************************************************
*函数名称：void Get_DIRTY_adc()
*函数功能：获取污水检测ADC值,并判断污水桶中水量的状态
*入口参数：void
*出口参数：en
*备   注：无
			AN6_污水
1对红外对管，无光，约为180，有光，4096
**************************************************/
void Get_DIRTY_adc()
{
	uint DIRTY_ADC;
	DIRTY_ADC = get_dirty_adc();
	if(DIRTY_ADC < DIRTY_ADC_MIN + DIRTY_ADC_DIF)//小于最小
	{
		DirtyCistern_Min = 1;
	}
	else if(DIRTY_ADC > DIRTY_ADC_MAX)//大于最大
	{
		DirtyCistern_Max = 1;
	}
	else//在中间
	{
		DirtyCistern_Max = 0;
		DirtyCistern_Min = 0;
	}
	//my_printf("DIRTY_ADC=%d\r\n",(uint)DIRTY_ADC);
}
/**************************************************
*函数名称：void GiveWaterTime()
*函数功能：控制水泵给水时间,不同拖地模式出水量不同(标准拖和加强湿拖)
*入口参数：
*出口参数：
*备   注：20ms/1次
**************************************************/
void GiveWaterTime()
{
	static uint GiveWater_cnt;
	static bit GiveWater_Flag;//1/0：不给水/给水
	static uint timemax;
	//检测清水箱是否有水
	//只在拖地模式中执行(标准拖和加强湿拖)
	if((CleanCistern_Min)&&((RunMode == RUNMODE_STANDARD)||(RunMode == RUNMODE_WET_MOPPING)))
	{
		if(GiveWater_cnt == 0)
		{
			if(GiveWater_Flag)	timemax = 50*RunMode;//不供水
			else				timemax = 10*RunMode;//供水时长
			GiveWater_Flag=!GiveWater_Flag;	
			PumpWater_Open = GiveWater_Flag;
			my_printf("PumpWater_Open = %d\r\n",(uint)PumpWater_Open);
		}
		if(++GiveWater_cnt >= timemax)	
			GiveWater_cnt = 0;
	}
	else
	{
		GiveWater_cnt = 0;
		PumpWater_Open = 0;
		GiveWater_Flag = 1;
	}
}
void GiveWaterTime_pwm()
{
	//检测清水箱是否有水
	//只在拖地模式中执行(标准拖和加强湿拖)
	if((CleanCistern_Min)&&((RunMode == RUNMODE_STANDARD)||(RunMode == RUNMODE_WET_MOPPING)))
	{

		if(RunMode == RUNMODE_STANDARD)//标准拖
		{
			PumpWater_Open = 1;
			PumpWater.TargetSpeed = 200;
		}
		if(RunMode == RUNMODE_WET_MOPPING)//加强湿拖
		{
			PumpWater_Open = 1;
			PumpWater.TargetSpeed = 400;
		}
	}
	else
	{
		PumpWater.RunSpeed = 0;
		PumpWater.TargetSpeed = 0;
		PumpWater_Open = 0;
	}
}

/**************************************************
*函数名称：void Robot_RunMode()
*函数功能：拖地模式下运行的工作模式
*入口参数：mode：			

*出口参数：void
*备   注：无
污水箱处的三通电磁阀,不上电时,为大气压,上电后,
**************************************************/
void Robot_RunMode()
{
	static bit Undone;//1:0  工作未完成/工作完成
	static uchar last_RobotInBaseFlag = 0;
	static uchar Pcnt = 0;
	static uint SelfClean_CNT = 0;
	static uchar DirtyC_Max_cnt;
	static uchar Dirty_out_cnt;
	
	
	//Get_CLEAN_adc();//获取清水箱的状态
	//Get_DIRTY_adc();//获取污水箱的状态
	//待机模式： 所有PWM关
	if(RunMode == RUNMODE_STANDBY)
	{	
		Hair.TargetSpeed = 0;
		Roller.TargetSpeed = 0;
		Hair_Open = 0;
		Roller_Open = 0;
		PumpAIR_Open = 0;
		PumpWater_Open = 0;
		SolenoidValveDIRTY_Open = 0;
		SolenoidValveCLEAN_Open = 0;
		STERILIZE_en = 0;
		//Ico_Deodorize_Flag = 0;
	}
	if(RobotInBaseFlag)
	{	
		if(RunMode == RUNMODE_DEODORIZE)//打开烘干消毒
		{
			Ico_Deodorize_Flag = 1;
			Hair.TargetSpeed = 0;
			Roller.TargetSpeed = WORKING_PWMPeriod_MAX/RUNMODE_DEODORIZE;
			Hair_Open = 0;
			Roller_Open = 1;
			PumpAIR_Open = 0;
			PumpWater_Open = 0;
			SolenoidValveDIRTY_Open = 0;
			SolenoidValveCLEAN_Open = 0;
			STERILIZE_en = 1;
			UART_Com = UART_Com_DEODORIZE;
			UART_Data = 1;
		}		
		else if(RunMode == RUNMODE_SELFCLEANING)//自清洁:包含排污和补充清水
		{
			Hair.TargetSpeed = 0;
			Roller.TargetSpeed = 0;
			Hair_Open = 0;
			Roller_Open = 0;
			PumpWater_Open = 0;
			STERILIZE_en = 0;
			SolenoidValveDIRTY_Open = 0;
			//Ico_Deodorize_Flag = 0;
			UART_Com = UART_Com_AddWater;
			UART_Data = 1;	
			
			if(CleanCistern_Max)// 清水箱满后,进行排污排污
			{
				SolenoidValveCLEAN_Open = 1;
				PumpAIR_Open = 1;
				if(Dirty_out_cnt < 250)
					Dirty_out_cnt++;
			}
			else//补充清水
			{
				SolenoidValveCLEAN_Open = 0;
				SolenoidValveDIRTY_Open = 1;
				PumpAIR_Open = 0;
				
			}
			if((!DirtyCistern_Min)&&(Dirty_out_cnt > 200))//污水箱无污水
			{
				Dirty_out_cnt = 0;
				SolenoidValveCLEAN_Open = 0;
				PumpAIR_Open = 0;
				UART_Data = 0;
				RunMode = RUNMODE_DEODORIZE;
				
			}
		}			
	}
	else
	{			
		//拖地模式：标准拖/干拖/加强湿拖, 
		if((RunMode == RUNMODE_STANDARD)||(RunMode == RUNMODE_DRY_MOPPING)||(RunMode == RUNMODE_WET_MOPPING))
		{
			
			//数据初始化
			DirtyC_Max_cnt = 0;
			ERROR_ROLLER = 0;
			//电磁阀关，
			SolenoidValveDIRTY_Open = 1;
			SolenoidValveCLEAN_Open = 0;
			//打开气泵并设置速度,抽真空
			PumpAIR_Open = 1;
			//是否需要消毒
			STERILIZE_en = 1;
			//Ico_Deodorize_Flag = 0;
			//获取ADC采样值，判断滚刷是否正常运行（滚刷电机工作电流是否过大）
			GetROLLER_BRUSH_current();
			//Roller_Open = 1;
			Hair_Open = Roller_Open;//打开滚刷和小毛刷
			
			if(Roller_Open == 0)
			{
				Hair.TargetSpeed = 0;
				Roller.TargetSpeed = 0;
			}
			else
			{
				Hair.TargetSpeed = WORKING_PWMPeriod_MAX;
				Roller.TargetSpeed = WORKING_PWMPeriod_MAX;		
			}	
			//检测污水箱是否满		
			if(DirtyCistern_Max)
			{
				//检测污水箱满1S后，进入待机模式
				if(++DirtyC_Max_cnt>100)
				{	
					DirtyC_Max_cnt = 0;
					RunMode = RUNMODE_STANDBY;
				}
			}			
		}
	}
	GiveWaterTime_pwm();//控制水泵的出水量,只在扫地模式中执行,加强湿拖需要增加水泵的出水量
	Set_AllModule();
	UART_CommandToBase();
}
	/*
	//排污模式：在底座上进行排污模式，排污模式完成后再进入充电模式
	if(RunMode == RUNMODE_BLEEDWATER)
	{
		//关水泵
		Hair_Open = 0;
		Roller_Open = 0;
		PumpWater_Open = 0;
			
		SolenoidValveCLEAN_Open = 0;
		PumpAIR_Open = 0;//关闭气泵		
		STERILIZE_en = 1;
		Hair.TargetSpeed = 0;
		Roller.TargetSpeed = 0;	
		PumpWater.TargetSpeed = 0;	
		//判断该污水箱是否有污水
		if(DirtyCistern_Min)
		{
			//检测机器是否在充电底座上
			if(RobotInBaseFlag)
			{
				SolenoidValveDIRTY_Open = 0;//打开三通，让污水流进底座污水箱
				PumpAIR_Open = 0;//关闭气泵
			}
			else
			{
				if(last_RobotInBaseFlag == 1)//排污过程中离开底座
				{			
					Undone = 1;	//工作未完成
					RunMode = RUNMODE_VACUUMIZE;		
				}			
			}
			last_RobotInBaseFlag = RobotInBaseFlag;		
		}
		else
		{
			SolenoidValveDIRTY_Open = 1;
		}
	}


	//自清洁
	if(RunMode == RUNMODE_SELFCLEANING)
	{

			SelfClean_CNT = 0;
			//机器上PWM都关闭
			Hair_Open = 1;
			Roller_Open = 1;
			Hair.TargetSpeed = WORKING_PWMPeriod_MAX;
			Roller.TargetSpeed = WORKING_PWMPeriod_MAX/RUNMODE_SELFCLEANING;
			Hair.TargetSpeed = 0;
			Roller.TargetSpeed = 0;
			PumpWater.TargetSpeed = 0;
			PumpAIR_Open = 0;
			PumpWater_Open = 0;
			SolenoidValveDIRTY_Open = 0;
			SolenoidValveCLEAN_Open = 1;
			STERILIZE_en = 1;

		//检测机器是否在充电底座上,在底座上才可以自清洁
		if(RobotInBaseFlag)
		{
			//串口发指令给底座板打开水泵
			UART_Com = UART_Com_SelfCleaning;
			UART_Data = 1;			
			if(++SelfClean_CNT>6000)//10分钟
			{
				SelfClean_CNT = 0;
				RunMode = RUNMODE_STANDBY;//退出自清洁模式进入待机模式
			}
		}
		else
		{
			SelfClean_CNT = 0;
			if(last_RobotInBaseFlag == 1)//自清洁过程中离开底座
			{			
				Undone = 1;	//工作未完成
				RunMode = RUNMODE_VACUUMIZE;
			}		
		}
		last_RobotInBaseFlag = RobotInBaseFlag;
	}
	
	
	//短暂抽真空模式：排污，自清洁未处理完就离开充电底座，抽真空，
	if(RunMode == RUNMODE_VACUUMIZE)
	{

			//机器上PWM都关闭
			Hair_Open = 0;
			Roller_Open = 0;
			Hair.TargetSpeed = 0;
			Roller.TargetSpeed = 0;
			PumpWater.TargetSpeed = 0;
			PumpWater_Open = 0;
			SolenoidValveDIRTY_Open = 0;
			SolenoidValveCLEAN_Open = 0;
			STERILIZE_en = 0;		
			
		if(Undone == 1)
		{
			PumpAIR_Open = 1;//开气泵
			if(++Pcnt>=RUNTIME25)//抽真空2.5s，
			{
				Pcnt = 0;
				Undone = 0;														
			}		
		}
		else
		{
			PumpAIR_Open = 0;		
			RunMode = RUNMODE_STANDBY;//退出短暂抽真空模式进入待机模式
		}	
	}
	if(RunMode == RUNMODE_DRAINANDADDWATER)//排污和补充清水同时进行
	{
		//检测机器是否在充电底座上
		if(RobotInBaseFlag)
		{	
			if((CleanCistern_Max == 1)&&(DirtyCistern_Min == 0))
			{
				//进入充电模式
				RunMode = RUNMODE_CHARGER;
			}		
			//是否需要补充清水
			//水箱满
			//串口发指令给底座板关闭水泵
			UART_Com = UART_Com_AddWater;
			UART_Data = 0;			
			//判断该污水箱是否有污水
			SolenoidValveDIRTY_Open = DirtyCistern_Min;//打开三通，让污水流进底座污水箱									
		}
		else
		{
			if((last_RobotInBaseFlag == 1)&&(DirtyCistern_Min))//排污过程中离开底座
			{			
				Undone = 1;	//工作未完成
				RunMode = RUNMODE_VACUUMIZE;		
			}
		}
		last_RobotInBaseFlag = RobotInBaseFlag;		
	}
	*/

/*************************************************
function name:Gradual_Calculation(unsigned char *Value_Now,unsigned char *Value_Target,unsigned char Speed)
function work: 但前值以设置的步进向目标值递进
input data   : *Value_Now：但前值；*Value_Target：目标值；Speed：步进
output data  : null
note：
*************************************************/
void Gradual_Calculation(unsigned int *Value_Now,unsigned int *Value_Target,unsigned char Speed)
{
	if (*Value_Now < *Value_Target)
	{
		*Value_Now+=Speed;
	}
	else if (*Value_Now > *Value_Target)
	{
		*Value_Now-=Speed;
	}
	else
	{
		*Value_Now = *Value_Target;
	}
}
/*************************************************
function name:void RunSpeed_GradualChange()
function work: RunSpeed渐变计算
input data   : null
output data  : null
note：//渐变计算	,当前速度转变为目标速度，步进为1
*************************************************/
void RunSpeed_GradualChange()
{
	if(Hair.RunSpeed <= WORKING_PWMPeriod_MAX - 10)
	{
		Gradual_Calculation(&Hair.RunSpeed,&Hair.TargetSpeed,10);
		//Gradual_Calculation(&Roller.RunSpeed,&Roller.TargetSpeed,10);	
		
		
	}
	Roller.RunSpeed = Roller.TargetSpeed;
	if(PumpWater.RunSpeed <= WORKING_PWMPeriod_MAX - 10)
		Gradual_Calculation(&PumpWater.RunSpeed,&PumpWater.TargetSpeed,10);
	//Gradual_Calculation(&SolenoidValveCLEAN.RunSpeed,&SolenoidValveCLEAN.TargetSpeed,1);
	//Gradual_Calculation(&SolenoidValveDIRTY.RunSpeed,&SolenoidValveDIRTY.TargetSpeed,1);
}




