
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "exeptions.h"

#include "eeprom.h"

extern SPI_HandleTypeDef hspi3;
struct tagEEPROMControl EEPROMControl;

BYTE SectBuff[SECTOR_LEN];

BOOL flag_EEPROM_Err;


/* Table of CRC values for high-order byte */
   const unsigned char auchCRCHi1[] = {
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
   0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
   0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
   0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
   0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
   0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
   0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
   0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
   } ;

   /* Table of CRC values for low-order byte */
   const unsigned char auchCRCLo1[] = {
   0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
   0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
   0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
   0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
   0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
   0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
   0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
   0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
   0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
   0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
   0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
   0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
   0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
   0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
   0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
   0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
   0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
   0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
   0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
   0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
   0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
   0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
   0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
   0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
   0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
   0x43, 0x83, 0x41, 0x81, 0x80, 0x40
   } ;
   
WORD EEPROM_CRC (BYTE Sbyte, WORD CRCbuf)
{
	unsigned char uIndex ;			
	uIndex = (LO2BYTE(CRCbuf)) ^ Sbyte ;	
	*((unsigned char *)&CRCbuf+1) = (unsigned char)(LOBYTE(CRCbuf)) ^ auchCRCHi1[uIndex] ;
	*((unsigned char *)&CRCbuf) = auchCRCLo1[uIndex] ;
	return CRCbuf;
}

int EEPROM_calcCRC(BYTE volatile * pData, int len)
{
	WORD CRCbuf = 0xffff;
	int i = 0;
	for(; i<len; i++)
	{
		CRCbuf = EEPROM_CRC(pData[i], CRCbuf);
	}
	pData[i++] = LOBYTE(CRCbuf);
	pData[i++] = LO2BYTE(CRCbuf);
	return i;
}

BOOL EEPROM_checkCRC(const BYTE * pData, int len)
{
	BYTE bt1,bt2;
	WORD CRCbuf = 0xffff;
	int i = 0;
	for(; i<len-2; i++)
	{
		CRCbuf = EEPROM_CRC(pData[i], CRCbuf);
	}
	bt1 = pData[len-1];
	bt2 = pData[len-2];
	return (CRCbuf==((bt1<<8)|(bt2)));
}


void EEPROM_Init(struct tagEEPROMControl *eepromc, SPI_HandleTypeDef *spi_port, 
                 GPIO_TypeDef *wp_port, GPIO_TypeDef *cs_port, 
                 uint16_t wp_pin, uint16_t cs_pin)
{
        eepromc->spi_port = spi_port;
        eepromc->wp_port = wp_port;
        eepromc->cs_port = cs_port;
        eepromc->wp_pin = wp_pin;
        eepromc->cs_pin = cs_pin;
        HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(eepromc->cs_port, eepromc->cs_pin, GPIO_PIN_SET);
        
}

//clear fifo buf in eeprom
void EEPROM_clearFIFO(struct tagEEPROMControl *eepromc)
{
    //Chip_SSP_Int_FlushData(LPC_SSP1);
  HAL_SPIEx_FlushRxFifo(eepromc->spi_port);
}



//read one sector from eeprom with chech crc
BOOL EEPROM_ReadSector(struct tagEEPROMControl *eepromc, const int SectorNo, unsigned char* SectorBuf)
{
	//26/12/2011
	//добавлено перечитывание при сбое чтения
	BOOL ret;
	for(int i=0;i<3;i++)
	{
		EEPROM_ReadFlash(eepromc, SectorNo, eepromc->flashBuffer);
		if(!SectorBuf)return 1;
		memcpy(SectorBuf, &eepromc->flashBuffer[0], SECTOR_DATA_LEN);
		ret = EEPROM_checkCRC((BYTE*)&eepromc->flashBuffer[0], 258);
		if(ret)
                  break;
	}
	return ret;
}
//write sector to eeprom with chech crc
void EEPROM_WriteSector(struct tagEEPROMControl *eepromc, int SectorNo, const unsigned char* SectorBuf)
{
	memcpy(&eepromc->flashBuffer[0], SectorBuf, SECTOR_DATA_LEN);
	EEPROM_calcCRC((BYTE*)&eepromc->flashBuffer[0], 256);
	EEPROM_WriteFlash(eepromc, SectorNo, &eepromc->flashBuffer[0]);
}


//write sector to eeprom with no chech crc
void EEPROM_WriteFlash(struct tagEEPROMControl *eepromc, int SectorNo, const unsigned char * SectorBuffer)
{
	EEPROM_WriteBlok(eepromc, SECTOR_LEN, SectorBuffer,
		(ULONG)EEPROM_ADDR+(ULONG)SectorNo*(ULONG)SECTOR_LEN_TRUE);
}

//read one sector from eeprom with no chech crc
void EEPROM_ReadFlash(struct tagEEPROMControl *eepromc, int SectorNo, unsigned char * SectorBuffer)
{
	EEPROM_ReadBlok (eepromc, SECTOR_LEN,   // Длина
      SectorBuffer,     // Адрес в RAM
      (ULONG)EEPROM_ADDR+(ULONG)SectorNo*(ULONG)SECTOR_LEN_TRUE);
}

