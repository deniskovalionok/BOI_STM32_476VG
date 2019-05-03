#ifndef FILESYSTEM_H
#define FILESYSTEM_H


//#include <iolpc2387.h>

#include "types.h"
#include "eeprom.h"

#define CLASTERTABLE_EMPTY_RECORD 0xffff
#define NAMECRCTABLE_EMPTY_RECORD 0xffff
#define MAX_INI_STRING_LEN 100

#define FILE_NAME_SZ 			(int)16
#define FILE_EXT_SZ 			(int)3
#define CLASTER_SZ_IN_SECTORS 	        (int)4
#define MAX_CLASTERS			(int)((int)MAX_SECTORS/(int)CLASTER_SZ_IN_SECTORS)
#define CLASTER_DATA_LEN 		(int)((int)SECTOR_DATA_LEN*(int)CLASTER_SZ_IN_SECTORS)
#define CLASTERTABLE_START_SECTOR_NO 	(int)4
#define CLASTERTABLE_SECTOR_AREA_LEN 		(int)((int)MAX_CLASTERS*(int)sizeof(WORD)/(int)SECTOR_DATA_LEN)
#define CLASTERTABLE_RECORDS_ON_SECTOR 	(int)((int)SECTOR_DATA_LEN/(int)sizeof(WORD))
#define FILERECORD_RECORDS_ON_SECTOR (int)((int)SECTOR_DATA_LEN/(int)sizeof(struct tagFileRecord))

#define NAMECRCTABLE_RECORDS_ON_SECTOR 	(int)((int)SECTOR_DATA_LEN/(int)sizeof(WORD))

struct tagInfo
{
	uint32_t Serial;
	uint8_t  ManufacturedMonth;
	uint16_t ManufacturedYear;
        float    softwareVer;
        float    hardwareVer;
        uint16_t errors;
};

extern struct tagInfo infoRegs;

#pragma pack(1)
struct tagDirEntry
{
	char name[FILE_NAME_SZ];
};
#pragma pack()

#pragma pack(1)
struct tagDateTime
{
  WORD year;
  BYTE month;
  BYTE dayOfMonth;
  BYTE hour;
  BYTE minute;
  BYTE second;
};
#pragma pack()

//size 32 bytes, it means 8 records on sector and 32 records on claster
//
#pragma pack(1)
struct tagFileRecord
{
	char name[FILE_NAME_SZ];
	char ext[FILE_EXT_SZ];
	struct tagDateTime dateTime;
	WORD wdStartClaster;//0xffff means file record is free
	DWORD dwLength;	//len of the file
};
#pragma pack()

struct tagFileSystem
{
	//mean current sector of claster table if 0xff then no claster table was loaded
	//if -1 then no file record was loaded
	int iCurrentClasterTableSector;
        
        int iCurrentNameCRCTableSector;
	//0xffff claster is free

	//=CLASTERTABLE_EMPTY_RECORD mean free claster
	//=on self mean last claster in file
	WORD clasterTableOnSector[CLASTERTABLE_RECORDS_ON_SECTOR];
        
        WORD nameCRCTableOnSector[NAMECRCTABLE_RECORDS_ON_SECTOR];

	//mean current sector of file record
	//if -1 then no file record was loaded
	int iCurrentFileRecordSector;
	//table of file records per one sector

	struct tagFileRecord fileRecordsOnSector[FILERECORD_RECORDS_ON_SECTOR];

	//mean current claster of file was loaded
	//if -1 then no file claster was loaded
	int iCurrentFileClaster;
	int iCurrentFileClasterSector;
	

	
	//must be higher or equal to SECTOR_LEN
	//если этот буфер будет испорчен не соответсвующим загруженному кластеру, то надо сделать следующее
	//filesystem.iCurrentFileClaster=-1;
	//filesystem.iCurrentFileClasterSector=-1;
	BYTE clasterBuffer[CLASTER_DATA_LEN+8];
	

	
	int file_pos_last_value;
	int file_pos_last_section;
	char lastSectionName[256];
	
	HFILE hFileExecuteSys;//HFILE of execute.sys, =NULL if no set                                         //Наверное вернуть надо будет!!!!!
	unsigned int uiCalcFreeMem;
	BOOL bMemoryLow;
        
        struct tagEEPROMControl *eepromc;
        
	
};


#define MAX_FILE_RECORDS			(int)10240//25472
#define MAX_FILES 					(int)MAX_FILE_RECORDS

