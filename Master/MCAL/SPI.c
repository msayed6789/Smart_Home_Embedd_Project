/*
 * SPI.c
 *
 * Created: 12/29/2022 6:40:39 AM
 *  Author: M.Sayed
 */ 
#include "SPI.h"
#include "DIO_Interface.h"
/***********************Pointer to Function*******************/
static void (*SPI_ptr)(void)=NULLPTR;
/*************************************************************/
void SPI_Init(MasterSlave_type mode,SPI_Frequency_en frq,SPI_Datafirst_en data)
{
	//master or slave
	//clock in case of master
	if (mode==Master)
	{
		//DIO_WRitePin(PIND0,HIGH);
		Set_Bit(SPCR,MSTR);
		SPCR&=0xfc;
		SPCR|=frq;
	}
	else 
	{
		CLR_Bit(SPCR,MSTR);
	}
	if (data==MSB)
	{
		CLR_Bit(SPCR,DORD);
	}
	else
	{
		Set_Bit(SPCR,DORD);
	}
	//enable
	Set_Bit(SPCR,SPE);
}

u8 SPI_SendReceive(u8 data)
{
	SPDR=data;
	//DIO_WRitePin(PIND1,HIGH);
	while (!Read_Bit(SPSR,SPIF));
	//DIO_WRitePin(PIND2,HIGH);
	return SPDR;
}

void SPI_Send(u8 data)
{
	SPDR=data;
}

u8 SPI_ReceivePerodic(u8* pdata)
{
	if (Read_Bit(SPSR,SPIF)==1)
	{
		*pdata=SPDR;
		return 1;
	}
	return 0;
}

u8 SPI_Receive(void)//in case of Slave & in Interrupt
{
	return SPDR;
}

void SPI_InterruptEnable(void)
{
	Set_Bit(SPCR,SPIE);
}
void SPI_InterruptDisable(void)
{
	CLR_Bit(SPCR,SPIE);
}
void SPI_SetcallBack(void (*ptr)(void))
{
	SPI_ptr=ptr;
}

ISR (SPI_STC_vect)
{
	if (SPI_ptr!=NULLPTR)
	SPI_ptr();
}
