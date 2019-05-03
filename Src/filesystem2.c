#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "filesystem.h"
#include <stdlib.h>
#include "eeprom.h"
#include "main.h"

#include "exeptions.h"
	
//вынес из функции поскольку сохранение 33Кб в стэк вело к его переполнению=))))
BYTE claster_buffer[MAX_CLASTERS];

extern struct tagDateTime realTime;
extern struct tagFileSystem filesystem;

//вынес в отдельную функцию для удобства
void filesystem_rec_clasterIndex(struct tagFileSystem *fsys)
{
  fsys->iCurrentFileClaster=-1;
  fsys->iCurrentFileClasterSector=-1;
}



//can be run once per start or rarely
//restoring claster table, find lost clasters and mark them empty
//losten clasters are from files that was saved but not closed
/*
заложен следующий механизм:
1.проверка всех секторов на формат
1.1. если сектор дает сбой, то переформатирование всей фс
2.проверка файлов по таблице файлов
2.1. если таблица файлов дает сбой, то переформатирование всей фс
2.2. если последовательность для файла ошибочна, то удаляется файл
3. поиск потерянных кластеров и воостановление их статуса пустых
*/

void filesystem_check_and_restore(struct tagFileSystem *fsys)
{
        
	//add 24/09/2010
	if(fsys->eepromc->wdEepromWritesCounter>50000)
	{//regenerate eeprom
		for(int i=0;i<MAX_SECTORS;i++)
		{
			//PowerControl_kickWatchDog();
			EEPROM_RegenerateFlash(fsys->eepromc, i);
		}
		fsys->eepromc->wdEepromWritesCounter = 0;
		//save essential data in eeprom (it is a eeprom writes counter )
		EEPROM_UpdateEssentialDataInEeprom(fsys->eepromc);
	}
	//BOOL bShowOnce = FALSE;
	BOOL bError = FALSE;
	
	//потому как мы испортили содержимое clasterBuffer
	filesystem_rec_clasterIndex(fsys);
	
	//check claster table and try to restore it before check sectors

	for(int i=FILES_CLASTER_START_NO;i<=FILES_CLASTER_END_NO;i++)
	{
		//PowerControl_kickWatchDog();
		filesystem_get_clasterIndex(fsys, i);
	}
	//потому как мы испортили содержимое clasterBuffer
	filesystem_rec_clasterIndex(fsys);

	//check files table and try to restore it before check sectors
	for(int i=FILERECORD_START_NO;i<=FILERECORD_END_NO;i++)
	{
		//PowerControl_kickWatchDog();
		filesystem_get_filerecordIndex(fsys, i);
	}

	//потому как мы испортили содержимое clasterBuffer
	filesystem_rec_clasterIndex(fsys);
        
        //--------------------------Vorobey 2k18--------------------------------
        //check nameCRC table and try to restore it before check sectors
	for(int i=FILERECORD_START_NO;i<=FILERECORD_END_NO;i++)
	{
		//PowerControl_kickWatchDog();
		filesystem_get_nameCRCIndex(fsys, i);
	}
        //----------------------------------------------------------------------

	//потому как мы испортили содержимое clasterBuffer
	filesystem_rec_clasterIndex(fsys);
	
	//BYTE claster_buffer[MAX_CLASTERS];	//0-not used, 1-used
	memset(&claster_buffer[0], 0, MAX_CLASTERS);

	//bShowOnce = FALSE;
	
	for(int k=FILERECORD_START_NO;k<=FILERECORD_END_NO;k++)
	{

          if(bError)break;
          //------------Rewrited by Vorobey 2k18--------------------------------
          //speedup file searching using name CRCtable
          int index = filesystem_get_nameCRCIndex(fsys, k);
          if(index==E_FAIL)
          {//error in filerecord, bad record in eeprom, all files of this records must be deleted
			bError = 1;	//restore everything
			break;
          }else
          {
            if(fsys->nameCRCTableOnSector[index] != NAMECRCTABLE_EMPTY_RECORD)
            {
		//PowerControl_kickWatchDog();

		index = filesystem_get_filerecordIndex(fsys, k);
		if(index==E_FAIL)
		{//error in filerecord, bad record in eeprom, all files of this records must be deleted
			bError = 1;	//restore everything
			break;
		}else
		{//found
			int startclaster = fsys->fileRecordsOnSector[index].wdStartClaster;
			if(startclaster==CLASTERTABLE_EMPTY_RECORD)
			{//empty, no file, go to the next one
				continue;
			}else if(startclaster<FILES_CLASTER_START_NO || startclaster>FILES_CLASTER_END_NO)
			{//error in file record
				//здесь можно просто пометить кластер файла как пустой и удалится только файл и не надо переформатировать всю фс
				//при этом образуются потерянные кластеры
				fsys->fileRecordsOnSector[index].wdStartClaster = CLASTERTABLE_EMPTY_RECORD;
				filesystem_write_filerecordstable(fsys);
				//bError = 1;	//mark that whole file system needs reformat
			}else
			{//have a file
				BOOL itsend =FALSE;
				do
				{
					if(startclaster==CLASTERTABLE_EMPTY_RECORD ||	//bad file
					   (startclaster!=CLASTERTABLE_EMPTY_RECORD &&
					   claster_buffer[startclaster]))//error, this claster is already used
					{//bad file, have to delete it
                                          
						filesystem_delete_file(fsys, index+1);
					}
					//ok, go ahead, mark as used
					claster_buffer[startclaster]=1;
					int cindex = filesystem_get_clasterIndex(fsys, startclaster);
					if(cindex==E_FAIL)
						exception(__FILE__,__FUNCTION__,__LINE__,"invalid claster record");
						
					int newstartclaster = fsys->clasterTableOnSector[cindex];

					if(newstartclaster<FILES_CLASTER_START_NO || newstartclaster>FILES_CLASTER_END_NO)
					{//error in file record

						startclaster = CLASTERTABLE_EMPTY_RECORD;
						itsend = FALSE;
						continue;	//go up to remove file
					}
					itsend = newstartclaster==startclaster;
					startclaster = newstartclaster;
				}while(!itsend);
			}
		}
            }
          }
          //--------------------------------------------------------------------
	}
	//check sector's format by CRC
        
	for(int i=CLASTERTABLE_START_SECTOR_NO;i<MAX_SECTORS;i++)
	{
		//PowerControl_kickWatchDog();
		if(!EEPROM_ReadSector(fsys->eepromc, i, (BYTE*)&fsys->clasterBuffer[0]))
		{//bad sector, format it

//we dont need to ask to reformat the whole file system that is maybe empy space only
//			bError = 1;	//mark that whole file system needs reformat
			//if(!battery.bBatteryAlarm)	//when battery discharged no file operation is allowed
			{
				for(int j=0;j<SECTOR_DATA_LEN;j++)
					fsys->clasterBuffer[j] = 0xff;
				EEPROM_WriteSector(fsys->eepromc, i, (BYTE*)&fsys->clasterBuffer[0]);
				if(!EEPROM_ReadSector(fsys->eepromc, i, (BYTE*)&fsys->clasterBuffer[0]))	//check after write
				{
					exception(__FILE__,__FUNCTION__,__LINE__,"EEPROM failed!");
				}
			}
		}
	}
        
	//потому как мы испортили содержимое clasterBuffer
	filesystem_rec_clasterIndex(fsys);
	
	
	if(bError)
	{//request for restore everything
          filesystem_format_filetable(fsys, 1);
	}else
	{//look for not used clasters and clean them

		//if(!battery.bBatteryAlarm)//when battery discharged no file operation is allowed
		{

			BOOL bNeedSave = 0;
			for(int i=FILES_CLASTER_START_NO;i<=FILES_CLASTER_END_NO;i++)
			{
				if(!claster_buffer[i])
				{//not used
					int sector = CLASTERTABLE_START_SECTOR_NO+i/CLASTERTABLE_RECORDS_ON_SECTOR;
					if(sector!=fsys->iCurrentClasterTableSector && bNeedSave)
					{
						filesystem_write_clastertable(fsys);
						bNeedSave = 0;
					}
	
					int index = filesystem_get_clasterIndex(fsys, i);
					if(index==E_FAIL)
						exception(__FILE__,__FUNCTION__,__LINE__,"EEPROM failed!");
					
					if(fsys->clasterTableOnSector[index]!=CLASTERTABLE_EMPTY_RECORD)
					{//not empty claster but no file associated
						fsys->clasterTableOnSector[index] = CLASTERTABLE_EMPTY_RECORD;
						bNeedSave = 1;
					}
				}
			}
			if(bNeedSave)
			{
				filesystem_write_clastertable(fsys);
				bNeedSave = 0;
			}
		}
	}
        
}

