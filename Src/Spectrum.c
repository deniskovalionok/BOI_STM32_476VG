#include "Spectrum.h"
#include "identify.h"
#include <math.h>

BOOL Spectrum_peakProc_ex(float *position, float* sigma)
{
	long pos = (long)(*position);

	//в окне двух сигм с каждой стороны будем искать максимум
	long sig = 2*SigmaArray[pos]/256
#ifdef _THIN_SIGMA
				*SIGMA_THIN_FACTOR_M/SIGMA_THIN_FACTOR_D
#endif	//#ifdef _THIN_SIGMA
		;
	
	long left = pos-sig;
	long right = pos+sig;
	if(left<1)left = 1;
	if(right>=CHANNELS-1)right = CHANNELS-2;
	if(left>right)return FALSE;
	
	long val, max=0;
	for(int i=left;i<=right;i++)
	{
		val = identifyControl.ISpectrum[i];
		if(val>max)
		{
			max = val;
			pos = i;
		}
	}
	//нашли максимум теперь средневзвешенным найдем центр пика по окну 2 сигмы с каждой стороны
	sig = 2*SigmaArray[pos]/256
#ifdef _THIN_SIGMA
				*SIGMA_THIN_FACTOR_M/SIGMA_THIN_FACTOR_D
#endif	//#ifdef _THIN_SIGMA
		;
	left = pos-sig;
	right = pos+sig;
	if(left<1)left = 1;
	if(right>=CHANNELS-1)right = CHANNELS-2;
	if(left>right)return FALSE;
	
	long long summ = 0;
	long long msumm = 0;
	long long mean;
	for(int i=left;i<=right;i++)
	{
		mean = (long long)identifyControl.ISpectrum[i];
		mean*=mean;
		summ+=mean;
		msumm+=mean*i;
	}
	
	if(summ<=0)return FALSE;	//empty spectrum

	//определили центр по средневзвешенному
	//теперь задаем края по 3,5 сигмы с каждой стороы и обрабатываем пик
	*position = (float)msumm/(float)summ;
	pos = (long)(*position+0.5);
	
	sig = (int)(3.5*SigmaArray[pos]/256)
#ifdef _THIN_SIGMA
				*SIGMA_THIN_FACTOR_M/SIGMA_THIN_FACTOR_D
#endif	//#ifdef _THIN_SIGMA
		;
	left = pos-sig;
	right = pos+sig;
	//обязательно с первого по последний -1 канал чтобы сделать усреднение по краям по три точки
	if(left<1)left = 1;
	if(right>=CHANNELS-1)
		right = CHANNELS-2;
	return Spectrum_QuickPeakCalculation(left,right,position,sigma);
}



