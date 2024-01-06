/*
 * SPI.h
 *
 * Created: 12/29/2022 6:40:52 AM
 *  Author: M.Sayed
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "MEMMAP.h"
#include "Utlise.h"
#include "Standard_Type.h"

typedef enum {
	Master,
	Slave
}MasterSlave_type;

typedef enum{
	Frequancy_4,
	Frequancy_16,
	Frequancy_64,
	Frequancy_128,
}SPI_Frequency_en;

typedef enum{
	MSB,
	LSB
}SPI_Datafirst_en;

void SPI_Init(MasterSlave_type mode,SPI_Frequency_en frq,SPI_Datafirst_en data);
u8 SPI_SendReceive(u8 data);
void SPI_Send(u8 data);
u8 SPI_ReceivePerodic(u8* pdata);
u8 SPI_Receive(void);
void SPI_InterruptEnable(void);
void SPI_InterruptDisable(void);
void SPI_SetcallBack(void (*ptr)(void));






#endif /* SPI_H_ */