//erase whole file system
void filesystem_format_filetable(struct tagFileSystem *fsys, BOOL bYes)
{
	if(!bYes)return;
        
        //EEPROM_chipErase(&EEPROMControl);
	
	//if(battery.bBatteryAlarm)return;	//when battery discharged no file operation is allowed
	
	//!!!!!!!!!!! be careful everything will be lost
	//restore claster table
	for(int j=0;j<CLASTERTABLE_RECORDS_ON_SECTOR;j++)
		fsys->clasterTableOnSector[j] = CLASTERTABLE_EMPTY_RECORD;
	for(int i=0;i<CLASTERTABLE_SECTOR_AREA_LEN;i++)
	{//rewrite all clasters
		fsys->iCurrentClasterTableSector = i+CLASTERTABLE_START_SECTOR_NO;
		filesystem_write_clastertable(fsys);
		//PowerControl_kickWatchDog();
	}
        
        for(int j=0;j<NAMECRCTABLE_RECORDS_ON_SECTOR;j++)
		fsys->nameCRCTableOnSector[j] = NAMECRCTABLE_EMPTY_RECORD;
	for(int i=0;i<NAMECRCTABLE_SECTOR_AREA_LEN;i++)
	{//rewrite all clasters
		fsys->iCurrentNameCRCTableSector = i+NAMECRCTABLE_START_SECTOR_NO;
		filesystem_write_nameCRCtable(fsys);
		//PowerControl_kickWatchDog();
	}
        
	//restore filerecord table
	for(int j=0;j<FILERECORD_RECORDS_ON_SECTOR;j++)
		fsys->fileRecordsOnSector[j].wdStartClaster = CLASTERTABLE_EMPTY_RECORD;
	for(int i=0;i<FILERECORD_SECTOR_AREA_LEN;i++)
	{//rewrite all clasters
		fsys->iCurrentFileRecordSector = i+FILERECORD_START_SECTOR_NO;
		filesystem_write_filerecordstable(fsys);
		//PowerControl_kickWatchDog();
	}
	filesystem_rec_clasterIndex(fsys);
}


