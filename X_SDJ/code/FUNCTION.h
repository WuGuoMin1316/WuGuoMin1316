#ifndef _FUNCTION_H_
#define _FUNCTION_H_


typedef union
{
	unsigned char byte;
	struct
	{
		unsigned char bit0 : 1;
		unsigned char bit1 : 1;
		unsigned char bit2 : 1;
		unsigned char bit3 : 1;
		unsigned char bit4 : 1;
		unsigned char bit5 : 1;
		unsigned char bit6 : 1;
		unsigned char bit7 : 1;
	} bits;
}Union_Flag;
extern Union_Flag Ico,Key,Sys_Flag,Other_Flag,Open_Flag;

#define KEY_CLICK_K1_Flag         	Key.bits.bit0	//按键1短按
#define KEY_CLICK_K2_Flag        	Key.bits.bit1	//按键2短按
#define KEY_CLICK_K3_Flag        	Key.bits.bit2	//按键3短按
#define KEY_CLICK_K4_Flag        	Key.bits.bit3	//按键3短按
#define KEY_LONG_K1_Flag         	Key.bits.bit4	//按键1长按
#define KEY_LONG_K2_Flag         	Key.bits.bit5	//按键2长按
#define KEY_LONG_K3_Flag        	Key.bits.bit6	//按键3长按
#define KEY_LONG_K4_Flag        	Key.bits.bit7	//按键3长按
#define KEY_LONG_RELEASE_K1_Flag	Key.bits.bit6	//按键1 松开
#define KEY_LONG_RELEASE_K2_Flag	Key.bits.bit7	//按键2松开
#define KEY_LONG_RELEASE_K3_Flag 	Sys_Flag.bits.bit0	//按键3松开

#define KEY_LONG_RELEASE_K4_Flag 	Other_Flag.bits.bit7	//按键3松开

#define Robot_On_Flag				Sys_Flag.bits.bit1	//机器是否开机
#define ROLLER_NormalOperation		Sys_Flag.bits.bit2//滚刷运行状态 1/0：正常/不正常
#define ERROR_ROLLER				Sys_Flag.bits.bit3//滚刷报警 1/0 ：报警/不报警
#define STERILIZE_en				Sys_Flag.bits.bit4//消毒模块使能
#define LED_DISPLAY_OFF				Sys_Flag.bits.bit5
#define Self_Cleaning_Flag			Sys_Flag.bits.bit6//进入自清洁,1/0 开始自清洁,在底座上后才执行自清洁程序
#define RobotInBaseFlag				Sys_Flag.bits.bit7//机器在底座上 1/0 在底座上,不在底座上

#define	CleanCistern_Min			Other_Flag.bits.bit0//清水箱  1/0： 最小处有水/最小处无水
#define	CleanCistern_Max			Other_Flag.bits.bit1//清水箱  1/0： 已满/未满
#define	DirtyCistern_Min			Other_Flag.bits.bit2//污水箱  1/0： 最小处有水/最小处无水
#define	DirtyCistern_Max			Other_Flag.bits.bit3//污水箱  1/0： 已满/未满

#define	Hair_Open					Open_Flag.bits.bit0
#define	Roller_Open					Open_Flag.bits.bit1
#define	PumpAIR_Open				Open_Flag.bits.bit2
#define	PumpWater_Open				Open_Flag.bits.bit3
#define	SolenoidValveDIRTY_Open		Open_Flag.bits.bit4
#define	SolenoidValveCLEAN_Open		Open_Flag.bits.bit5
#define	Charger_Open				Open_Flag.bits.bit6

#define Ico_Battery_Flag			Ico.bits.bit0//图标 电池显示标志位
#define	Ico_Standby_Flag			Ico.bits.bit1//图标 待机标志位
#define Ico_Standard_Flag			Ico.bits.bit2//图标 标准拖模式标志位
#define Ico_Dry_Mopping_Flag		Ico.bits.bit3//图标 干拖模式标志位
#define Ico_Wet_Mopping_Flag		Ico.bits.bit4//图标 湿拖模式标志位
#define	Ico_Deodorize_Flag			Ico.bits.bit5//图标 烘干消毒模式标志位
#define	Ico_Self_Cleaning_Flag		Ico.bits.bit6//图标 自清洁模式标志位
#define	Ico_Clean_Water_Flag		Ico.bits.bit7//图标 清水箱警报标志位
#define	Ico_Logo_Flag				Other_Flag.bits.bit4//图标 商标显示标志位
#define	Ico_Dirty_Water_Flag		Other_Flag.bits.bit5//图标 污水箱警报标志位
#define	Ico_Brush_Flag				Other_Flag.bits.bit6//图标 滚刷缠绕警报标志位


#endif