#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"
#include "eeprom.h"

struct tagFileSystem filesystem;

BYTE nameCRCBuff[FILE_NAME_SZ];

struct tagInfo infoRegs;

#include "exeptions.h"


int cn, cne;

extern BYTE SectBuff[SECTOR_LEN];

void filesystem_Init(struct tagFileSystem *fsys, struct tagEEPROMControl *eepromc)
{

        for(int i = 0; i < FILERECORD_RECORDS_ON_SECTOR; i++)
        {
          fsys->fileRecordsOnSector[i].dateTime.dayOfMonth = 28;
          fsys->fileRecordsOnSector[i].dateTime.hour = 12;
          fsys->fileRecordsOnSector[i].dateTime.minute = 30;
          fsys->fileRecordsOnSector[i].dateTime.month = 6;
          fsys->fileRecordsOnSector[i].dateTime.second = 45;
          fsys->fileRecordsOnSector[i].dateTime.year = 1993;
        }
        
	//mean current sector of claster table if 0xff then no claster table was loaded
	//if -1 then no file record was loaded
	fsys->iCurrentClasterTableSector = -1;	//nothing loaded by default
	//mean current sector of file record
	//if -1 then no file record was loaded
	fsys->iCurrentFileRecordSector = -1;	//nothing loaded by default
	fsys->iCurrentFileClaster = -1;
	fsys->iCurrentFileClasterSector = -1;
        
	fsys->bMemoryLow = FALSE;
        
	fsys->file_pos_last_value = -1;
	fsys->file_pos_last_section = -1;
	memset(fsys->lastSectionName,0 , sizeof(fsys->lastSectionName));
	fsys->hFileExecuteSys = NULL;
        
	//here we read internal non erasable data with serial number and so on.
	//get serial number
	/*
	7 байт используютс¤ дл¤ серийного номера мес¤ца и года выпуска
	2 байта дл¤ счетчика перезаписей еепром
	1 байт дл¤ призначка ¤зыка интерфейса пользовател¤
	*/
        
        fsys->eepromc = eepromc;

        
	if(EEPROM_ReadSector(fsys->eepromc, 0, &SectBuff[0]))
	{//yes we have it
		infoRegs.Serial = ((uint32_t)SectBuff[3]<<24)|((uint32_t)SectBuff[2]<<16)|((uint32_t)SectBuff[1]<<8)|((uint32_t)SectBuff[0]);
		infoRegs.ManufacturedMonth = SectBuff[4];
		infoRegs.ManufacturedYear = ((uint16_t)SectBuff[6]<<8)|((uint16_t)SectBuff[5]);
		//add 24/09/2010
		fsys->eepromc->wdEepromWritesCounter = ((uint16_t)SectBuff[8]<<8)|((uint16_t)SectBuff[7]);
		//modeControl.bLang = (int)buf[9];
                infoRegs.softwareVer = ((uint32_t)SectBuff[13]<<24)|((uint32_t)SectBuff[12]<<16)|((uint32_t)SectBuff[11]<<8)|((uint32_t)SectBuff[10]);
                infoRegs.hardwareVer = ((uint32_t)SectBuff[17]<<24)|((uint32_t)SectBuff[16]<<16)|((uint32_t)SectBuff[15]<<8)|((uint32_t)SectBuff[14]);
                infoRegs.errors = 0x0000;
	}
        else
          infoRegs.errors = 0x0001;
        
        /*
        else 
	{//set settings by default in RAM
		if(!modeControl.bNoSysSettings)
		{
			modeControl.bNeedSetTrue_NoSysSettings = TRUE;
		}
//		MCA.usREGs[0xA7-0x80] = 600;	//default contrast
	}
        */
        
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//filerecord work

//ret index of record in fileRecordsOnSector
//if E_FAIL then error
//unsafe function for read claster, if it is bad then no exception
int filesystem_get_filerecordIndex(struct tagFileSystem *fsys, int filerecord)
{
        //fsys->ig = FILES_CLASTER_AREA_LEN;
	if(filerecord<FILERECORD_START_NO || filerecord>FILERECORD_END_NO)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"filerecord is out of range");
		return E_FAIL;
	}
	int sector_num = FILERECORD_START_SECTOR_NO+filerecord/FILERECORD_RECORDS_ON_SECTOR;
	BOOL ret = filesystem_read_filerecordstable(fsys, sector_num);
	int index;
	if(ret)
		index = filerecord%FILERECORD_RECORDS_ON_SECTOR;
	else//error
		index = E_FAIL;
	return index;
}

//read a part of file records table by sector number
//ret 0 if no file records at this sector
//else 1
BOOL filesystem_read_filerecordstable(struct tagFileSystem *fsys, int sector_num)
{
	if(sector_num<FILERECORD_START_SECTOR_NO || sector_num>=FILERECORD_START_SECTOR_NO+FILERECORD_SECTOR_AREA_LEN+FILERECORD_SECTOR_AREA_LEN)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"sector number is out of range");
		return 0;
	}
	BOOL ret = 1;
	if(fsys->iCurrentFileRecordSector != sector_num)
	{
		ret = EEPROM_ReadSector(fsys->eepromc, sector_num, (BYTE*)&fsys->fileRecordsOnSector[0]);
		if(!ret)
		{//сбой основной копии таблицы файлов. восстановление из резерва
			ret = EEPROM_ReadSector(fsys->eepromc, sector_num+FILERECORD_SECTOR_AREA_LEN, (BYTE*)&fsys->fileRecordsOnSector[0]);
			if(ret)
				EEPROM_WriteSector(fsys->eepromc, sector_num,(BYTE*)&fsys->fileRecordsOnSector[0]);
			else
				;//сбой резерва таблицы файлов
		}
		if(ret)
			fsys->iCurrentFileRecordSector = sector_num;
	}
	return ret;
}