//copy bytes from one file to another from one position to another
//if ret E_FAIL then no free memory
int filesystem_copy_file_data(struct tagFileSystem *fsys, HFILE hFileDst, HFILE hFileSrc, int file_pos_dst, int file_pos_src, int len)
{
	BYTE buf[CLASTER_DATA_LEN];
	int pass_clasters = len/CLASTER_DATA_LEN;
	int pass_bytes = len%CLASTER_DATA_LEN;
	for(int i=0;i<pass_clasters;i++)
	{
		int rlen = filesystem_file_get(fsys, hFileSrc, &file_pos_src, buf, CLASTER_DATA_LEN);
		if(rlen==E_FAIL)return E_FAIL;//failed to read file or file empty
		if(rlen!=CLASTER_DATA_LEN)
		{
			exception(__FILE__,__FUNCTION__,__LINE__,"Internal error!");
		}
		int wlen = filesystem_file_put(fsys, hFileDst, &file_pos_dst, buf, rlen);
		if(wlen==E_FAIL)return E_FAIL;//failed to write file or no free memory
		//PowerControl_kickWatchDog();
	}
	int rlen = filesystem_file_get(fsys, hFileSrc, &file_pos_src, buf, pass_bytes);
	if(rlen==E_FAIL)return E_FAIL;//failed to read file or file empty
	if(rlen!=pass_bytes)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Internal error!");
	}
	int wlen = filesystem_file_put(fsys, hFileDst, &file_pos_dst, buf, rlen);
	if(wlen==E_FAIL)return E_FAIL;//failed to write file or no free memory
	return S_OK;
}

