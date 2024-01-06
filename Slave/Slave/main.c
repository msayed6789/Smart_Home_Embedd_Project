
#include "DIO_Interface.h"
#include "LCD.h"
#include "KeyPad.h"
#include "ADC.h"
#include "Sensors.h"
#include "EX_Interrupt.h"
#include "SPI.h"
#include "Timers.h" 
void func (void);
void control (void);
void ff(void);
void Timeout (void);
void Clear_Blocking (void);
/***********************************Rooms*******************************************/
#define  Room1                    PIND0
#define  Room2                    PIND1
#define  Room3                    PIND2
#define  Room4                    PIND3
#define  TV                       PIND4
#define  Air_Condition            PIND5
#define AirCondit_CheckTime       250                //Time in ms
volatile u8 f=0,ch,Detect_Flag=0,Control_Flag=0,Status_Flag=0,str[2],Air_Flag=0,ON_Flag=0,Condit_flag=0,Count_Flag=0;
volatile u16 temp=0,temp_Read=0;
volatile u32 Owner_CalTime=(((u32)(AirCondit_CheckTime)*1000)/50000)-1;
int main(void)
{
	u8 volatile Equal_Flag=0,FirstTime_Flag=0;
	sei();
	DIO_init();
	LCD_init();
	ADC_init(ADC_VCC,ADC_SCAler_64);
	LCD_WriteString("Slave  ");
	SPI_Init(Slave,Frequancy_4,MSB);
	SPI_SetcallBack(control);
	SPI_InterruptEnable();
    while (1)
    {
		if (Status_Flag==1)
		{
			switch (str[0])
			{
				case 1:
				SPI_Send(DIO_ReadPin(Room1));
				break;
				case 2:
				SPI_Send(DIO_ReadPin(Room2));
				break;
				case 3:
				SPI_Send(DIO_ReadPin(Room3));
				break;
				case 4:
				SPI_Send(DIO_ReadPin(Room4));
				break;
				case 5:
				SPI_Send(DIO_ReadPin(TV));
				break;
			}
		}
		else if (Control_Flag==1)
		{
			switch (str[1])
			{
				case 'n':
				switch (str[0])
				{
					case 1:
					DIO_WRitePin(Room1,HIGH);
					break;
					case 2:
					DIO_WRitePin(Room2,HIGH);
					break;
					case 3:
					DIO_WRitePin(Room3,HIGH);
					break;
					case 4:
					DIO_WRitePin(Room4,HIGH);
					break;	
					case 5:
					DIO_WRitePin(TV,HIGH);
					break;
					case 6:
					ON_Flag=1;
					if (FirstTime_Flag==0)
					{
						FirstTime_Flag=1;
					}
					else
					{
						Air_Flag=1;
					}
					break;
				}
				break;
				case 'f':
				switch (str[0])
				{
					case 1:
					DIO_WRitePin(Room1,LOW);
					break;
					case 2:
					DIO_WRitePin(Room2,LOW);
					break;
					case 3:
					DIO_WRitePin(Room3,LOW);
					break;
					case 4:
					DIO_WRitePin(Room4,LOW);
					break;
					case 5:
					DIO_WRitePin(TV,LOW);
					break;
					case 6:
					DIO_WRitePin(Air_Condition,LOW);
					Timer1_OCA_InterruptDisable();
					Air_Flag=0;
					ON_Flag=0;
					Count_Flag=0;
					break;
					
				}
				break;
			}
		}
	if ((Count_Flag>=Owner_CalTime||Air_Flag==1)&&(Detect_Flag==1))
	{
		Air_Flag=0;
		Timeout();
		Count_Flag=0;
			temp_Read=LM35_GetTemp();
			if (temp_Read>(temp))
			{
				DIO_WRitePin(Air_Condition,HIGH);
				Equal_Flag=1;
			}
			else if (temp_Read<temp)
			{
				DIO_WRitePin(Air_Condition,LOW);
				Equal_Flag=0;
			}
			else 
			{
				if (Equal_Flag==1)
				{
					DIO_WRitePin(Air_Condition,HIGH);
				}
				else if (Equal_Flag==0)
				{
					DIO_WRitePin(Air_Condition,LOW);
				}
			}
		}	
  }
	
}
void control (void)
{
	s8 static i=0 ;
	ch=SPI_Receive();
	if (ch=='s')
	{
		i=1;	
	}
	else if (i==1)
	{
		Status_Flag=1;
		Control_Flag=1;
		str[0]=ch;
		i=2;
	}
	else if (i==3&&ch!='m'&&Condit_flag!=1)
	{
		Control_Flag=1;
		str[1]=ch;
		Status_Flag=0;
		i++;
	}
	else if (ch=='d')
	{
		i=0;
		str[0]=0;
		str[1]=0;
	}
	
	else if (ch=='m')
	{
		Condit_flag=1;
	}
	else if (Condit_flag)
	{
		if (ON_Flag)
		{
			temp=ch;
			Detect_Flag=1;
			Air_Flag=1;
			f=1;
		}
		Condit_flag=0;
	}
	else
	{
		i++;
	}
}
void Timeout ()
{
	Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_8);
	Timer1_OCA_SetCallBack(Clear_Blocking);
	Timer1_OCA_InterruptEnable();
	OCR1A=49999;
}
void Clear_Blocking (void)
{
	Count_Flag++;
}