//write a part of file records table by sector number in glob variables
BOOL filesystem_write_filerecordstable(struct tagFileSystem *fsys)
{
	if(fsys->iCurrentFileRecordSector<FILERECORD_START_SECTOR_NO || fsys->iCurrentFileRecordSector>=FILERECORD_START_SECTOR_NO+FILERECORD_SECTOR_AREA_LEN)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"sector number is out of range");
		return 0;
	}
        //--------------------------Vorobey 2k18--------------------------------
        //write nameCRC of all files in this sector of filerecordstable
        BOOL CRCTabelRewr =FALSE;
        for(int i = 0; i<FILERECORD_RECORDS_ON_SECTOR; i++)
        {
          int index = filesystem_get_nameCRCIndex(fsys, (fsys->iCurrentFileRecordSector - FILERECORD_START_SECTOR_NO)*FILERECORD_RECORDS_ON_SECTOR+i);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
          }
          if(fsys->fileRecordsOnSector[i].wdStartClaster != CLASTERTABLE_EMPTY_RECORD)
          {
            WORD nameCRC = CalcNameCRC(fsys->fileRecordsOnSector[i].name);
            if(nameCRC != fsys->nameCRCTableOnSector[index])
            {
                CRCTabelRewr = TRUE;
                fsys->nameCRCTableOnSector[index] = nameCRC;
            }
          }
          else
          {
            if(fsys->nameCRCTableOnSector[index] != NAMECRCTABLE_EMPTY_RECORD)
            {
                CRCTabelRewr = TRUE;
                fsys->nameCRCTableOnSector[index] = NAMECRCTABLE_EMPTY_RECORD;
            }
          }
        }
        if(CRCTabelRewr)
          filesystem_write_nameCRCtable(fsys);
        //----------------------------------------------------------------------
        
        
	EEPROM_WriteSector(fsys->eepromc, fsys->iCurrentFileRecordSector,(BYTE*)&fsys->fileRecordsOnSector[0]);
	//запсиь в резерв
	EEPROM_WriteSector(fsys->eepromc, fsys->iCurrentFileRecordSector+FILERECORD_SECTOR_AREA_LEN,(BYTE*)&fsys->fileRecordsOnSector[0]);
	return 1;
}



//get file record table index with exception processing
//safe function, it is guarantee that index will be OK
int filesystem_get_first_filerecordIndex(struct tagFileSystem *fsys, HFILE hFile)
{
	int findex = hFile-1;
	int index = filesystem_get_filerecordIndex(fsys, findex);
	if(index==E_FAIL)
	{//error in filerecord, bad record in eeprom, all files of this records must be deleted
		exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
	}
	return index;
}

//find free record in filerecord table
//ret E_FAIL if no filerecords is free
//else filerecord index
int filesystem_find_free_filerecord(struct tagFileSystem *fsys)
{
	int k;
	for(k=FILERECORD_START_NO;k<=FILERECORD_END_NO;k++)
	{
		//PowerControl_kickWatchDog();
		int index = filesystem_get_filerecordIndex(fsys, k);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
		}else
		{//found
			if(fsys->fileRecordsOnSector[index].wdStartClaster==CLASTERTABLE_EMPTY_RECORD)break;
		}
	}
	if(k>FILERECORD_END_NO)
		return E_FAIL;
	else
		return k;
}

//filerecord work
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//claster work

//read a part of claster table
BOOL filesystem_read_clastertable(struct tagFileSystem *fsys, int sector)
{
	if((sector<(CLASTERTABLE_START_SECTOR_NO)) || (sector>=(CLASTERTABLE_START_SECTOR_NO+CLASTERTABLE_SECTOR_AREA_LEN)))
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"sector number is out of range");
		return 0;
	}
	BOOL ret = 1;
	if(fsys->iCurrentClasterTableSector != sector)
	{
		ret = EEPROM_ReadSector(fsys->eepromc, sector,(BYTE*)&fsys->clasterTableOnSector[0]);
		if(!ret)
		{
			//сбой в главной копии, восстановление из резерва
			ret = EEPROM_ReadSector(fsys->eepromc, sector+CLASTERTABLE_SECTOR_AREA_LEN,(BYTE*)&fsys->clasterTableOnSector[0]);
			if(ret)
				EEPROM_WriteSector(fsys->eepromc, sector,(BYTE*)&fsys->clasterTableOnSector[0]);
			else
				;//сбой резервной копии
		}
		if(ret)
		{
			fsys->iCurrentClasterTableSector = sector;
		}
	}
	return ret;
}


//write a part of claster table in sector that is in glob variables
BOOL filesystem_write_clastertable(struct tagFileSystem *fsys)
{
	if(fsys->iCurrentClasterTableSector<CLASTERTABLE_START_SECTOR_NO || fsys->iCurrentClasterTableSector>=CLASTERTABLE_START_SECTOR_NO+CLASTERTABLE_SECTOR_AREA_LEN)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"sector number is out of range");
		return 0;
	}
	EEPROM_WriteSector(fsys->eepromc, fsys->iCurrentClasterTableSector,(BYTE*)&fsys->clasterTableOnSector[0]);
	//дублировать запись сектора кластерной таблицы
	EEPROM_WriteSector(fsys->eepromc, fsys->iCurrentClasterTableSector+CLASTERTABLE_SECTOR_AREA_LEN,(BYTE*)&fsys->clasterTableOnSector[0]);
	return 1;
}