//cut file on bytes_cut if >0, grows file if <0
//procedure is going next: create temp file, save all data to temp file ith cut, delete original, rename temp to original
//ret cur len of file
//file_pos<file_len and bytes_cut<file_len
//ret E_FAIL if no free memory
int filesystem_cut_file(struct tagFileSystem *fsys, HFILE hFile, /*file that will be cut*/
						 int file_pos, /*file pos from cut*/
						 int bytes_cut /*bytes to cut >0, if <0 then grows*/
							 )
{
	if(bytes_cut==0)return S_OK;
	
	int flen = filesystem_get_length(fsys, hFile);
	if(file_pos>=flen)
	{
		//invalid file pos
		exception(__FILE__,__FUNCTION__,__LINE__,"Invalid parameters");
	}
	if(bytes_cut>=flen)
	{
		//invalid bytes_cut
		exception(__FILE__,__FUNCTION__,__LINE__,"Invalid parameters");
	}
	char newFileName[FILE_NAME_SZ];
	//char temp[FILE_NAME_SZ];
	//sprintf(newFileName, "tmp_%s", Clock_getClockDateTimeStrEx(temp));
	HFILE hNewFile = filesystem_create_file(fsys, newFileName,"tmp", 1);
	if(hNewFile==NULL)return E_FAIL;
	//read from old file and save to the new
	//copy unchanged data byte to byte
	int ret = filesystem_copy_file_data(fsys, hNewFile, hFile, 0, 0, file_pos);
	if(ret==E_FAIL)return E_FAIL;
	//we saved all data upto cut position
	//now we save data depends on cutting bytes number
	int newpos = file_pos+bytes_cut;
	ret = filesystem_copy_file_data(fsys, hNewFile, hFile, file_pos, newpos, flen-newpos);
	if(ret==E_FAIL)return E_FAIL;
	/////////////
	filesystem_delete_file(fsys, hFile);
	//reside newHFILE to the prev HFILE
	int idx = filesystem_get_first_filerecordIndex(fsys, hNewFile);
	int cls_idx = fsys->fileRecordsOnSector[idx].wdStartClaster;
	DWORD src_len = fsys->fileRecordsOnSector[idx].dwLength;
	int idx_dst = filesystem_get_first_filerecordIndex(fsys, hFile);
	//reside data from temp file to the deleted file, and now it is restored
	fsys->fileRecordsOnSector[idx_dst].wdStartClaster = cls_idx;
	fsys->fileRecordsOnSector[idx_dst].dwLength = src_len;
	//memcpy(&fsys->fileRecordsOnSector[idx_dst].dateTime,
	//	   &fsys->fileRecordsOnSector[idx].dateTime,
	//	   sizeof(struct tagDateTime));
	filesystem_write_filerecordstable(fsys);
	//delete temp file
	idx = filesystem_get_first_filerecordIndex(fsys, hNewFile);
	fsys->fileRecordsOnSector[idx].wdStartClaster = CLASTERTABLE_EMPTY_RECORD;
	filesystem_write_filerecordstable(fsys);
	return S_OK;
}



//подсичтать количество специальных файлов
void filesystem_calc_special_files_number(struct tagFileSystem *fsys, UINT* pAllFilesNum, 
					UINT* pSPZFilesNum,
					UINT* pLIBFilesNum,
					UINT* pMCSFilesNum,
					UINT* pLOGFilesNum)
{
	UINT allfiles=0;
	UINT spzfiles=0;
	UINT mcsfiles=0;
	UINT logfiles=0;
	UINT libfiles=0;
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
			//if(fsys->fileRecordsOnSector[j].wdStartClaster!=CLASTERTABLE_EMPTY_RECORD)
			{
				allfiles++;
				if(!strncmp(fsys->fileRecordsOnSector[index].ext, "spz", FILE_EXT_SZ))
					spzfiles++;
				else
				if(!strncmp(fsys->fileRecordsOnSector[index].ext, "lib", FILE_EXT_SZ))
					libfiles++;
				else
				if(!strncmp(fsys->fileRecordsOnSector[index].ext, "mcs", FILE_EXT_SZ))
					mcsfiles++;
				else
				if(!strncmp(fsys->fileRecordsOnSector[index].ext, "log", FILE_EXT_SZ))
					logfiles++;
			}
		}
            }
          }
          //--------------------------------------------------------------------
	}
	*pAllFilesNum=allfiles;
	*pSPZFilesNum=spzfiles;
	*pLIBFilesNum=libfiles;
	*pMCSFilesNum=mcsfiles;
	*pLOGFilesNum=logfiles;
}



//calc number of files
//pExt - filter by extension if not NULL then calc only those files
UINT filesystem_calc_files_number(struct tagFileSystem *fsys, char* pExt)
{
	UINT files=0;
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
			if(fsys->fileRecordsOnSector[index].wdStartClaster!=CLASTERTABLE_EMPTY_RECORD)
			{
				if(!pExt)
					files++;
				else if(!strncmp(fsys->fileRecordsOnSector[index].ext, pExt, FILE_EXT_SZ))
					files++;
			}
		}
            }
          }
          //--------------------------------------------------------------------
	}
	return files;
}