//сдвинули таблицу файлов изза появления резерва таблицы кластеров
#define FILERECORD_START_SECTOR_NO 	(int)((int)CLASTERTABLE_START_SECTOR_NO+(int)CLASTERTABLE_SECTOR_AREA_LEN+(int)CLASTERTABLE_SECTOR_AREA_LEN)
#define FILERECORD_SECTOR_AREA_LEN	(int)((int)MAX_FILE_RECORDS/(int)FILERECORD_RECORDS_ON_SECTOR)

//сдвинули файловую область изза появления резерва таблицы файлов
#define FILES_SECTOR_START_NO 		(int)((int)NAMECRCTABLE_START_SECTOR_NO+(int)NAMECRCTABLE_SECTOR_AREA_LEN+(int)NAMECRCTABLE_SECTOR_AREA_LEN)
#define FILES_SECTOR_AREA_LEN 		(int)((int)MAX_SECTORS-(int)FILES_SECTOR_START_NO)


#define FILES_CLASTER_START_NO 		(int)((int)FILES_SECTOR_START_NO/(int)CLASTER_SZ_IN_SECTORS)
#define FILES_CLASTER_AREA_LEN 		(int)((int)FILES_SECTOR_AREA_LEN/(int)CLASTER_SZ_IN_SECTORS)
#define FILES_CLASTER_END_NO 		(int)((int)FILES_CLASTER_START_NO+(int)FILES_CLASTER_AREA_LEN-1)

#define FILERECORD_START_NO		(int)0
#define FILERECORD_END_NO		(int)((int)MAX_FILES-1)

#define NAMECRCTABLE_START_SECTOR_NO    (int)((int)FILERECORD_START_SECTOR_NO+(int)FILERECORD_SECTOR_AREA_LEN+(int)FILERECORD_SECTOR_AREA_LEN)
#define NAMECRCTABLE_SECTOR_AREA_LEN    (int)((int)MAX_FILE_RECORDS*(int)sizeof(WORD)/(int)SECTOR_DATA_LEN)

//in KB
#define TOTAL_MEMORY	(FILES_CLASTER_AREA_LEN*CLASTER_DATA_LEN/1024)




extern struct tagFileSystem filesystem;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//system functions
void filesystem_Init(struct tagFileSystem *fsys, struct tagEEPROMControl *eepromc);


int filesystem_get_clasterIndex(struct tagFileSystem *fsys, int claster_number);
int filesystem_get_filerecordIndex(struct tagFileSystem *fsys, int filerecord);
BOOL filesystem_read_filerecordstable(struct tagFileSystem *fsys, int sector_num);
BOOL filesystem_write_filerecordstable(struct tagFileSystem *fsys);
BOOL filesystem_read_clastertable(struct tagFileSystem *fsys, int sector);
BOOL filesystem_write_clastertable(struct tagFileSystem *fsys);

BOOL filesystem_read_nameCRCtable(struct tagFileSystem *fsys, int sector);
BOOL filesystem_write_nameCRCtable(struct tagFileSystem *fsys);

BOOL filesystem_read_claster(struct tagFileSystem *fsys, int claster_number);
BOOL filesystem_write_claster(struct tagFileSystem *fsys, int claster_number);

void filesystem_format_filetable(struct tagFileSystem *fsys, BOOL bYes);

HFILE filesystem_find_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file, will be found and rewrite*/
							const char* pFileExt /*ext of the file*/
						  );
int filesystem_find_free_filerecord(struct tagFileSystem *fsys);
int filesystem_read_from_position(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int file_pos /*it is counter or read bytes, input: start from 0, as output ret pointer for the next non read byte*/
							);
int filesystem_read_from_position_withoutErr(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int file_pos /*it is counter or read bytes, input: start from 0, as output ret pointer for the next non read byte*/
							);

int filesystem_find_free_claster(struct tagFileSystem *fsys, int notThisClaster);

int filesystem_get_first_filerecordIndex(struct tagFileSystem *fsys, HFILE hFile);

HRESULT filesystem_file_change_len(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
							   int len /*new len of the file*/
								   );

int filesystem_get_nameCRCIndex(struct tagFileSystem *fsys, int file_number);

WORD CalcNameCRC(const char* pFileName);


int filesystem_get_filename(struct tagFileSystem *fsys, HFILE hFile, /*hfile of file we are looking for name and ext*/
							char* pFileName, /*name of the file*/
							char* pFileExt /*ext of the file*/
								);