//read one claster of some file
BOOL filesystem_read_claster(struct tagFileSystem *fsys, int claster_number)
{
	if(claster_number<FILES_CLASTER_START_NO || claster_number>FILES_CLASTER_END_NO)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"claster number is out of range");
		return FALSE;
	}
	BOOL ret  = TRUE;
	if(claster_number!=fsys->iCurrentFileClaster)
	{//!!!!!!! здесь есть опасность что если надо перечитать один и тот же кластер, то это не будет сделано
		int sector = claster_number*CLASTER_SZ_IN_SECTORS;
		for(int i=0;i<CLASTER_SZ_IN_SECTORS;i++)
			ret &= EEPROM_ReadSector(fsys->eepromc, sector+i, &fsys->clasterBuffer[i*SECTOR_DATA_LEN]);
		if(ret)
		{
			fsys->iCurrentFileClaster = claster_number;
			fsys->iCurrentFileClasterSector = sector;
		}
	}
	return ret;
}

//write one claster of some file
BOOL filesystem_write_claster(struct tagFileSystem *fsys, int claster_number)
{
	if(claster_number<FILES_CLASTER_START_NO || claster_number>FILES_CLASTER_END_NO)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"claster number is out of range");
		return 0;
	}
	int sector = claster_number*CLASTER_SZ_IN_SECTORS;
	for(int i=0;i<CLASTER_SZ_IN_SECTORS;i++)
		EEPROM_WriteSector(fsys->eepromc, sector+i, &fsys->clasterBuffer[i*SECTOR_DATA_LEN]);
	fsys->iCurrentFileClaster = claster_number;
	fsys->iCurrentFileClasterSector = sector;
	return 1;
}

//ret index of claster in clasterTableOnSector
//if E_FAIL then error
int filesystem_get_clasterIndex(struct tagFileSystem *fsys, int claster_number)
{
	if(claster_number<FILES_CLASTER_START_NO || claster_number>FILES_CLASTER_END_NO)
		exception(__FILE__,__FUNCTION__,__LINE__,"claster number is out of range");
	int sector = CLASTERTABLE_START_SECTOR_NO+claster_number/CLASTERTABLE_RECORDS_ON_SECTOR;
	BOOL ret = filesystem_read_clastertable(fsys, sector);
	int index;
	if(ret)
		index = claster_number%CLASTERTABLE_RECORDS_ON_SECTOR;
	else//error
		index = E_FAIL;
	return index;
}

//find free claster in claster table for FILES ONLY
//ret E_FAIL if no claster is free
//else claster index
//notThisClaster used to pass this claster
//if notThisClaster==CLASTERTABLE_EMPTY_RECORD then procedure will take first free
int filesystem_find_free_claster(struct tagFileSystem *fsys, int notThisClaster)
{
	int k;
	for(k=FILES_CLASTER_START_NO;k<=FILES_CLASTER_END_NO;k++)
	{
		//PowerControl_kickWatchDog();
		int index = filesystem_get_clasterIndex(fsys, k);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
		}else
		{//found
			if(k!=notThisClaster && fsys->clasterTableOnSector[index]==CLASTERTABLE_EMPTY_RECORD)break;
		}
	}
	if(k>FILES_CLASTER_END_NO)k=E_FAIL;	//to make result NULL
	return k;
}
//claster work
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CRCtable work

//Calc CRC32 of filename. If rezult = 0xffff(0xffff = ampty filerecord) incriment first byte in name and calc agein.
WORD CalcNameCRC(const char* pFileName)
{
        memcpy(nameCRCBuff, pFileName, FILE_NAME_SZ);
  	WORD CRCbuf;
        do
        {
            CRCbuf = 0xffff;
            nameCRCBuff[0]++;
            for(int i = 0; i<FILE_NAME_SZ ; i++)
            {
		CRCbuf = EEPROM_CRC(pFileName[i], CRCbuf);
                if(pFileName[i] != '\n')
                    break;
            }
        }
        while(CRCbuf == NAMECRCTABLE_EMPTY_RECORD);
        return CRCbuf;
}

//read sector from nameCRCtable and write this to fsys.nameCRCTableOnSector.
BOOL filesystem_read_nameCRCtable(struct tagFileSystem *fsys, int sector)
{
	if((sector<(NAMECRCTABLE_START_SECTOR_NO)) || (sector>=(NAMECRCTABLE_START_SECTOR_NO+NAMECRCTABLE_SECTOR_AREA_LEN)))
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"sector number is out of range");
		return 0;
	}
	BOOL ret = 1;
	if(fsys->iCurrentNameCRCTableSector != sector)
	{
		ret = EEPROM_ReadSector(fsys->eepromc, sector,(BYTE*)&fsys->nameCRCTableOnSector[0]);
		if(!ret)
		{
			//сбой в главной копии, восстановление из резерва
			ret = EEPROM_ReadSector(fsys->eepromc, sector+NAMECRCTABLE_START_SECTOR_NO,(BYTE*)&fsys->nameCRCTableOnSector[0]);
			if(ret)
				EEPROM_WriteSector(fsys->eepromc, sector,(BYTE*)&fsys->nameCRCTableOnSector[0]);
			else
				;//сбой резервной копии
		}
		if(ret)
		{
			fsys->iCurrentNameCRCTableSector = sector;
		}
	}
	return ret;
} 