//ret free memory in bytes
UINT filesystem_calc_free_memory(struct tagFileSystem *fsys)
{
	UINT membusy=0;
	for(int i=0;i<CLASTERTABLE_SECTOR_AREA_LEN;i++)
	{
		if(!filesystem_read_clastertable(fsys, i+CLASTERTABLE_START_SECTOR_NO))
		{
			exception(__FILE__,__FUNCTION__,__LINE__,"Filesystem failed");
		}
		for(int j=0;j<CLASTERTABLE_RECORDS_ON_SECTOR;j++)
		{
			if(fsys->clasterTableOnSector[j]!=CLASTERTABLE_EMPTY_RECORD)membusy++;
		}
	}
	return ((FILES_CLASTER_AREA_LEN-membusy)*CLASTER_DATA_LEN);
}


//get string from the file
//ret string len
//string_len must be <MAX_INI_STRING_LEN
//if E_FAIL then error or file empty
//ret string without '\r' and '\0'
//ret 0 if empty string
int filesystem_get_string(struct tagFileSystem *fsys, HFILE hFile, int* pFile_pos, char* pString, int string_len)
{
	if(string_len>MAX_INI_STRING_LEN)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Requested string is too long");
	}
	char buf[MAX_INI_STRING_LEN];
	memset(buf,0,MAX_INI_STRING_LEN);
	int pos = *pFile_pos;
	int rlen = filesystem_file_get(fsys, hFile, &pos,(BYTE*) buf, MAX_INI_STRING_LEN);
	if(rlen==E_FAIL)return E_FAIL;
	if(rlen==0)
	{
		pString[0]='\0';
		return rlen;
	}
	char* pAdr = strchr(buf, '\r');
	if(!pAdr)
	{//may be it is last string or string is too long
		pAdr = strchr(buf, '\0');
		if(!pAdr)return E_FAIL;	//string is too long
//		if(*pAdr!='\0')return E_FAIL;	//last 0
		//it is last string
	}
	//copy string
	int retlen = pAdr-buf;
	if(retlen>string_len)return E_FAIL;	//empty string
	if(retlen==0)
	{//empty string
		pString[0]=buf[0];
		pString[1]='\0';
	}else
	{
		memcpy(pString,buf,retlen);
		if(retlen<string_len)
			pString[retlen]='\0';
	}
	*pFile_pos += retlen+1;
	return retlen;
}


//get value in int format from the txt file, depends on section and value name
//ret S_OK
//if ret E_FAIL then empty file, no section, no value
int filesystem_ini_get_int(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, int* pValue)
{
	char buf[MAX_INI_STRING_LEN];
	memset(buf, 0, sizeof(buf));
	int rlen = filesystem_ini_get_string(fsys, hFile, pSection, pName, buf, sizeof(buf));
	if(rlen==E_FAIL)return E_FAIL;
	*pValue = atoi(buf);
	return S_OK;
}

//get value in txt from the txt file, depends on section and value name
//ret value len
//if ret E_FAIL then empty file, no section, no value
int filesystem_ini_get_string(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, char* pValue, int value_len)
{
	int file_pos = 0;
	
	file_pos = file_system_find_section(fsys, hFile, pSection);
	if(file_pos==E_FAIL)return E_FAIL;
	
	file_pos = filesystem_find_value(fsys, hFile, file_pos, pName);
	if(file_pos==E_FAIL)return E_FAIL;

	memset(pValue, 0, value_len);
	int rlen = filesystem_get_string(fsys, hFile, &file_pos, pValue, value_len);
	if(rlen==E_FAIL)return E_FAIL;
	return strlen(pValue);
}

