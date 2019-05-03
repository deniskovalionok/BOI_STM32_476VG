//eeprom.h

#ifndef _EEPROM_H
#define _EEPROM_H


#include "types.h"

#include "stm32l4xx_hal.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Write Protect: When the WP pin is asserted, all sectors specified for protection by the Sector
Protection Register will be protected against program and erase operations regardless of whether
the Enable Sector Protection command has been issued or not. The WP pin functions
independently of the software controlled protection method. After the WP pin goes low, the
content of the Sector Protection Register cannot be modified.
If a program or erase command is issued to the device while the WP pin is asserted, the device
will simply ignore the command and perform no operation. The device will return to the idle state
once the CS pin has been deasserted. The Enable Sector Protection command and Sector
Lockdown command, however, will be recognized by the device when the WP pin is asserted.
The WP pin is internally pulled-high and may be left floating if hardware controlled protection will
not be used. However, it is recommended that the WP pin also be externally connected to VCC
whenever possible.
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Reset: A low state on the reset pin (RESET) will terminate the operation in progress and reset
the internal state machine to an idle state. The device will remain in the reset condition as long as
a low level is present on the RESET pin. Normal operation can resume once the RESET pin is
brought back to a high level.
The device incorporates an internal power-on reset circuit, so there are no restrictions on the
RESET pin during power-on sequences. If this pin and feature are not utilized it is recommended
that the RESET pin be driven high externally.
*/




//len for sector addres for adressing
#define SECTOR_LEN_TRUE 512
//len of whole data including CRC
#define SECTOR_LEN 264
//len of data part of sector
#define SECTOR_DATA_LEN 256
//start adr, start from second sector, first one will be busy
#define EEPROM_ADDR 0x000000
//fifo buf size in eeprom
#define FIFOSIZE 8



//max number of sectors for this type of eeprom
#define MAX_SECTORS 32768





struct tagEEPROMControl
{
	BYTE flashBuffer[SECTOR_LEN];
	//these buffers must not be changed when eeprom is busy
	BYTE arTrmBuffer[SECTOR_LEN+8];
	BYTE arRcvBuffer[SECTOR_LEN+8];
	DWORD dwTrmLen;
	DWORD dwTrmIdx;
	DWORD dwRcvLen;
	DWORD dwRcvIdx;
        
        GPIO_TypeDef *wp_port;
        uint16_t wp_pin;
        GPIO_TypeDef *cs_port;
        uint16_t cs_pin;

        SPI_HandleTypeDef *spi_port;

//add 24/09/2010	
	WORD wdEepromWritesCounter;//counter of eeprom writes. it is saved in time of device power off only
	
};

extern struct tagEEPROMControl EEPROMControl;























void EEPROM_Init(struct tagEEPROMControl *eepromc, SPI_HandleTypeDef *spi_port, 
                 GPIO_TypeDef *wp_port, GPIO_TypeDef *cs_port, 
                 uint16_t wp_pin, uint16_t cs_pin);

BOOL EEPROM_Busy(struct tagEEPROMControl *eepromc);

//BOOL EEPROM_SSP_Busy(void);


//DWORD EEPROM_WaitFlash (void);
//WORD EEPROM_calcCRC(BYTE * pBuffer, WORD length);
void EEPROM_WriteBlok (struct tagEEPROMControl *eepromc, unsigned int count,   // Длина
                const unsigned char  * RAM,     // Адрес в RAM
                DWORD EEPROM  // Адрес в EEPROM
                );    // Разрешение записи CRC
void EEPROM_ReadBlok (struct tagEEPROMControl *eepromc, unsigned int  count,   // Длина
              unsigned char  * RAM,     // Адрес в RAM
              DWORD EEPROM);  // Адрес в EEPROM
void EEPROM_ReadFlash(struct tagEEPROMControl *eepromc, int SectorNo, unsigned char * SectorBuffer);
void EEPROM_WriteFlash(struct tagEEPROMControl *eepromc, int SectorNo, const unsigned char * SectorBuffer);
void EEPROM_WriteSector(struct tagEEPROMControl *eepromc, int SectorNo, const unsigned char* SectorBuf);
BOOL EEPROM_ReadSector(struct tagEEPROMControl *eepromc, const int SectorNo, unsigned char* SectorBuf);
void EEPROM_powerOFFSSP(void);
void EEPROM_powerONSSP(void);


void EEPROM_StartTrm(struct tagEEPROMControl *eepromc, BOOL bReceive);
void EEPROM_clearFIFO(struct tagEEPROMControl *eepromc);

BYTE EEPROM_ReadStatus(struct tagEEPROMControl *eepromc);


void EEPROM_WriteSystemSector(struct tagEEPROMControl *eepromc, const unsigned char * SectorBuffer);
void EEPROM_ReadSystemSector(struct tagEEPROMControl *eepromc, unsigned char * SectorBuffer);
void EEPROM_RegenerateFlash(struct tagEEPROMControl *eepromc, int SectorNo);
void EEPROM_chipErase(struct tagEEPROMControl *eepromc);

void EEPROM_UpdateEssentialDataInEeprom(struct tagEEPROMControl *eepromc);

WORD EEPROM_CRC (BYTE Sbyte, WORD CRCbuf);

int EEPROM_calcCRC(BYTE volatile * pData, int len);

#endif	//#ifndef _EEPROM_H