//write nameCRCtable from fsys.nameCRCTableOnSector to EEPROM.
// this function automaticly coled in filesystem_write_filerecordstable
BOOL filesystem_write_nameCRCtable(struct tagFileSystem *fsys)
{
	if(fsys->iCurrentNameCRCTableSector<NAMECRCTABLE_START_SECTOR_NO || fsys->iCurrentNameCRCTableSector>=NAMECRCTABLE_START_SECTOR_NO+NAMECRCTABLE_SECTOR_AREA_LEN)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"claster number is out of range");
		return 0;
	}
	EEPROM_WriteSector(fsys->eepromc, fsys->iCurrentNameCRCTableSector,(BYTE*)&fsys->nameCRCTableOnSector[0]);
	//дублировать запись сектора nameCRCtable
	EEPROM_WriteSector(fsys->eepromc, fsys->iCurrentNameCRCTableSector+NAMECRCTABLE_SECTOR_AREA_LEN,(BYTE*)&fsys->nameCRCTableOnSector[0]);
	return 1;
}

//ret index of record in fsys.nameCRCTableOnSector
//if E_FAIL then error
//unsafe function for read claster, if it is bad then no exception
int filesystem_get_nameCRCIndex(struct tagFileSystem *fsys, int file_number)
{
	if(file_number<FILERECORD_START_NO || file_number>FILERECORD_END_NO)
		exception(__FILE__,__FUNCTION__,__LINE__,"filerecord is out of range");
	int sector = NAMECRCTABLE_START_SECTOR_NO+file_number/NAMECRCTABLE_RECORDS_ON_SECTOR;
	BOOL ret = filesystem_read_nameCRCtable(fsys, sector);
	int index;
	if(ret)
		index = file_number%NAMECRCTABLE_RECORDS_ON_SECTOR;
	else//error
		index = E_FAIL;
	return index;
}

//CRCtable work
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//file work

//find a file by name and extention
HFILE filesystem_find_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file, will be found and rewrite*/
							const char* pFileExt /*ext of the file*/
						  )
{
        if(pFileName[0] == '\n')
          return NULL;
	int k;
        WORD sNameCRC = CalcNameCRC(pFileName);
	for(k=FILERECORD_START_NO;k<=FILERECORD_END_NO;k++)
	{
          //PowerControl_kickWatchDog();
          
          //------------Rewrited by Vorobey 2k18--------------------------------
          //speedup file searching using name CRCtable
          int index = filesystem_get_nameCRCIndex(fsys, k);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid CRC record");
          }else
          {
            if(sNameCRC == fsys->nameCRCTableOnSector[index])
            {
		index = filesystem_get_filerecordIndex(fsys, k);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
		}else
		{//found
			if(fsys->fileRecordsOnSector[index].wdStartClaster==CLASTERTABLE_EMPTY_RECORD)continue;
			int cmp = strncmp(fsys->fileRecordsOnSector[index].name, pFileName, FILE_NAME_SZ);
			int cmp2 = strncmp(fsys->fileRecordsOnSector[index].ext, pFileExt, FILE_EXT_SZ);
			if(!(cmp|cmp2))break;
		}
            }
          }
          //--------------------------------------------------------------------
	}
	if(k>FILERECORD_END_NO)
		return NULL;
	else
		return (HFILE)(k+1);
}

//remove file
//ret S_OK if OK
//ret E_FAIL if file is already deleted or battery discharged
HRESULT filesystem_delete_file(struct tagFileSystem *fsys, HFILE hfile)
{
	//if(battery.bBatteryAlarm)return E_FAIL;	//when battery discharged no file operation is allowed

	int index = filesystem_get_first_filerecordIndex(fsys, hfile);
	WORD startclaster = fsys->fileRecordsOnSector[index].wdStartClaster;
	if(startclaster==CLASTERTABLE_EMPTY_RECORD)
	{//no such file
		return E_FAIL;
	}
	BOOL bItsEnd = FALSE;
	do
	{
		//PowerControl_kickWatchDog();
		int clasterindex = filesystem_get_clasterIndex(fsys, startclaster);
		if(clasterindex==E_FAIL)
		{
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
			return E_FAIL;
		}else
		{
			int newstartclaster = fsys->clasterTableOnSector[clasterindex];
			bItsEnd = newstartclaster==startclaster;
			startclaster = newstartclaster;
			if(startclaster!=CLASTERTABLE_EMPTY_RECORD)
			{
				fsys->clasterTableOnSector[clasterindex] = CLASTERTABLE_EMPTY_RECORD;	//change and write
				filesystem_write_clastertable(fsys);
			}
		}
	}while(!bItsEnd && startclaster!=CLASTERTABLE_EMPTY_RECORD);
	fsys->fileRecordsOnSector[index].wdStartClaster = CLASTERTABLE_EMPTY_RECORD;
	filesystem_write_filerecordstable(fsys);
	return (HRESULT)S_OK;
}