//find value in txt file by name in one section
//ret pos of value mean
//ret E_FAIL if error or no found
int filesystem_find_value(struct tagFileSystem *fsys, HFILE hFile, int start_file_pos, const char* pName)
{
	char buf[MAX_INI_STRING_LEN];
	int buflen = sizeof(buf);
	int name_len = strlen(pName);
	int file_pos;
	int stop_file_pos = 0;
	file_pos = start_file_pos;
	if(fsys->file_pos_last_value>=0)//will try to search from the last value
		file_pos = fsys->file_pos_last_value;
	do
	{
		//PowerControl_kickWatchDog();
		
		int rlen = filesystem_get_string(fsys, hFile, &file_pos, buf, buflen);
		if(rlen==E_FAIL ||
		   (stop_file_pos>0 && file_pos>stop_file_pos) ||
			  buf[0]=='[')
		{
			//выход на втором цикле поиска
			if(fsys->file_pos_last_value>=0)
			{
				stop_file_pos = fsys->file_pos_last_value;	//для раннего выхода из второго круга
				fsys->file_pos_last_value = -1;
				file_pos = start_file_pos;
				if(fsys->file_pos_last_value>=0)//will try to search from the last value
					file_pos = fsys->file_pos_last_value;
				continue;
			}else
				return E_FAIL;
		}
		if(buf[0]==';')continue;	//remark
		//look for value
		char* pAdr = strstr(buf, pName);
		if(!pAdr)continue;
		if(pAdr[name_len]!='=' && pAdr[name_len]!=' ')continue;
		pAdr = strchr(pAdr, '=');
		if(!pAdr)continue;
		//value found
		//pass spaces
		do
		{
			pAdr++;
		}while(*pAdr==' ');
		file_pos = file_pos-rlen+(pAdr-buf-1);
		fsys->file_pos_last_value = file_pos;
		return file_pos;
	}while(TRUE);
	//return E_FAIL;
}



//find section in txt file
//ret pos of next line after section
//ret E_FAIL if error or no found
//реализован алгоритм поиска секции последовательно от последней использованной.
//но если ничего не будет найдено, то тогда поиск начнется с начала, что займет монго больше времени
int file_system_find_section(struct tagFileSystem *fsys, HFILE hFile, const char* pSection)
{
	//check if we are already in this section
	if(strcmp(fsys->lastSectionName, pSection)==0 && fsys->file_pos_last_section>=0)
		return fsys->file_pos_last_section;
	
	fsys->file_pos_last_value = -1;
	
	char buf[MAX_INI_STRING_LEN];
	int buflen = sizeof(buf);
	int file_pos;
	int stop_file_pos = 0;
	
	file_pos = 0;
	if(fsys->file_pos_last_section>=0)//will try to search from the last section
		file_pos = fsys->file_pos_last_section;
	int section_len = strlen(pSection);
	do
	{
		
		//PowerControl_kickWatchDog();
		int rlen = filesystem_get_string(fsys, hFile, &file_pos, buf, buflen);
		if(rlen==E_FAIL || (stop_file_pos>0 && file_pos>stop_file_pos))
		{
			//выход на втором цикле поиска
			if(fsys->file_pos_last_section>=0)
			{
				stop_file_pos = fsys->file_pos_last_section;	//для раннего выхода из второго круга
				fsys->file_pos_last_section = -1;
				fsys->file_pos_last_value = -1;
				file_pos = 0;
				if(fsys->file_pos_last_section>=0)//will try to search from the last section
					file_pos = fsys->file_pos_last_section;
				continue;
			}else
				return E_FAIL;
		}
		if(buf[0]==';')continue;	//remark
		if(buf[0]!='[')continue;	//not a section start
		char* pAdr = strstr(buf, pSection);
		if(!pAdr)continue;
		if(pAdr[section_len]!=']')continue;
		//found section
		strcpy(fsys->lastSectionName, pSection);
		fsys->file_pos_last_section = file_pos;
		return file_pos;
	}while(TRUE);
	//return E_FAIL;
}


//put value in txt to the txt file, depends on section and value name
//if OK then ret S_OK
//if ret E_FAIL then no section, no value, empty file, several faults
int filesystem_ini_put_string(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, const char* pValue)
{
	int file_pos = 0;
	
	file_pos = file_system_find_section(fsys, hFile, pSection);
	if(file_pos==E_FAIL)return E_FAIL;
	
	file_pos = filesystem_find_value(fsys, hFile, file_pos, pName);
	if(file_pos==E_FAIL)return E_FAIL;

	int fpos = file_pos;
	char buf[MAX_INI_STRING_LEN];
	int buflen = sizeof(buf);
	int rlen = filesystem_get_string(fsys, hFile, &fpos, buf, buflen);
	if(rlen==E_FAIL)return E_FAIL;
	int value_len = strlen(pValue);
	if(value_len>=buflen)return E_FAIL;
	int ret = filesystem_cut_file(fsys, hFile, file_pos, rlen-value_len);
	if(ret!=S_OK)return E_FAIL;
	int wlen = filesystem_file_put(fsys, hFile, &file_pos, (const unsigned char*)pValue, value_len);
	if(wlen==E_FAIL)return E_FAIL;//failed to write file or no free memory
	return S_OK;
}