//write system sector to eeprom with no chech crc
void EEPROM_WriteSystemSector(struct tagEEPROMControl *eepromc, const unsigned char * SectorBuffer)
{
	EEPROM_WriteBlok(eepromc, SECTOR_LEN, SectorBuffer,
		(ULONG)0);
}

//read system sector from eeprom with no chech crc
void EEPROM_ReadSystemSector(struct tagEEPROMControl *eepromc, unsigned char * SectorBuffer)
{
	EEPROM_ReadBlok (eepromc, SECTOR_LEN,   // Длина
      SectorBuffer,     // Адрес в RAM
      (ULONG)0);
}


//regeneration of sector
void EEPROM_RegenerateFlash(struct tagEEPROMControl *eepromc, int SectorNo)
{
	DWORD EEPROM = (ULONG)EEPROM_ADDR+(ULONG)SectorNo*(ULONG)SECTOR_LEN_TRUE;
	eepromc->arTrmBuffer[0]=0x58;
	eepromc->arTrmBuffer[1]=*((unsigned char *)&EEPROM+2);
	eepromc->arTrmBuffer[2]=*((unsigned char *)&EEPROM+1);
	eepromc->arTrmBuffer[3]=*((unsigned char *)&EEPROM+0);
	eepromc->dwTrmLen = 4;
        HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_RESET);
	EEPROM_StartTrm(eepromc, 0);
        HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_SET);
}

void EEPROM_chipErase(struct tagEEPROMControl *eepromc)
{
	eepromc->arTrmBuffer[0]=0xc7;
	eepromc->arTrmBuffer[1]=0x94;
	eepromc->arTrmBuffer[2]=0x80;
	eepromc->arTrmBuffer[3]=0x9a;
	eepromc->dwTrmLen = 4;
        HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_RESET);
	EEPROM_StartTrm(eepromc, 0);
        HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_SET);
        
        while(EEPROM_Busy(eepromc));
}

//read a block of memory in one sector
void EEPROM_ReadBlok (struct tagEEPROMControl *eepromc, unsigned int         count,   // Длина
              unsigned char  * RAM,     // Адрес в RAM
              DWORD EEPROM)  // Адрес в EEPROM
{
	eepromc->arTrmBuffer[0]=0xD2;
	eepromc->arTrmBuffer[1]=*((unsigned char *)&EEPROM+2);
	eepromc->arTrmBuffer[2]=*((unsigned char *)&EEPROM+1);
	eepromc->arTrmBuffer[3]=*((unsigned char *)&EEPROM+0);
	eepromc->arTrmBuffer[4]=0xff;
	eepromc->arTrmBuffer[5]=0xff;
	eepromc->arTrmBuffer[6]=0xff;
	eepromc->arTrmBuffer[7]=0xff;
	eepromc->dwTrmLen = 8;
	eepromc->dwRcvLen = count;
	EEPROM_StartTrm(eepromc, 1);

	if(RAM)
		for(UINT i=0;i<count;i++)
			RAM[i] = eepromc->arRcvBuffer[i];
}



void EEPROM_WriteBlok (struct tagEEPROMControl *eepromc, unsigned int count,   // Длина
                const unsigned char  * RAM,     // Адрес в RAM
                DWORD EEPROM  // Адрес в EEPROM
                )    // Разрешение записи CRC

{
	eepromc->arTrmBuffer[0] = 0x53;
	eepromc->arTrmBuffer[1] = *((unsigned char *)&EEPROM+2);
	eepromc->arTrmBuffer[2] = *((unsigned char *)&EEPROM+1);
	eepromc->arTrmBuffer[3] = *((unsigned char *)&EEPROM+0);
	eepromc->dwTrmLen = 4;
	EEPROM_StartTrm(eepromc, 0);
		
	eepromc->arTrmBuffer[0] = 0x82;
	eepromc->arTrmBuffer[1] = *((unsigned char *)&EEPROM+2);
	eepromc->arTrmBuffer[2] = *((unsigned char *)&EEPROM+1);
	eepromc->arTrmBuffer[3] = *((unsigned char *)&EEPROM+0);
	
	for(UINT i=0;i<count;i++)
		eepromc->arTrmBuffer[4+i]=RAM[i];
	
	eepromc->dwTrmLen = count+4;
        
        HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_RESET);
	EEPROM_StartTrm(eepromc, 0);
	HAL_GPIO_WritePin(eepromc->wp_port, eepromc->wp_pin, GPIO_PIN_SET);
        
	//add 24/09/2010
	eepromc->wdEepromWritesCounter++;

}