//create new file in directory
//ret NULL if file is already exists and bOverwrite = 0 or no free space or battery discharged
//HFILE is a index of file in filerecord table +1
HFILE filesystem_create_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file, will be found and rewrite*/
							const char* pFileExt, /*ext of the file*/
							BOOL bOverwrite	/*1 then exists file will be overwritten*/
						  )
{
	//if(battery.bBatteryAlarm) return NULL;	//when battery discharged no file operation is allowed
	
	HFILE hfile = filesystem_find_file(fsys, pFileName, pFileExt);
	if(hfile!=NULL)
	{
		if(!bOverwrite)return NULL;//already exists and no overwrite allow
		//remove this file
		filesystem_delete_file(fsys, hfile);
	}
	int freefilerecord = filesystem_find_free_filerecord(fsys);
	if(freefilerecord==E_FAIL)
          return NULL;//no free space in filerecord
	int index = filesystem_get_filerecordIndex(fsys, freefilerecord);
	if(index==E_FAIL)
	{//error in filerecord, bad record in eeprom
		exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
		return NULL;
	}
        //Clock_retrieveDateTimeData_intcall();
        //fsys->fileRecordsOnSector[index].dateTime = realTime;
	memset(fsys->fileRecordsOnSector[index].name, 0, FILE_NAME_SZ);
	strncpy(fsys->fileRecordsOnSector[index].name,pFileName,FILE_NAME_SZ);
	memset(fsys->fileRecordsOnSector[index].ext, 0, FILE_EXT_SZ);
	strncpy(fsys->fileRecordsOnSector[index].ext,pFileExt,FILE_EXT_SZ);
	//memcpy(&fsys->fileRecordsOnSector[index].dateTime, (void const*)&fsys->fileRecordsOnSector[index].dateTime, sizeof(struct tagDateTime));
	fsys->fileRecordsOnSector[index].dwLength = 0;
        filesystem_write_filerecordstable(fsys);
	return (HFILE)(freefilerecord+1);
}


//rename file
int filesystem_rename_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file*/
							const char* pFileExt, /*ext of the file*/
						   const char* pNewFileName, /*new name of the file*/
							const char* pNewFileExt /*new ext of the file*/
								)
{
	if(!pFileName || !pFileExt || !pNewFileName || !pNewFileExt)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Invalid parameters");
	}

	//if(battery.bBatteryAlarm)return E_FAIL;	//when battery discharged no file operation is allowed

	HFILE hfile = filesystem_find_file(fsys, pFileName, pFileExt);
	if(hfile==NULL)return E_FAIL;	//no file
	HFILE hfile2 = filesystem_find_file(fsys, pNewFileName, pNewFileExt);
	if(hfile2!=NULL)return E_FAIL;	//new file name is exists
	int index = filesystem_get_first_filerecordIndex(fsys, hfile);
	memset(fsys->fileRecordsOnSector[index].name, 0,FILE_NAME_SZ);
	strncpy(fsys->fileRecordsOnSector[index].name, pNewFileName,FILE_NAME_SZ);
	memset(fsys->fileRecordsOnSector[index].ext, 0,FILE_EXT_SZ);
	strncpy(fsys->fileRecordsOnSector[index].ext, pNewFileExt,FILE_EXT_SZ);
	filesystem_write_filerecordstable(fsys);
	return S_OK;
}

//get file name
int filesystem_get_filename(struct tagFileSystem *fsys, HFILE hFile, /*hfile of file we are looking for name and ext*/
							char* pFileName, /*name of the file*/
							char* pFileExt /*ext of the file*/
								)
{
	if(!pFileName || !pFileExt)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Invalid parameters");
	}
	int index = filesystem_get_first_filerecordIndex(fsys, hFile);
	memset(pFileName, 0, FILE_NAME_SZ);
	strncpy(pFileName, fsys->fileRecordsOnSector[index].name, FILE_NAME_SZ);
	memset(pFileExt, 0, FILE_EXT_SZ);
	strncpy(pFileExt, fsys->fileRecordsOnSector[index].ext, FILE_EXT_SZ);
	return S_OK;
}


//open file from directory
//ret NULL if no file
//HFILE is a index of file in filerecord table +1
HFILE filesystem_open_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file, will be found*/
						   const char* pFileExt /*ext of the file*/
						  )
{
	//reset INI default state
	fsys->file_pos_last_value = -1;
	fsys->file_pos_last_section = -1;
	memset(fsys->lastSectionName,0 , sizeof(fsys->lastSectionName));
	return filesystem_find_file(fsys, pFileName, pFileExt);
}

int gggg = 0;


//find poisition in file and open nessesary claster
//ret E_FAIL if file empty or out of file range
//ret data index of clasterBuffer
int filesystem_read_from_position(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int file_pos /*it is counter or read bytes, input: start from 0, as output ret pointer for the next non read byte*/
							)
{
	int index = filesystem_get_first_filerecordIndex(fsys, hFile);
	int startclaster = (int)fsys->fileRecordsOnSector[index].wdStartClaster;
        
        gggg = startclaster;
        
	if(startclaster==CLASTERTABLE_EMPTY_RECORD)return E_FAIL;	//empty file

	int flen = fsys->fileRecordsOnSector[index].dwLength;
	if(file_pos >= flen)return E_FAIL;	//out of file

	int spendclasters = file_pos/CLASTER_DATA_LEN;
	for(int i=0;i<spendclasters;i++)
	{
		//PowerControl_kickWatchDog();
		int clasterindex = filesystem_get_clasterIndex(fsys, startclaster);
		if(clasterindex==E_FAIL)
		{
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
			return E_FAIL;
		}else
		{
			int newstartclaster = fsys->clasterTableOnSector[clasterindex];
			BOOL bItsEnd = startclaster == newstartclaster;
			startclaster = newstartclaster;
			if(bItsEnd || startclaster==CLASTERTABLE_EMPTY_RECORD)return E_FAIL;	//out of file
		}
	}
        
        gggg = startclaster;
        
	//startclaster now contain a start claster of position
	if(!filesystem_read_claster(fsys, startclaster))
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"invalid file claster");
		return E_FAIL;
	}
	return (file_pos%CLASTER_DATA_LEN);
}