//быстрая обработка пика
//задаем левую и правую границу и положение центра,
//возврат точная позиция и сигма
//если возврат FALSE то расчет не удался
BOOL Spectrum_QuickPeakCalculation(long left, long right, float *position, float *sigma)
{
	float s,d2,d,d1,k,b;
	if(right-left<=0)return FALSE;
	long lng = (long)(identifyControl.ISpectrum[left-1]+identifyControl.ISpectrum[left]+identifyControl.ISpectrum[left+1]);
	k=(float)((long)(identifyControl.ISpectrum[right-1]+identifyControl.ISpectrum[right]+identifyControl.ISpectrum[right+1])-
			  lng)/(float)(3*(right-left));
	b=(float)lng/3.0-k*left;
	d=0; d1=0; d2=0;
	for(long i=left; i<=right; i++){
		s=((float)identifyControl.ISpectrum[i]-k*i-b); if(s<0) s=0;
		d+=s; s*=i; d1+=s; s*=i; d2+=s;
	}
	if(d<=0)return FALSE;
	(*position)=d1/d;
	k=(d2/d-(*position)*(*position));
	if(k<=0)return FALSE;
	(*sigma)=sqrt(k);
	return TRUE;
}
/*
int Spectrum_save(char* pFileName, BOOL bOverwrite)
{
	if(powerControl.bBatteryAlarm)return -2;	//battery alarm
	
	int iret = 1;
	
	//check that file exists
	HFILE hfile = filesystem_find_file(pFileName, "spz");
	if(hfile!=NULL && !bOverwrite)
	{//file exists
		//do no allow to rewrite file
		//change the name of spectrum
		int count = 777;
		do
		{
			PowerControl_kickWatchDog();
			Spectrum_newfilename(pFileName);
			hfile = filesystem_find_file(pFileName, "spz");
		}while(hfile!=NULL && --count);//file exists

		iret = -1;
	}

	
	hfile = filesystem_create_file(pFileName, //name of the file, will be found and rewrite
						"spz", //ext of the file
						bOverwrite	//1 then exists file will be overwritten
					  );
	if(hfile==NULL)return 0;//!!!!!!!! have to show MSG, not exception
//		exception(__FILE__,__FUNCTION__,__LINE__,"No free memory");
	int file_pos = 0;

	
	
	BYTE buf[256];
	BYTE* pBuf = buf;
	int len = 0;
	memcpy(pBuf, &spectrumControl.pShowSpectrum->wAcqTime, 2);
	pBuf+=sizeof(spectrumControl.pShowSpectrum->wAcqTime);
	len+=sizeof(spectrumControl.pShowSpectrum->wAcqTime);
	memcpy(pBuf, &spectrumControl.pShowSpectrum->dateTime, sizeof(spectrumControl.pShowSpectrum->dateTime));
	pBuf+=sizeof(spectrumControl.pShowSpectrum->dateTime);
	len+=sizeof(spectrumControl.pShowSpectrum->dateTime);
	memcpy(pBuf, &spectrumControl.pShowSpectrum->commonGPS, sizeof(spectrumControl.pShowSpectrum->commonGPS));
	pBuf+=sizeof(spectrumControl.pShowSpectrum->commonGPS);
	len+=sizeof(spectrumControl.pShowSpectrum->commonGPS);
	memcpy(pBuf, &spectrumControl.pShowSpectrum->fTemperature, sizeof(spectrumControl.pShowSpectrum->fTemperature));
	pBuf+=sizeof(spectrumControl.pShowSpectrum->fTemperature);
	len+=sizeof(spectrumControl.pShowSpectrum->fTemperature);
	
	char temprep[MAX_REPORT_SYMS];
	memset(temprep,0,sizeof(temprep));
	char tempnuc[NUCLNAMELENGTH*3];
	int reti;
	for(int i=0;i<identifyControl.NUCLNUM;i++)
	{
		reti = identify_getnuclidetxt(i, tempnuc);
		if(reti==0)continue;	//никлид не идентифицирован
		if(reti==-1)break;//	the end
		if((MAX_REPORT_SYMS-strlen(temprep))<=strlen(tempnuc))break;	//больше места нет в буфере
		strncat(temprep, tempnuc, NUCLNAMELENGTH*3);
	}
	
	memcpy(pBuf, &temprep, sizeof(temprep));
	pBuf+=sizeof(temprep);
	len+=sizeof(temprep);
	
	long pval = spectrumControl.pShowSpectrum->dwarSpectrum[0];
	long nval;
	long rval;
	//store first channel as absolute value
	memcpy(pBuf, &pval, sizeof(pval));
	pBuf+=sizeof(pval);
	len+=sizeof(pval);
	for(int i=1;i<CHANNELS;i++)
	{
		if(len>252)
		{//buffer is full, will save in file
			int ret = filesystem_file_put(hfile, //file descriptor = number of file in file record table
									&file_pos, //it is counter or written bytes, input: start from 0, as output ret pointer for the next non written byte
									  (BYTE*)buf, //send buffer
									  len);//buffer len in bytes
			if(ret==E_FAIL)
			{
				filesystem_delete_file(hfile);
				return 0;//!!!!!!!! have to show MSG, not exception
			}
				//exception(__FILE__,__FUNCTION__,__LINE__,"No free memory");
			len = 0;
			pBuf = buf;
		}

		nval = (long)spectrumControl.pShowSpectrum->dwarSpectrum[i];
		rval = nval-pval;
		pval = nval;
		if(rval>32767 || rval<-32768)
		{//3 bytes on channel + 1 mark byte
			*pBuf++ = 0x7f;
			memcpy(pBuf, &rval, 3);
			pBuf+=3;
			len+=4;
		}else if(rval>126 || rval<-127)
		{//2 bytes on channel + 1 mark byte
			*pBuf++ = 0x80;
			memcpy(pBuf, &rval, 2);
			pBuf+=2;
			len+=3;
		}else
		{//1 byte on channel
			*pBuf++=(signed char)rval;
			len++;
		}
	}

	if(len>0)
	{//buffer is full, will save in file
		int ret = filesystem_file_put(hfile, //file descriptor = number of file in file record table
								&file_pos, //it is counter or written bytes, input: start from 0, as output ret pointer for the next non written byte
								  (BYTE*)buf, //send buffer
								  len);//buffer len in bytes
		if(ret==E_FAIL)//!!!!!!!! have to show MSG, not exception
		{
			filesystem_delete_file(hfile);
			return 0;//!!!!!!!! have to show MSG, not exception
		}
//			exception(__FILE__,__FUNCTION__,__LINE__,"No free memory");
	}


	
	//вторая версия файла спектра
	//save doserate
	len = 0;
	pBuf = buf;
	memcpy(pBuf, &spectrumControl.pShowSpectrum->fDoserate, sizeof(spectrumControl.pShowSpectrum->fDoserate));
	pBuf+=sizeof(spectrumControl.pShowSpectrum->fDoserate);
	len+=sizeof(spectrumControl.pShowSpectrum->fDoserate);
	memcpy(pBuf, &spectrumControl.pShowSpectrum->wRealTime, sizeof(spectrumControl.pShowSpectrum->wRealTime));
	pBuf+=sizeof(spectrumControl.pShowSpectrum->wRealTime);
	len+=sizeof(spectrumControl.pShowSpectrum->wRealTime);
	memcpy(pBuf, &spectrumControl.pShowSpectrum->fCps, sizeof(spectrumControl.pShowSpectrum->fCps));
	pBuf+=sizeof(spectrumControl.pShowSpectrum->fCps);
	len+=sizeof(spectrumControl.pShowSpectrum->fCps);
	
	if(len>0)
	{//buffer is full, will save in file
		int ret = filesystem_file_put(hfile, //file descriptor = number of file in file record table
								&file_pos, //it is counter or written bytes, input: start from 0, as output ret pointer for the next non written byte
								  (BYTE*)buf, //send buffer
								  len);//buffer len in bytes
		if(ret==E_FAIL)//!!!!!!!! have to show MSG, not exception
		{
			filesystem_delete_file(hfile);
			return 0;//!!!!!!!! have to show MSG, not exception
		}
//			exception(__FILE__,__FUNCTION__,__LINE__,"No free memory");
	}

	return iret;
}
*/