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

#define KEY_CLICK_K1_Flag         	Key.bits.bit0	//����1�̰�
#define KEY_CLICK_K2_Flag        	Key.bits.bit1	//����2�̰�
#define KEY_CLICK_K3_Flag        	Key.bits.bit2	//����3�̰�
#define KEY_CLICK_K4_Flag        	Key.bits.bit3	//����3�̰�
#define KEY_LONG_K1_Flag         	Key.bits.bit4	//����1����
#define KEY_LONG_K2_Flag         	Key.bits.bit5	//����2����
#define KEY_LONG_K3_Flag        	Key.bits.bit6	//����3����
#define KEY_LONG_K4_Flag        	Key.bits.bit7	//����3����
#define KEY_LONG_RELEASE_K1_Flag	Key.bits.bit6	//����1 �ɿ�
#define KEY_LONG_RELEASE_K2_Flag	Key.bits.bit7	//����2�ɿ�
#define KEY_LONG_RELEASE_K3_Flag 	Sys_Flag.bits.bit0	//����3�ɿ�

#define KEY_LONG_RELEASE_K4_Flag 	Other_Flag.bits.bit7	//����3�ɿ�

#define Robot_On_Flag				Sys_Flag.bits.bit1	//�����Ƿ񿪻�
#define ROLLER_NormalOperation		Sys_Flag.bits.bit2//��ˢ����״̬ 1/0������/������
#define ERROR_ROLLER				Sys_Flag.bits.bit3//��ˢ���� 1/0 ������/������
#define STERILIZE_en				Sys_Flag.bits.bit4//����ģ��ʹ��
#define LED_DISPLAY_OFF				Sys_Flag.bits.bit5
#define Self_Cleaning_Flag			Sys_Flag.bits.bit6//���������,1/0 ��ʼ�����,�ڵ����Ϻ��ִ����������
#define RobotInBaseFlag				Sys_Flag.bits.bit7//�����ڵ����� 1/0 �ڵ�����,���ڵ�����

#define	CleanCistern_Min			Other_Flag.bits.bit0//��ˮ��  1/0�� ��С����ˮ/��С����ˮ
#define	CleanCistern_Max			Other_Flag.bits.bit1//��ˮ��  1/0�� ����/δ��
#define	DirtyCistern_Min			Other_Flag.bits.bit2//��ˮ��  1/0�� ��С����ˮ/��С����ˮ
#define	DirtyCistern_Max			Other_Flag.bits.bit3//��ˮ��  1/0�� ����/δ��

#define	Hair_Open					Open_Flag.bits.bit0
#define	Roller_Open					Open_Flag.bits.bit1
#define	PumpAIR_Open				Open_Flag.bits.bit2
#define	PumpWater_Open				Open_Flag.bits.bit3
#define	SolenoidValveDIRTY_Open		Open_Flag.bits.bit4
#define	SolenoidValveCLEAN_Open		Open_Flag.bits.bit5
#define	Charger_Open				Open_Flag.bits.bit6

#define Ico_Battery_Flag			Ico.bits.bit0//ͼ�� �����ʾ��־λ
#define	Ico_Standby_Flag			Ico.bits.bit1//ͼ�� ������־λ
#define Ico_Standard_Flag			Ico.bits.bit2//ͼ�� ��׼��ģʽ��־λ
#define Ico_Dry_Mopping_Flag		Ico.bits.bit3//ͼ�� ����ģʽ��־λ
#define Ico_Wet_Mopping_Flag		Ico.bits.bit4//ͼ�� ʪ��ģʽ��־λ
#define	Ico_Deodorize_Flag			Ico.bits.bit5//ͼ�� �������ģʽ��־λ
#define	Ico_Self_Cleaning_Flag		Ico.bits.bit6//ͼ�� �����ģʽ��־λ
#define	Ico_Clean_Water_Flag		Ico.bits.bit7//ͼ�� ��ˮ�侯����־λ
#define	Ico_Logo_Flag				Other_Flag.bits.bit4//ͼ�� �̱���ʾ��־λ
#define	Ico_Dirty_Water_Flag		Other_Flag.bits.bit5//ͼ�� ��ˮ�侯����־λ
#define	Ico_Brush_Flag				Other_Flag.bits.bit6//ͼ�� ��ˢ���ƾ�����־λ


#endif