//find poisition in file and open nessesary claster
//ret E_FAIL if file empty or out of file range
//ret data index of clasterBuffer
int filesystem_read_from_position_withoutErr(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int file_pos /*it is counter or read bytes, input: start from 0, as output ret pointer for the next non read byte*/
							)
{
	int index = filesystem_get_first_filerecordIndex(fsys, hFile);
	int startclaster = (int)fsys->fileRecordsOnSector[index].wdStartClaster;
        
        gggg = startclaster;
        
	if(startclaster==CLASTERTABLE_EMPTY_RECORD)return E_FAIL;	//empty file

	int flen = fsys->fileRecordsOnSector[index].dwLength;
	if(file_pos >= flen)return E_FAIL;	//out of file

	int spendclasters = file_pos/CLASTER_DATA_LEN;
	for(int i=0;i<spendclasters;i++)
	{
		//PowerControl_kickWatchDog();
		int clasterindex = filesystem_get_clasterIndex(fsys, startclaster);
		if(clasterindex==E_FAIL)
		{
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
			return E_FAIL;
		}else
		{
			int newstartclaster = fsys->clasterTableOnSector[clasterindex];
			BOOL bItsEnd = startclaster == newstartclaster;
			startclaster = newstartclaster;
			if(bItsEnd || startclaster==CLASTERTABLE_EMPTY_RECORD)return E_FAIL;	//out of file
		}
	}
        
        filesystem_read_claster(fsys, startclaster);
        
	return (file_pos%CLASTER_DATA_LEN);
}

//ret file length in bytes
//ret 0 if file is empty
DWORD filesystem_get_length(struct tagFileSystem *fsys, HFILE hFile /*file descriptor = number of file in file record table*/
							)
{
	int index = filesystem_get_first_filerecordIndex(fsys, hFile);
	if(fsys->fileRecordsOnSector[index].wdStartClaster==CLASTERTABLE_EMPTY_RECORD)
		return 0;
	else
		return fsys->fileRecordsOnSector[index].dwLength;
}

//read file data from filesystem
//ret number of bytes read
//ret E_FAIL - if file_pos out of file ot file empty
int filesystem_file_get(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int* pFile_pos, /*it is counter of read bytes, input: start from 0, as output ret pointer for the next non read byte*/
						  BYTE* pBuffer, /*recepient buffer*/
						  int len)/*input: buffer len in bytes, output: read len in bytes*/
{
	int rlen = 0;
	int flen = filesystem_get_length(fsys, hFile);
	flen -= *pFile_pos;
	if(flen<=0)
          return E_FAIL;
	do
	{
		int buf_index = filesystem_read_from_position(fsys, hFile, *pFile_pos);
		if(buf_index==E_FAIL)
                  return E_FAIL;	//some errors: empty file, out of file
		int calc_len = CLASTER_DATA_LEN-buf_index;
		if(len<calc_len)
		{
			calc_len = len;
		}
		if(flen<calc_len)
		{
			calc_len = flen;
		}
		memcpy(&pBuffer[rlen], &fsys->clasterBuffer[buf_index], calc_len);
		rlen+=calc_len;
		*pFile_pos+=calc_len;
		len -= calc_len;
		flen -= calc_len;
	}while(flen>0 && len>0);
	return rlen;
}


