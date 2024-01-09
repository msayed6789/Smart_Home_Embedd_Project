
#include "SPI.h"
#include "DIO_Interface.h"
#include "EEPROM.h"
#include "EEPROM_Services.h"
#include "KeyPad.h"
#include "LCD.h"
#include "Timers.h"
#include "Timers_Services.h"
#define Owner_PasswordAdress       (u16)0x01
#define Owner_Password_FlagAddress (u16)0x06
#define Guest_PasswordAdress       (u16)0x07
#define Guest_Password_FlagAddress (u16)0xC
#define Password_Count               3
#define Owner_Led                    PIND0
#define Guest_Led                    PIND1
#define Block_Led                    PIND2
/**************************Blocking Mode********************************************/
#define Blocking_Time     20000              //time in ms
//void Blocking(void);
void Clear_Blocking (void);
/***************************************Timeout*********************************/
void Timeout (void);
#define Owner_Timeout     20000               //time in ms 
#define Guest_Timeout     20000              //time in ms  
/***************************************Communication_Functions*************************/
void Send_f(u8 f); 
void ON_OFF_f(u8 f,u8 af);
void wrong (u8 num, u8 detect_mode);
volatile u16 Owner_Password=0,Guest_Password=0;
typedef enum{
	Owner=0,
	Guest,
	Block
}Mode_t;
volatile u8 Blocking_Flag=1,Timeout_Flag=1,status,a,count=0,s=NO_KEY,Air_Flag=0,ON_Flag=0;
volatile u32 Count_Flag=0;
 volatile u32 Owner_CalTime=(((u32)(Owner_Timeout)*1000)/50000)-1,Guest_CalTime=(((u32)(Guest_Timeout)*1000)/50000)-1,Blocking_CalTime=(((u32)(Blocking_Time)*1000)/50000)-1;