//read eeprom status
BYTE EEPROM_ReadStatus(struct tagEEPROMControl *eepromc)
{
	EEPROM_clearFIFO(eepromc);
        HAL_GPIO_WritePin(eepromc->cs_port, eepromc->cs_pin, GPIO_PIN_RESET);
        BYTE cmd = 0xd7;

        while(HAL_SPI_Transmit(eepromc->spi_port, &cmd, 1, 1000) != HAL_OK)
        {
                  HAL_SPI_DeInit(eepromc->spi_port);
                  HAL_SPI_Init(eepromc->spi_port);
        }
	
	BYTE status1[2];
        while(HAL_SPI_Receive(eepromc->spi_port, status1, 2, 1000) != HAL_OK)
        {
                  HAL_SPI_DeInit(eepromc->spi_port);
                  HAL_SPI_Init(eepromc->spi_port);
        }
	
        HAL_GPIO_WritePin(eepromc->cs_port, eepromc->cs_pin, GPIO_PIN_SET);
	return status1[1];
}


//ret TRUE is eeprom is busy
BOOL EEPROM_Busy(struct tagEEPROMControl *eepromc)
{
	return !(EEPROM_ReadStatus(eepromc)&0x80);
}


//start transmition data to eeprom
//and receiving data if bReceive is set to 1
void EEPROM_StartTrm(struct tagEEPROMControl *eepromc, BOOL bReceive)
{
	//wait while busy
        /*                      Включить когда настроим таймеры. Защита от порчи EEPROM
	timers.stop12 = SECOND_5;
        timers.period12_Ready = FALSE;
        timers.period12 = RUN_TIMER;
	while(EEPROM_Busy())
        {
          if(timers.period12_Ready)
          {
            flag_EEPROM_Err = TRUE;
            return;
          }
	}
	*/
        uint32_t tickstart = 0U;
        tickstart = HAL_GetTick();
        while(EEPROM_Busy(eepromc))
        {
          if(HAL_GetTick() > tickstart+5000)
          {
            exception(__FILE__,__FUNCTION__,__LINE__,"EEPROM fault");
            return;
          }
        }
  
	HAL_GPIO_WritePin(eepromc->cs_port, eepromc->cs_pin, GPIO_PIN_RESET);

        while(HAL_SPI_Transmit(eepromc->spi_port, &eepromc->arTrmBuffer[0], eepromc->dwTrmLen, 1000) != HAL_OK)
        {
                  HAL_SPI_DeInit(eepromc->spi_port);
                  HAL_SPI_Init(eepromc->spi_port);
        }
        /*
	for(int i=0;i<EEPROMControl.dwTrmLen;i++)
	{
	  	while(Chip_SSP_GetStatus(LPC_SSP1, SSP_STAT_TFE) == RESET);
		Chip_SSP_SendFrame(LPC_SSP1, *pBuf++);
	};
	*/
	
	if(bReceive)
	{
		EEPROM_clearFIFO(eepromc);
		//BYTE* pBuf = EEPROMControl.arRcvBuffer;

                while(HAL_SPI_Receive(eepromc->spi_port, eepromc->arRcvBuffer, eepromc->dwRcvLen, 1000) != HAL_OK)
                {
                  HAL_SPI_DeInit(eepromc->spi_port);
                  HAL_SPI_Init(eepromc->spi_port);
                }
                /*
		for(int i=0;i<EEPROMControl.dwRcvLen;i++)
		{
			Chip_SSP_SendFrame(LPC_SSP1, 0xff);
			while(Chip_SSP_GetStatus(LPC_SSP1, SSP_STAT_RNE) == RESET);
			*pBuf++ = Chip_SSP_ReceiveFrame(LPC_SSP1);
		};
                */
		
	}
	
	HAL_GPIO_WritePin(eepromc->cs_port, eepromc->cs_pin, GPIO_PIN_SET);
}




//update essential data in eeprom
void EEPROM_UpdateEssentialDataInEeprom(struct tagEEPROMControl *eepromc)
{
	memset(SectBuff, 0, sizeof(SectBuff));
	
	//19/03/2010
	EEPROM_ReadSector(eepromc, 0, &SectBuff[0]);
	
	eepromc->wdEepromWritesCounter++;
	////////////
	//первые 7 байт это серийный номер месяц и год выпуска
        //                                              Можно вернуть, если нужно будет
	//buf[0] = (BYTE)(modeControl.Serial&0xff);
	//buf[1] = (BYTE)((modeControl.Serial>>8)&0xff);
	//buf[2] = (BYTE)((modeControl.Serial>>16)&0xff);
	//buf[3] = (BYTE)((modeControl.Serial>>24)&0xff);
	//buf[4] = (BYTE)(modeControl.ManufacturedMonth);
	//buf[5] = (BYTE)(modeControl.ManufacturedYear&0xff);
	//buf[6] = (BYTE)((modeControl.ManufacturedYear>>8)&0xff);
        
	SectBuff[7] = (BYTE)(eepromc->wdEepromWritesCounter&0xff);
	SectBuff[8] = (BYTE)((eepromc->wdEepromWritesCounter>>8)&0xff);
	//buf[9] = (BYTE)modeControl.bLang;
        
	EEPROM_WriteSector(eepromc, 0, &SectBuff[0]);
}