//change file length
//len must be >0 else exception
//if ret E_FAIL then file is already 0, no free space,
//ret S_OK if OK
HRESULT filesystem_file_change_len(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
							   int len /*new len of the file*/
								   )
{
	//if(battery.bBatteryAlarm)return E_FAIL;	//when battery discharged no file operation is allowed

	int newclasterlen = len/CLASTER_DATA_LEN + ((len%CLASTER_DATA_LEN)?1:0);
	if(newclasterlen==0)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"len parameter must not be 0");
		return E_FAIL;
	}
	DWORD curlen = filesystem_get_length(fsys, hFile);
	int curclasterlen = curlen/CLASTER_DATA_LEN + ((curlen%CLASTER_DATA_LEN)?1:0);
	int clasterlen = newclasterlen-curclasterlen;
	int index = filesystem_get_first_filerecordIndex(fsys, hFile);
	int startclaster = fsys->fileRecordsOnSector[index].wdStartClaster;
	if(curlen>len && len>0)
	{//make smaller
		int clbufidx = filesystem_read_from_position(fsys, hFile, len-1);
		if(clbufidx==E_FAIL)
		{
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid file");
			return E_FAIL;
		}
		//delete all next clasters
		BOOL bFirstOne = TRUE;
		BOOL bItsEnd = FALSE;
		int delstartclaster = fsys->iCurrentFileClaster;
		do
		{
			int clasterindex = filesystem_get_clasterIndex(fsys, delstartclaster);
			if(clasterindex==E_FAIL)
			{
				exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
				return E_FAIL;
			}
			int newstartclaster = fsys->clasterTableOnSector[clasterindex];
			bItsEnd = delstartclaster == newstartclaster;
			if(!bItsEnd)
			{
				fsys->clasterTableOnSector[clasterindex] = bFirstOne?delstartclaster:CLASTERTABLE_EMPTY_RECORD;
				filesystem_write_clastertable(fsys);
				bFirstOne = FALSE;
			}
			delstartclaster = newstartclaster;
		}while(!bItsEnd);
	}else if(curlen<len)
	{//make bigger
		if(clasterlen>0)
		{
			int clbufidx = filesystem_read_from_position(fsys, hFile, curlen-1);
			int newstartclaster = fsys->iCurrentFileClaster;
			if(clbufidx==E_FAIL)
			{//no clasters yet for file
				startclaster = filesystem_find_free_claster(fsys, CLASTERTABLE_EMPTY_RECORD);
				if(startclaster==E_FAIL)return E_FAIL;//no free space
				//here must do mark claster as busy
				newstartclaster = startclaster;
				int clasterindex = filesystem_get_clasterIndex(fsys, newstartclaster);
				if(clasterindex==E_FAIL)
					exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
				fsys->clasterTableOnSector[clasterindex] = newstartclaster;
				filesystem_write_clastertable(fsys);
				clasterlen--;
			}
			//make all next clasters
			for(int i = 0;i<clasterlen;i++)
			{
                                
				int new2startclaster = filesystem_find_free_claster(fsys, startclaster);
				if(new2startclaster==E_FAIL)return E_FAIL;//no free space
				int clasterindex = filesystem_get_clasterIndex(fsys, newstartclaster);
				if(clasterindex==E_FAIL)
					exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
				newstartclaster = new2startclaster;
				fsys->clasterTableOnSector[clasterindex] = newstartclaster;
				filesystem_write_clastertable(fsys);
				clasterindex = filesystem_get_clasterIndex(fsys, newstartclaster);
				if(clasterindex==E_FAIL)
					exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
				fsys->clasterTableOnSector[clasterindex] = newstartclaster;
				filesystem_write_clastertable(fsys);
			}
		}
	}
	fsys->fileRecordsOnSector[index].wdStartClaster = startclaster;
	fsys->fileRecordsOnSector[index].dwLength = len;
	filesystem_write_filerecordstable(fsys);
	return S_OK;
}


//save file to filesystem
//ret number of bytes written
//ret E_FAIL - if error
int filesystem_file_put(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int* pFile_pos, /*it is counter or written bytes, input: start from 0, as output ret pointer for the next non written byte*/
						  const BYTE* pBuffer, /*send buffer*/
						  int len)/*buffer len in bytes*/
{
	//if(battery.bBatteryAlarm) return E_FAIL;	//when battery discharged no file operation is allowed

	int filelen = *pFile_pos + len;
	if(filelen==0)
          return E_FAIL;	//no free space
	int wlen = 0;
	do
	{
		//int buf_index = filesystem_read_from_position_withoutErr(fsys, hFile, *pFile_pos);
                int buf_index = filesystem_read_from_position(fsys, hFile, *pFile_pos);
		if(buf_index==E_FAIL)
		{
			if(filesystem_file_change_len(fsys, hFile, filelen)!=S_OK)
			{
				return E_FAIL;
			}
			//buf_index = filesystem_read_from_position_withoutErr(fsys, hFile, *pFile_pos);
                        buf_index = filesystem_read_from_position(fsys, hFile, *pFile_pos);
			if(buf_index==E_FAIL)
			{
				return E_FAIL;	//invalid file
//				exception(__FILE__,__FUNCTION__,__LINE__,"internal error");
			}
		}
		int calc_len = CLASTER_DATA_LEN-buf_index;
		if(len<calc_len)
		{
			calc_len = len;
			len = 0;
		}else
			len -= calc_len;
		memcpy(&fsys->clasterBuffer[buf_index], &pBuffer[wlen], calc_len);
		filesystem_write_claster(fsys, fsys->iCurrentFileClaster);
		wlen+=calc_len;
		*pFile_pos+=calc_len;
	}while(len>0);
	return wlen;
}


//create file dir list
//ret items number retrieved
//does not get system files
int filesystem_get_dir(struct tagFileSystem *fsys, const char* pFileExt, /*ext of file, if NULL then all files */
						struct tagDirEntry* pDir, /*output directory entries*/
						int itemsNum, /*items number of pDir*/
						int itemsPass	/*items number must be passed*/
							)
{
	if(itemsNum<1)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"itemsNum < 1");
	}
	int retnum = 0;
	for(int i=FILERECORD_START_NO;i<=FILERECORD_END_NO;i++)
	{
          //------------Rewrited by Vorobey 2k18--------------------------------
          //speedup file searching using name CRCtable
          int index = filesystem_get_nameCRCIndex(fsys, i);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid CRC record");
          }else
          {
            if(fsys->nameCRCTableOnSector[index] != NAMECRCTABLE_EMPTY_RECORD)
            {
		//PowerControl_kickWatchDog();
		index = filesystem_get_filerecordIndex(fsys, i);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"invalid file record");
		}else
		{
			//if(fsys->fileRecordsOnSector[index].wdStartClaster==CLASTERTABLE_EMPTY_RECORD)continue;	//deleted
			if(!strncmp(fsys->fileRecordsOnSector[index].ext,pFileExt,FILE_EXT_SZ) &&
				!(!strncmp(fsys->fileRecordsOnSector[index].ext,"spz",FILE_EXT_SZ)
			   && (!strncmp(fsys->fileRecordsOnSector[index].name,"energy",FILE_NAME_SZ)
			   || (!strncmp(fsys->fileRecordsOnSector[index].name,"sigma",FILE_NAME_SZ)))))
			{//not deleted, not filtered, not system
				if(itemsPass)
				{
					--itemsPass;
				}else
				{
					//yes we found one
					memset(pDir->name, 0, FILE_NAME_SZ);
					strncpy(pDir->name, fsys->fileRecordsOnSector[index].name, FILE_NAME_SZ);
					retnum++;
					pDir++;
					if(--itemsNum==0)break;
				}
			}
		}
            }
          }
          //--------------------------------------------------------------------
        }
	return retnum;
}