int main(void)
{
	sei();
	DIO_init();
	LCD_init();
	SPI_Init(Master);
	u16 num=0;
	u8 count=0,P_count=Password_Count;
	Mode_t Mode=Block;
    if (EEPROM_Read(Owner_Password_FlagAddress)==255)
	{
		LCD_GoTo(0,0);
		LCD_WriteString("First Used ");
		LCD_GoTo(1,0);
		LCD_WriteString("Set Owner Password");
		_delay_ms(1000);
		LCD_Clear();
		LCD_WriteString("Owner Password");
		LCD_GoTo(1,0);
		while (count<4)
		{
			s=KEYPAD_GetKey();
			if (s!=NO_KEY)
			{
				
				if (s<='9'&& s>='0')
				{
					num=num*10+(s-'0');
					LCD_GoTo(1,0+count);
					LCD_WriteChar(s);
					_delay_ms(300);
					LCD_GoTo(1,0+count);
					LCD_WriteChar('*');
					count++;
				}
				else if (s=='c')
				{
					num=num/10;
					count--;
					LCD_GoTo(1,0+count);
					LCD_WriteChar(' ');
				}
			}
		}
		Owner_Password=num;
		EEPROM_WritePassword_4Number(Owner_PasswordAdress,Owner_Password);
		count=0;
		num=0;
		s=NO_KEY;
		EEPROM_Write(Owner_Password_FlagAddress,0);
	}
	if (EEPROM_Read(Guest_Password_FlagAddress)==255)
	{
		LCD_GoTo(0,0);
		LCD_WriteString("First Used ");
		LCD_GoTo(1,0);
		LCD_WriteString("Set Guest Password");
		_delay_ms(1000);
		LCD_Clear();
		LCD_WriteString("Guest Password");
		LCD_GoTo(1,0);
		while (count<4)
		{
			s=KEYPAD_GetKey();
			if (s!=NO_KEY)
			{
				
				if (s<='9'&& s>='0')
				{
					num=num*10+(s-'0');
					LCD_GoTo(1,0+count);
					LCD_WriteChar(s);
					_delay_ms(300);
					LCD_GoTo(1,0+count);
					LCD_WriteChar('*');
					count++;
				}
				else if (s=='c')
				{
					num=num/10;
					count--;
					LCD_GoTo(1,0+count);
					LCD_WriteChar(' ');
				}
			}
		}
		Guest_Password=num;
		EEPROM_WritePassword_4Number(Guest_PasswordAdress,Guest_Password);
		count=0;
		num=0;
		s=NO_KEY;
		EEPROM_Write(Guest_Password_FlagAddress,0);
	}

	else 
	{
		Owner_Password=EEPROM_ReadPassword_4Number(Owner_PasswordAdress);
		Guest_Password=EEPROM_ReadPassword_4Number(Guest_PasswordAdress);
	}
	
    while (1) 
    {
	num=0;
	count=0;
	Timeout_Flag=1;
	LCD_Clear();
	LCD_GoTo(0,10);
	LCD_WriteString("Select The Mode ");
	LCD_GoTo(1,0);
	LCD_WriteString("1)OWNER MODE        2)GUEST MODE");
	while (count<1)
	{
		s=KEYPAD_GetKey();
		if (s!=NO_KEY)
		{
			
			if (s=='1'|| s=='2')
			{
				num=num*10+(s-'0');
				count++;
			}
			else
			{
				LCD_WriteString("Enter The Right Mode");
			}
		}
	}
	switch (num)
	{
		case 1:
		num=0;
		count=0;
		s=NO_KEY;
		while (P_count>0)
	{
		LCD_Clear();
		LCD_GoTo(0,5);
		LCD_WriteString("Enter The Owner Password");
		while (count<4)
		{
			s=KEYPAD_GetKey();
			if (s!=NO_KEY)
			{
				if (s<='9'&& s>='0')
				{
					num=num*10+(s-'0');
					LCD_GoTo(1,15+count);
					LCD_WriteChar(s);
					_delay_ms(300);
					LCD_GoTo(1,15+count);
					LCD_WriteChar('*');
					count++;
				}
				else if (s=='c')
				{
					num=num/10;
					count--;
					LCD_GoTo(1,15+count);
					LCD_WriteChar(' ');
					LCD_GoTo(1,15+count);
				}
			}
		}
		if (num==Owner_Password)
		{
			LCD_Clear();
			LCD_GoTo(0,8);
			Mode=Owner;
			LCD_WriteString("You are in Owner Mode");
			DIO_WRitePin(Owner_Led,HIGH);
			_delay_ms(1000);
			num=0;
			count=0;
			s=NO_KEY;
			P_count=0;
		}
		else
		{
			P_count--;
			count=0;
			LCD_Clear();
			LCD_WriteString("Wrong Password");
			LCD_GoTo(1,0);
			LCD_WriteString("Tries Left   ");
			LCD_WriteNumber(P_count);
			_delay_ms(2000);
			if (P_count==0)
			{
				Mode=Block;
			}
		}
	}
	    P_count=Password_Count;
		break;
		case 2:
		num=0;
		count=0;
		s=NO_KEY;
		while (P_count>0)
	{
		LCD_Clear();
		LCD_GoTo(0,5);
		LCD_WriteString("Enter The Guest Password");
		while (count<4)
		{
			s=KEYPAD_GetKey();
			if (s!=NO_KEY)
			{
				if (s<='9'&& s>='0')
				{
					num=num*10+(s-'0');
					LCD_GoTo(1,15+count);
					LCD_WriteChar(s);
					_delay_ms(300);
					LCD_GoTo(1,15+count);
					LCD_WriteChar('*');
					count++;
				}
				else if (s=='c')
				{
					num=num/10;
					count--;
					LCD_GoTo(1,15+count);
					LCD_WriteChar(' ');
					LCD_GoTo(1,15+count);
				}
			}
		}
		if (num==Guest_Password)
		{
			Mode=Guest;
			LCD_Clear();
			LCD_GoTo(0,8);
			LCD_WriteString("You are in Guest Mode");
			DIO_WRitePin(Guest_Led,HIGH);
			_delay_ms(1000);
			num=0;
			count=0;
			s=NO_KEY;
			P_count=0;
		}
		else 
		{
			P_count--;
			LCD_Clear();
			LCD_GoTo(0,0);
			LCD_WriteString("Wrong Password");
			LCD_GoTo(1,0);
			LCD_WriteString("Tries Left   ");
			LCD_WriteNumber(P_count);
			_delay_ms(2000);
			if (P_count==0)
			{
				Mode=Block;
			}
		}
	}
	    P_count=Password_Count;
		break;
	}
		LCD_Clear();
		if (Mode==Owner)
		{
			LCD_GoTo(0,30);
			LCD_WriteString("Owner Mode");
			num=0;
			s=NO_KEY;
			count=0;
			LCD_GoTo(0,0);
			LCD_WriteString("1)Room1  2)Room2  3)Room3");
			LCD_GoTo(1,0);
			LCD_WriteString("4)Room4  5)TV  6)Air Condition");
			while (Timeout_Flag==1)
		  {
			SPI_SendReceive('s');
			_delay_ms(5);
			Timeout();
			num=0;
			s=NO_KEY;
			count=0;
			 if (Count_Flag>=Owner_CalTime)
			{
				Timer1_OCA_InterruptDisable();
				Timeout_Flag=0;
				Count_Flag=0;
				DIO_WRitePin(Owner_Led,LOW);
				LCD_Clear();
				LCD_GoTo(0,13);
				LCD_WriteString("Session Timeout!!");
				_delay_ms(1000);
			}
			while (count<1 && Timeout_Flag==1)
			{
				if (Count_Flag>=Owner_CalTime)
				{
					Timer1_OCA_InterruptDisable();
					Timeout_Flag=0;
					Count_Flag=0;
					DIO_WRitePin(Owner_Led,LOW);
					LCD_Clear();
					LCD_GoTo(0,13);
					LCD_WriteString("Session Timeout!!");
					_delay_ms(1000);
				}
				s=KEYPAD_GetKey();
				if (s!=NO_KEY)
				{
					if (s<='6'&& s>='1')
					{
						num=num*10+(s-'0');
						count++;
						SPI_SendReceive(num);
						a=num;
					}
					else 
					{
					  LCD_Clear();
					  LCD_GoTo(0,15);
					  LCD_WriteString("Wrong Input");
					  _delay_ms(1000);	
					  LCD_GoTo(0,0);
					  LCD_WriteString("1)Room1  2)Room2  3)Room3");
					  LCD_GoTo(1,0);
					  LCD_WriteString("4)Room4  5)TV  6)Air Condition");
					}
				}
			}
		Send_f(Owner);	
		count=0;
     }
  }
		
		else if (Mode==Guest)
		{
			LCD_GoTo(0,30);
			LCD_WriteString("Guest Mode");
			num=0;
			s=NO_KEY;
			count=0;
			LCD_GoTo(0,0);
			LCD_WriteString("1)Room1  2)Room2 ");
			LCD_GoTo(1,0);
			LCD_WriteString("3)Room3  4)Room4");
			while (Timeout_Flag==1)
			{
				SPI_SendReceive('s');
				_delay_us(500);
				Timeout();
				num=0;
				s=NO_KEY;
				count=0;
				if (Count_Flag>=Owner_CalTime)
				{
					Timer1_OCA_InterruptDisable();
					Timeout_Flag=0;
					Count_Flag=0;
					DIO_WRitePin(Guest_Led,LOW);
					LCD_Clear();
					LCD_GoTo(0,13);
					LCD_WriteString("Session Timeout!!");
					_delay_ms(1000);
				}
				while (count<1 && Timeout_Flag==1)
				{
					if (Count_Flag>=Owner_CalTime)
					{
						Timer1_OCA_InterruptDisable();
						Timeout_Flag=0;
						Count_Flag=0;
						DIO_WRitePin(Guest_Led,LOW);
						LCD_Clear();
						LCD_GoTo(0,13);
						LCD_WriteString("Session Timeout!!");
						SPI_SendReceive('d');
						_delay_ms(1000);
					}
					s=KEYPAD_GetKey();
					if (s!=NO_KEY)
					{
						if (s<='4'&& s>='1')
						{
							num=num*10+(s-'0');
							count++;
							SPI_SendReceive(num);
							a=num;
						}
						else
						{
							LCD_Clear();
							LCD_GoTo(0,15);
							LCD_WriteString("Wrong Input");
							_delay_ms(1000);
							LCD_GoTo(0,0);
							LCD_WriteString("1)Room1  2)Room2 ");
							LCD_GoTo(1,0);
							LCD_WriteString("3)Room3  4)Room4");
						}
					}
				}
				Send_f(Guest);
			}
		}
		else
		{
			DIO_WRitePin(Block_Led,HIGH);
			num=0;
			count=0;
			P_count=Password_Count;
			Blocking_Flag=1;
			LCD_Clear();
			LCD_WriteString("The System In Blocking Mode");
			LCD_GoTo(1,0);
			LCD_WriteString("Blocking Time is 20 s");
			Timeout();
			while (Timeout_Flag)
			{
				if (Count_Flag>=Blocking_CalTime)
				{
					Timer1_OCA_InterruptDisable();
					DIO_WRitePin(Block_Led,LOW);
					Timeout_Flag=0;
					Count_Flag=0;
				}
			}
		}
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

void Send_f(u8 f)
{
	while (Timeout_Flag==1 && s!='0')
	{
		if (Count_Flag>=Owner_CalTime)
		{
			Timer1_OCA_InterruptDisable();
			Timeout_Flag=0;
			Count_Flag=0;
			DIO_WRitePin(Owner_Led,LOW);
			DIO_WRitePin(Guest_Led,LOW);
			LCD_Clear();
			LCD_GoTo(0,13);
			LCD_WriteString("Session Timeout!!");
			_delay_ms(1000);
		}
		LCD_Clear();
		status=SPI_SendReceive('c');
		_delay_ms(1);
		if (a==6)
		{
			if (ON_Flag)
			status=1;
			else 
			status=0;
		}
		if (status==1)
		{
			count=0;
			if (f==0)
			{
				LCD_GoTo(0,30);
				LCD_WriteString("Owner Mode");
			}
			else
			{
				LCD_GoTo(0,30);
				LCD_WriteString("Guest Mode");
			}
			LCD_GoTo(0,0);
			if (a==5)
			{
			    LCD_WriteString("TV");
				LCD_WriteString(" is ON:-");
				LCD_GoTo(1,0);
				LCD_WriteString("1)ON   2)OFF   0)RET");  	
			}
			else if (a==6)
			{
				LCD_WriteString("Air Condition");
				LCD_WriteString(" is ON:-");
				LCD_GoTo(1,0);
				LCD_WriteString("1)Set Temperature  2)ON  3)OFF  0)RET");
				Air_Flag=1;
			}
			else
			{
				LCD_WriteString("Room");
				LCD_WriteNumber (a);
				LCD_WriteString(" is ON:-");
				LCD_GoTo(1,0);
				LCD_WriteString("1)ON   2)OFF   0)RET");
			}	
		}
		else if (status==0)
		{
			count=0;
			if (f==0)
			{
				LCD_GoTo(0,30);
				LCD_WriteString("Owner Mode");
			}
			else
			{
				LCD_GoTo(0,30);
				LCD_WriteString("Guest Mode");
			}
			LCD_GoTo(0,0);
			if (a==5)
			{
				LCD_WriteString("TV");
				LCD_WriteString(" is OFF:-");
				LCD_GoTo(1,0);
				LCD_WriteString("1)ON   2)OFF   0)RET");
			}
			else if (a==6)
			{
				LCD_WriteString("Air Condition");
				LCD_WriteString(" is OFF:-");
				LCD_GoTo(1,0);
				LCD_WriteString("1)Set Temperature  2)ON  3)OFF  0)RET");
				Air_Flag=1;
			}
			else
			{
				LCD_WriteString("Room");
				LCD_WriteNumber (a);
				LCD_WriteString(" is OFF:-");
				LCD_GoTo(1,0);
				LCD_WriteString("1)ON   2)OFF   0)RET");
			}
		}
		ON_OFF_f(f,Air_Flag);
		if ((s=='1'||s=='2'||s=='3'))
		{
			_delay_ms(1);
			SPI_SendReceive('d');
			_delay_ms(1);
			SPI_SendReceive('s');
			_delay_ms(1);
			SPI_SendReceive(a);
			_delay_ms(1);
		}
	}
}

void ON_OFF_f (u8 f, u8 af)
{
	while (count<1 && Timeout_Flag==1)
	{
		if (Count_Flag>=Owner_CalTime)
		{
			Timer1_OCA_InterruptDisable();
			Timeout_Flag=0;
			Count_Flag=0;
			DIO_WRitePin(Owner_Led,LOW);
			DIO_WRitePin(Guest_Led,LOW);
			LCD_Clear();
			LCD_GoTo(0,13);
			LCD_WriteString("Session Timeout!!");
			_delay_ms(1000);
		}
		
		if (af==1)
		{
			s=KEYPAD_GetKey();
			if (s!=NO_KEY)
			{
				if (s=='1'|| s=='2' || s=='0'|| s=='3')
				{
					if (s=='1')
					{
						u8 num=0,m=15;
						SPI_SendReceive('m');
						_delay_ms(1);
						LCD_Clear();
						LCD_WriteString("Enter Temperature");
						LCD_GoTo(1,15);
						while (s!='=')
						{
							s=KEYPAD_GetKey();
							if (s!=NO_KEY)
							{
								
								if (s<='9'&& s>='0')
								{
									num=num*10+(s-'0');
									LCD_WriteChar(s);
									m++;
								}
								else if (s=='c')
								{
									num=num/10;
									LCD_GoTo(1,m-1);
									LCD_WriteChar(' ');
									LCD_GoTo(1,m-1);
								}
							}
						}
						SPI_SendReceive(num);
						_delay_ms(1);
						s='1';	
						count++;	
					}
					else if (s=='2')
					{
						SPI_SendReceive('n');
						_delay_ms(1);
						ON_Flag=1;
						count++;
					}
					else if (s=='0')
					{
						SPI_SendReceive('d');
						_delay_ms(1);
						LCD_Clear();
					    LCD_GoTo(0,0);
					    LCD_WriteString("1)Room1  2)Room2  3)Room3");
					    LCD_GoTo(1,0);
					    LCD_WriteString("4)Room4  5)TV  6)Air Condition");	
						Air_Flag=0;
						count++;
					}
					else if (s=='3')
					{
						SPI_SendReceive('f');
						_delay_ms(1);
						ON_Flag=0;
						count++;	
					}
				}
				else 
				{
					wrong(a,Owner);
				}
			}
		}
		
		else if (af==0)
		{
			s=KEYPAD_GetKey();
			if (s!=NO_KEY)
			{
				if (s=='1'|| s=='2' || s=='0')
				{
					if (s=='1')
					{
						SPI_SendReceive('n');
						_delay_ms(1);
						count++;
					}
					else if (s=='2')
					{
						SPI_SendReceive('f');
						_delay_ms(1);
						count++;
					}
					else if (s=='0')
					{
						count++;
						SPI_SendReceive('d');
						LCD_Clear();
						if (f==0)
						{
							LCD_GoTo(0,0);
							LCD_WriteString("1)Room1  2)Room2  3)Room3");
							LCD_GoTo(1,0);
							LCD_WriteString("4)Room4  5)TV  6)Air Condition");
						}
						else
						{
							LCD_GoTo(0,0);
							LCD_WriteString("1)Room1  2)Room2 ");
							LCD_GoTo(1,0);
							LCD_WriteString("3)Room3  4)Room4");
						}
					}
					}
					else 
					{
						if (f==0)
						{
							wrong(a,Owner);
						}
						else 
						{
							wrong(a,Guest);
						}
					}
		        }
				
		
			}
		}
	}

void wrong (u8 num, u8 detect_mode)
{
	LCD_Clear();
	LCD_GoTo(0,15);
	LCD_WriteString("Wrong Input");
	LCD_Clear();
	if (detect_mode==0)
 {
	 LCD_GoTo(0,30);
	 LCD_WriteString("Owner Mode");
	 LCD_GoTo(0,0);
	switch (num)
	{
		case 6:
		if (status==1)
		{
     		LCD_WriteString("Air Condition");
     		LCD_WriteString(" is ON:-");
     		LCD_GoTo(1,0);
     		LCD_WriteString("1)Set Temperature  2)ON  3)OFF  0)RET");
		}
		else 
		{
			LCD_WriteString("Air Condition");
			LCD_WriteString(" is OFF:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)Set Temperature  2)ON  3)OFF  0)RET");
		}
		break;
		case 5:
		if (status==1)
		{
			LCD_WriteString("TV");
			LCD_WriteString(" is ON:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)ON   2)OFF   0)RET");
		}
		else
		{
			LCD_WriteString("TV");
			LCD_WriteString(" is OFF:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)ON   2)OFF   0)RET");
		}
		break;
		default:
		if (status==1)
		{
			LCD_WriteString("Room");
			LCD_WriteNumber (num);
			LCD_WriteString(" is ON:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)ON   2)OFF   0)RET");
		}
		else
		{
			LCD_WriteString("Room");
			LCD_WriteNumber (num);
			LCD_WriteString(" is OFF:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)ON   2)OFF   0)RET");
		}
		
	}
  }
	else 
	{
		LCD_GoTo(0,30);
		LCD_WriteString("Guest Mode");
		LCD_GoTo(0,0);
		if (status==1)
		{
			LCD_WriteString("Room");
			LCD_WriteNumber (num);
			LCD_WriteString(" is ON:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)ON   2)OFF   0)RET");
		}
		else
		{
			LCD_WriteString("Room");
			LCD_WriteNumber (num);
			LCD_WriteString(" is OFF:-");
			LCD_GoTo(1,0);
			LCD_WriteString("1)ON   2)OFF   0)RET");
		}
	}
}