#include "cms8s6990.h"
#include "X_PWM.h"
#include "X_FLASH.h"


//Hair,Roller,PumpAIR,PumpWater,SolenoidValveCLEAN,SolenoidValveDIRTY;

/*************************************************
function name: void FLASH_DownData()
function work: 将eeprom中的数据读出
input data   : en :使能，启动中才保存数据，
output data  : null
note：
*************************************************/
void FLASH_DownData(char en)
{
	if(en)
	{
		Hair.RunSpeed = FLASH_Read(FLASH_DATA,Address_1);	
		Roller.RunSpeed = FLASH_Read(FLASH_DATA,Address_2);	
		PumpAIR.RunSpeed = FLASH_Read(FLASH_DATA,Address_3);	
		PumpWater.RunSpeed = FLASH_Read(FLASH_DATA,Address_4);	
		SolenoidValveDIRTY.RunSpeed = FLASH_Read(FLASH_DATA,Address_5);	
		SolenoidValveCLEAN.RunSpeed = FLASH_Read(FLASH_DATA,Address_6);	
	}
}
/*************************************************
function name: void FLASH_UpData()
function work: 将更新后的数据重新写入到eeprom中
input data   : en :低电平使能，启动中才保存数据，
output data  : null
note：
*************************************************/
void FLASH_UpData(char en)
{
	if(en)
	{
		FLASH_Erase(FLASH_DATA,Start_Address);		  // 扇区： 第0个扇区
		FLASH_Write(FLASH_DATA,Address_1,Hair.RunSpeed );
		FLASH_Write(FLASH_DATA,Address_2,Roller.RunSpeed);
		FLASH_Write(FLASH_DATA,Address_3,PumpAIR.RunSpeed);	
		FLASH_Write(FLASH_DATA,Address_4,PumpWater.RunSpeed );
		FLASH_Write(FLASH_DATA,Address_5,SolenoidValveDIRTY.RunSpeed);
		FLASH_Write(FLASH_DATA,Address_6,SolenoidValveCLEAN.RunSpeed);

	}
}

	