//find first file with filter by extension
HFILE filesystem_open_first(struct tagFileSystem *fsys, char* pExt)
{
	HFILE hfile = NULL;
	for(int i=FILERECORD_START_NO;i<=FILERECORD_END_NO;i++)
	{
          //------------Rewrited by Vorobey 2k18--------------------------------
          //speedup file searching using name CRCtable
          int index = filesystem_get_nameCRCIndex(fsys, i);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
          }else
          {
            if(fsys->nameCRCTableOnSector[index] != NAMECRCTABLE_EMPTY_RECORD)
            {
		//PowerControl_kickWatchDog();
		index = filesystem_get_filerecordIndex(fsys, i);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
		}else
		{
			//if(fsys->fileRecordsOnSector[index].wdStartClaster!=CLASTERTABLE_EMPTY_RECORD)
			{
				hfile = i+1;
				if(!pExt)
					return hfile;
				else if(!strncmp(fsys->fileRecordsOnSector[index].ext, pExt, FILE_EXT_SZ))
					return hfile;
			}
		}
            }
          }
          //--------------------------------------------------------------------
	}
	return NULL;
}

HFILE filesystem_open_num(struct tagFileSystem *fsys, char* pExt, int num)
{
	HFILE hfile = NULL;
        int counter = 0;
        if(num < 0)
          exception(__FILE__,__FUNCTION__,__LINE__,"Wrong number");
	for(int i=FILERECORD_START_NO;i<=FILERECORD_END_NO;i++)
	{
          //------------Rewrited by Vorobey 2k18--------------------------------
          //speedup file searching using name CRCtable
          int index = filesystem_get_nameCRCIndex(fsys, i);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
          }else
          {
            if(fsys->nameCRCTableOnSector[index] != NAMECRCTABLE_EMPTY_RECORD)
            {
		//PowerControl_kickWatchDog();
		index = filesystem_get_filerecordIndex(fsys, i);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
		}else
		{
			//if(fsys->fileRecordsOnSector[index].wdStartClaster!=CLASTERTABLE_EMPTY_RECORD)
			{
				hfile = i+1;
				if(!pExt)
                                {
                                  if(counter < num)
                                    counter++;
                                  else
                                    return hfile;
                                }
				else if(!strncmp(fsys->fileRecordsOnSector[index].ext, pExt, FILE_EXT_SZ))
                                {
                                  if(counter < num)
                                    counter++;
                                  else
                                    return hfile;
                                }
			}
		}
            }
          }
          //--------------------------------------------------------------------
	}
	return NULL;
}

//find first file with filter by extension and name that begins from
HFILE filesystem_open_firstEx(struct tagFileSystem *fsys, char* pName, char* pExt)
{
	HFILE hfile = NULL;
	for(int i=FILERECORD_START_NO;i<=FILERECORD_END_NO;i++)
	{
          //------------Rewrited by Vorobey 2k18--------------------------------
          //speedup file searching using name CRCtable
          int index = filesystem_get_nameCRCIndex(fsys, i);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
          }else
          {
            if(fsys->nameCRCTableOnSector[index] != NAMECRCTABLE_EMPTY_RECORD)
            {
		//PowerControl_kickWatchDog();
		index = filesystem_get_filerecordIndex(fsys, i);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
		}else
		{
			//if(fsys->fileRecordsOnSector[index].wdStartClaster!=CLASTERTABLE_EMPTY_RECORD)
			{
				hfile = i+1;
				if(!pExt)
					return hfile;
				else if(!strncmp(fsys->fileRecordsOnSector[index].ext, pExt, FILE_EXT_SZ) &&
						strstr(fsys->fileRecordsOnSector[index].name, pName)==fsys->fileRecordsOnSector[index].name)
					return hfile;
			}
		}
            }
          }
          //--------------------------------------------------------------------
	}
	return NULL;
}


//check that file is exists
//ret TRUE if exists
//ret FALSE if error or no file
BOOL filesystem_check_file_exists(struct tagFileSystem *fsys, HFILE hFile)
{
	int filerecord = hFile-1;
	if(filerecord<FILERECORD_START_NO || filerecord>FILERECORD_END_NO)
		return FALSE;
	int idx = filesystem_get_filerecordIndex(fsys, filerecord);
	if(idx==E_FAIL)return FALSE;
	return (fsys->fileRecordsOnSector[idx].wdStartClaster!=CLASTERTABLE_EMPTY_RECORD);
}

//file work
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void filesystem_detect_memorylow(struct tagFileSystem *fsys)
{
	fsys->uiCalcFreeMem = filesystem_calc_free_memory(fsys)/1024;
	fsys->bMemoryLow = ((UINT)(fsys->uiCalcFreeMem*100/TOTAL_MEMORY) < 3);
}