//put value in int format in the txt file, depends on section and value name
//ret S_OK
//if ret E_FAIL then empty file, no section, no value
int filesystem_ini_put_int(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, int value)
{
	char buf[MAX_INI_STRING_LEN];
	sprintf(buf, "%d", value);
	int rlen = filesystem_ini_put_string(fsys, hFile, pSection, pName, buf);
	if(rlen==E_FAIL)return E_FAIL;
	return S_OK;
}

#ifndef HIBYTE_W
#define HIBYTE_W(w)           ((WORD)((DWORD_PTR)(w) >> 8))
#endif  //#ifndef HIBYTE_W

#ifndef LOBYTE
#define LOBYTE(w)           ((uint8_t)(((ULONG_PTR)(w)) & 0xff))
#endif  //#ifndef LOBYTE

uint16_t USBRS_readDir(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen)
{
        txBuff[0] = rxBuff[0];
	uint16_t adr = ((uint16_t)rxBuff[2]<<8)|rxBuff[3];
        /*
        if(flag_EEPROM_Err)
        {
          USBRS_except(4, pUart);
          return;
        }
        */
	if(adr>=MAX_FILES)
          return USBRS_except(2, txBuff, txBuffLen);
        
	uint16_t recs = ((uint16_t)rxBuff[4]<<8)|rxBuff[5];
        
	if(recs>(txBuffLen/sizeof(struct tagFileRecord)) || recs<1)
		return USBRS_except(3, txBuff, txBuffLen);
        
	for(int i=0;i<recs;i++)
	{
		int index = filesystem_get_filerecordIndex(fsys, adr+i);
		if(index==E_FAIL)
			return USBRS_except(4, txBuff, txBuffLen);
		memcpy((void*)&txBuff[4+i*sizeof(struct tagFileRecord)],
			   (void*)&fsys->fileRecordsOnSector[index],
			   sizeof(struct tagFileRecord));
	}
	int len = recs*sizeof(struct tagFileRecord);
	txBuff[2] = HIBYTE_W(len);
	txBuff[3] = LOBYTE(len);
        memcpy(txBuff, rxBuff, 2);
	return 4+len;
}

uint16_t USBRS_readFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen)
{
        txBuff[0] = rxBuff[0];
	uint16_t fnum = ((uint16_t)rxBuff[2]<<8)|rxBuff[3];
        /*if(flag_EEPROM_Err)
        {
          USBRS_except(4, pUart);
          return;
        }*/
	if(fnum>=MAX_FILES)//file number invalid
		return USBRS_except(2, txBuff, txBuffLen);
        
	HFILE hfile = (HFILE)(fnum+1);
        
	if(!filesystem_check_file_exists(fsys, hfile))//no such file
		return USBRS_except(2, txBuff, txBuffLen);
        
	uint32_t flen = filesystem_get_length(fsys, hfile);
	uint32_t adr = ((uint32_t)rxBuff[4]<<24)|((uint32_t)rxBuff[5]<<16)|
		((uint32_t)rxBuff[6]<<8)|rxBuff[7];
        
	if(adr>=flen)//out of file
		return USBRS_except(2, txBuff, txBuffLen);
        
	uint16_t bytes = ((uint16_t)rxBuff[8]<<8)|rxBuff[9];
        
	if(bytes>txBuffLen || bytes<1 || (adr+bytes)>flen)//out of file
		return USBRS_except(3, txBuff, txBuffLen);
        
	int ret = filesystem_file_get(fsys, hfile, /*file descriptor = number of file in file record table*/
							(int*)&adr, /*it is counter of read bytes, input: start from 0, as output ret pointer for the next non read byte*/
							  (BYTE*)&txBuff[4], /*recepient buffer*/
							  bytes);/*input: buffer len in bytes, output: read len in bytes*/
	if(ret==E_FAIL)
		return USBRS_except(4, txBuff, txBuffLen);
        
	txBuff[2] = HIBYTE_W(ret);
	txBuff[3] = LOBYTE(ret);
        memcpy(txBuff, rxBuff, 2);
	return 4+ret;
}