int filesystem_copy_file_data(struct tagFileSystem *fsys, HFILE hFileDst, HFILE hFileSrc, int file_pos_dst, int file_pos_src, int len);

int filesystem_cut_file(struct tagFileSystem *fsys, HFILE hFile, /*file that will be cut*/
						 int file_pos, /*file pos from cut*/
						 int bytes_cut /*bytes to cut >0, if <0 then grows*/
							 );












///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//user function



/////////////////////////////////////////
//file function
int filesystem_rename_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file*/
							const char* pFileExt, /*ext of the file*/
						   const char* pNewFileName, /*name of the file, will be found and rewrite*/
							const char* pNewFileExt /*ext of the file*/
								);
int filesystem_get_dir(struct tagFileSystem *fsys, const char* pFileExt, /*ext of file, if NULL then all files */
						struct tagDirEntry* pDir, /*output directory entries*/
						int itemsNum, /*items number of pDir*/
						int itemsPass	/*items number must be passed*/
							);
int filesystem_file_get(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int* pFile_pos, /*it is counter or read bytes, input: start from 0, as output ret pointer for the next non read byte*/
						  BYTE* pBuffer, /*recepient buffer*/
						  int len);/*input: buffer len in bytes, output: read len in bytes*/

int filesystem_file_put(struct tagFileSystem *fsys, HFILE hFile, /*file descriptor = number of file in file record table*/
						int* pFile_pos, /*it is counter or written bytes, input: start from 0, as output ret pointer for the next non written byte*/
						  const BYTE* pBuffer, /*send buffer*/
						  int len);/*buffer len in bytes*/
HRESULT filesystem_delete_file(struct tagFileSystem *fsys, HFILE hfile);
DWORD filesystem_get_length(struct tagFileSystem *fsys, HFILE hFile /*file descriptor = number of file in file record table*/
							);
HFILE filesystem_create_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file, will be found and rewrite*/
							const char* pFileExt, /*ext of the file*/
							BOOL bOverwrite	/*1 then exists file will be overwritten*/
						  );
HFILE filesystem_open_file(struct tagFileSystem *fsys, const char* pFileName, /*name of the file, will be found*/
						   const char* pFileExt /*ext of the file*/
						  );


/////////////////////////////////////////
//test function
void filesystem_test(void);
void filesystem_check_and_restore(struct tagFileSystem *fsys);

void filesistem_check_sect_CRC(void);

UINT filesystem_calc_files_number(struct tagFileSystem *fsys, char* pExt);
UINT filesystem_calc_free_memory(struct tagFileSystem *fsys);

HFILE filesystem_open_first(struct tagFileSystem *fsys, char* pExt);

BOOL filesystem_check_file_exists(struct tagFileSystem *fsys, HFILE hFile);

void filesystem_detect_memorylow(struct tagFileSystem *fsys);
void filesystem_show_symbol(int x);

//подсичтать количество специальных файлов
void filesystem_calc_special_files_number(struct tagFileSystem *fsys, UINT* pAllFilesNum, 
					UINT* pSPZFilesNum,
					UINT* pLIBFilesNum,
					UINT* pMCSFilesNum,
					UINT* pLOGFilesNum);


int filesystem_get_string(struct tagFileSystem *fsys, HFILE hFile, int* pFile_pos, char* pString, int string_len);
int filesystem_ini_get_int(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, int* pValue);
int filesystem_ini_get_string(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, char* pValue, int value_len);
int filesystem_find_value(struct tagFileSystem *fsys, HFILE hFile, int start_file_pos, const char* pName);
int file_system_find_section(struct tagFileSystem *fsys, HFILE hFile, const char* pSection);

HFILE filesystem_open_firstEx(struct tagFileSystem *fsys, char* pName, char* pExt);
HFILE filesystem_open_num(struct tagFileSystem *fsys, char* pExt, int num);

uint16_t USBRS_readDir(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen);
uint16_t USBRS_readFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen);
uint16_t USBRS_writeFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen);
uint16_t USBRS_createFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen);
uint16_t USBRS_deleteFile(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen);
uint16_t USBRS_getFileNum(struct tagFileSystem *fsys, char* txBuff, uint16_t txBuffLen, char* rxBuff, uint16_t rxBuffLen);
uint16_t USBRS_except(BYTE exceptCode, char* txBuff, uint16_t txBuffLen);

int filesystem_ini_put_string(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, const char* pValue);
int filesystem_ini_put_int(struct tagFileSystem *fsys, HFILE hFile, const char* pSection, const char* pName, int value);

#endif