uint16_t USBRS_writeFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen)
{
        txBuff[0] = rxBuff[0];
	uint16_t fnum = ((uint16_t)rxBuff[2]<<8)|rxBuff[3];
        /*if(flag_EEPROM_Err)
        {
          USBRS_except(4, pUart);
          return;
        }*/
        
	if(fnum>=MAX_FILES)//file number invalid
		return USBRS_except(2, txBuff, txBuffLen);
        
	HFILE hfile = (HFILE)(fnum+1);
	
	uint32_t adr = ((uint32_t)rxBuff[4]<<24)|((uint32_t)rxBuff[5]<<16)|
		((uint32_t)rxBuff[6]<<8)|rxBuff[7];
	WORD bytes = ((WORD)rxBuff[8]<<8)|rxBuff[9];
        
	if(bytes>txBuffLen || bytes<1)//out of file
		return USBRS_except(3, txBuff, txBuffLen);
        
	int ret = filesystem_file_put(fsys, hfile, /*file descriptor = number of file in file record table*/
							(int*)&adr, /*it is counter of read bytes, input: start from 0, as output ret pointer for the next non read byte*/
							  (BYTE*)&rxBuff[10], /*recepient buffer*/
							  bytes);/*input: buffer len in bytes, output: read len in bytes*/
	if(ret==E_FAIL)
		return USBRS_except(4, txBuff, txBuffLen);
        
        memcpy(txBuff, rxBuff, 2);
	return 2;
}


uint16_t USBRS_createFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen)
{
        txBuff[0] = rxBuff[0];
        /*if(flag_EEPROM_Err)
        {
          USBRS_except(4, pUart);
          return;
        }*/
	char fname[FILE_NAME_SZ];
	memset(fname, 0, FILE_NAME_SZ);
	strncpy(fname, (char*)&rxBuff[2], FILE_NAME_SZ);
	char fext[FILE_EXT_SZ];
	memset(fext, 0, FILE_EXT_SZ);
	strncpy(fext, (char*)&rxBuff[2+FILE_NAME_SZ], FILE_EXT_SZ);
	HFILE hfile = filesystem_create_file(fsys, fname, fext, FALSE);
        
	if(hfile==0)
		return USBRS_except(4, txBuff, txBuffLen);
        
	int idx = hfile-1;
	txBuff[2] = HIBYTE_W(idx);
	txBuff[3] = LOBYTE(idx);
        memcpy(txBuff, rxBuff, 2);
	return 4;
}


uint16_t USBRS_deleteFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen)
{
        txBuff[0] = rxBuff[0];
        /*if(flag_EEPROM_Err)
        {
          USBRS_except(4, pUart);
          return;
        }*/
	uint16_t fnum = ((uint16_t)rxBuff[2]<<8)|rxBuff[3];
        
	if(fnum>=MAX_FILES)//file number invalid
		return USBRS_except(2, txBuff, txBuffLen);
        
	HFILE hfile = (HFILE)(fnum+1);
	filesystem_delete_file(fsys, hfile);
        memcpy(txBuff, rxBuff, 2);
	return 2;
}


uint16_t USBRS_getFileNum(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen)
{
        txBuff[0] = rxBuff[0];
        /*if(flag_EEPROM_Err)
        {
          USBRS_except(4, pUart);
          return;
        }*/
	char fname[FILE_NAME_SZ];
	memset(fname, 0, FILE_NAME_SZ);
	strncpy(fname, (char*)&rxBuff[2], FILE_NAME_SZ);
	char fext[FILE_EXT_SZ];
	memset(fext, 0, FILE_EXT_SZ);
	strncpy(fext, (char*)&rxBuff[2+FILE_NAME_SZ], FILE_EXT_SZ);
	HFILE hfile = filesystem_find_file(fsys, fname, fext);
	int idx = hfile-1;
	rxBuff[2] = HIBYTE_W(idx);
	rxBuff[3] = LOBYTE(idx);
        memcpy(txBuff, rxBuff, 2);
	return 4;
}

uint16_t USBRS_except(BYTE exceptCode, char* txBuff, uint16_t txBuffLen)
{
        if(txBuffLen < 3)
        {
          exception(__FILE__,__FUNCTION__,__LINE__,"Litle buff");
          return 0;
        }
	txBuff[1]|=0x80;
	txBuff[2] = exceptCode;
	return 3;
}