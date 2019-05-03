//identify.c


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "identify.h"

#include "exeptions.h"

struct tagIdentify identifyControl;




void identify_InitIdent(long* spec, long* energy, long* sigma, int numOfCanals)
{
        identifyControl.energy = energy;
        identifyControl.sigma = sigma;
	identifyControl.ISpectrum = spec;
	identifyControl.collectionTime = 0;
	identifyControl.CHANK =0;
	identifyControl.CHANB=0;
	identifyControl.NUMCHAN = numOfCanals;
	identifyControl.testNullPeak=1;
	identifyControl.MAXENERGY=3000;	
	identifyControl.MINENERGY=20;
	identifyControl.NUCLNUM=0;
	identifyControl.nsigma_searchpeaks=16;
	identifyControl.LEFTBORDER=0;
	identifyControl.SCALEINSTABILITY=20;
	identifyControl._nuclsDiffs = 128;	//������� � ����� ��������: 128-2 ����; 170-1.5 ����

	identifyControl.nsigma_peakcheck_left=26;
	identifyControl.nsigma_peakcheck_right=24;
	identifyControl.msigma_peakcheck=20;
	identifyControl.nLine = 0;
	identifyControl.threshold = 3;
	
	identifyControl.bHasLibrary = FALSE;
	identifyControl.iNuclsIdentified = 0;
	identifyControl.bHaveAlreadyResult = FALSE;
	identifyControl.bHaveUnknownResult = FALSE;

	identifyControl.identifyStartDeadTime = 120;	//start time to stop identify if no nuclides
	identifyControl.identifyDeadTime = identifyControl.identifyStartDeadTime;	//current time to stop identify if no nuclides

	
	memset(identifyControl.report, 0, sizeof(identifyControl.report));
}

//read settings from ini
//!!!!!!! must be run before identify_open_library
//ret E_FAIL if error
int identify_read_identify_ini(void)
{
	//read library file from identify.ini
	HFILE hfile = filesystem_open_file(&filesystem, "identify", /*name of the file, will be found*/
		   "ini" /*ext of the file*/
			   );
	if(hfile==NULL)
		return E_FAIL;

	/*
"[identifyControl]\r"
"MINENERGY=\r"
"MAXENERGY=\r"
"nsigma_searchpeaks=\r"
"SCALEINSTABILITY=\r"
"testNullPeak=\r"
"nsigma_peakcheck_left=\r"
"nsigma_peakcheck_right=\r"
"msigma_peakcheck=\r"
"libraryFileName=main\r";
	*/
	
	int ret = filesystem_ini_get_string(&filesystem, hfile, "identifyControl", "libraryFileName", identifyControl.libraryFileName, FILE_NAME_SZ);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "MINENERGY", &identifyControl.MINENERGY);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "MAXENERGY", &identifyControl.MAXENERGY);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "SCALEINSTABILITY", &identifyControl.SCALEINSTABILITY);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "testNullPeak", (int*)&identifyControl.testNullPeak);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "nsigma_peakcheck_left", (int*)&identifyControl.nsigma_peakcheck_left);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "nsigma_peakcheck_right", (int*)&identifyControl.nsigma_peakcheck_right);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "msigma_peakcheck", (int*)&identifyControl.msigma_peakcheck);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "nsigma_searchpeaks", &identifyControl.nsigma_searchpeaks);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "threshold", (int*)&identifyControl.threshold);	
	if(ret==E_FAIL)
		return E_FAIL;
	ret = filesystem_ini_get_int(&filesystem, hfile, "identifyControl", "identifyStartDeadTime", &identifyControl.identifyStartDeadTime);	
	if(ret==E_FAIL)
		return E_FAIL;
	return S_OK;
}



//write int value to system  ini
//if failed then exception
BOOL identify_write_identify_ini_int(const char * pSection, const char* pValueName, int value)
{
	//read library file from identify.ini
	HFILE hfile = filesystem_open_file(&filesystem, "identify", /*name of the file, will be found*/
		   "ini" /*ext of the file*/
			   );
	if(hfile==NULL)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Invalid identify.ini");
	}

	int ret = filesystem_ini_put_int(&filesystem, hfile, pSection, pValueName, value);
	return (BOOL)(ret!=E_FAIL);
}







BOOL identify_write_identify_ini_string(const char * pSection, const char* pValueName, const char* value)
{
	//read library file from identify.ini
	HFILE hfile = filesystem_open_file(&filesystem, "identify", /*name of the file, will be found*/
		   "ini" /*ext of the file*/
			   );
	if(hfile==NULL)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Invalid identify.ini");
	}

	int ret = filesystem_ini_put_string(&filesystem, hfile, pSection, pValueName, value);
	return (BOOL)(ret!=E_FAIL);
}












//open library from file
//if ret E_FAIL then no lib file or it is bad
int identify_open_library(void)
{
	//open lib
	HFILE hfile = filesystem_open_file(&filesystem, identifyControl.libraryFileName, /*name of the file, will be found*/
		   "lib" /*ext of the file*/
			   );
	if(hfile==NULL)
		return E_FAIL;	//no lib file
	
	int ret = filesystem_ini_get_int(&filesystem, hfile, "common", "dwLibVer", &identifyControl.dwLibVer);
	if(ret==E_FAIL)
		return E_FAIL;	//invalid lib file, no version info
	
	ret = filesystem_ini_get_int(&filesystem, hfile, "common", "NUCLNUM", &identifyControl.NUCLNUM);
	if(ret==E_FAIL || identifyControl.NUCLNUM>NUMNUCLIDES)
		return E_FAIL;	//invalid lib file, no nuclides number, or num nucl is too big
	
	
	//read by nuclide
	char section[10];
	char value_name[10];
	char buf[11];
	int numlines;
	int ival;
	for(int i=0;i<identifyControl.NUCLNUM;i++)
	{
		sprintf(section, "n_%d", i+1);
		int ret = filesystem_ini_get_string(&filesystem, hfile, section, "N", buf, sizeof(buf));	
		if(ret==E_FAIL)
			return E_FAIL;
		//process name
		identifyControl.Nucls[i].category = buf[0];	//get category
		char* pStr = strchr(buf, '-');
		if(pStr==NULL)return E_FAIL;	//invalid nuclide name
		pStr++;
		char* pStr2 = strchr(pStr, '-');
		if(pStr2==NULL)return E_FAIL;	//invalid nuclide name
		int len = pStr2-pStr;
		if(len>NUCLNAMELENGTH)return E_FAIL;
		memset(identifyControl.Nucls[i].Name, 0,NUCLNAMELENGTH);
		strncpy(identifyControl.Nucls[i].Name, pStr,len);
		pStr2++;
		len = strlen(pStr2);
		if(len>NUCLNAMELENGTH)return E_FAIL;
		memset(identifyControl.Nucls[i].NumStr, 0,NUCLNAMELENGTH);
		strncpy(identifyControl.Nucls[i].NumStr, pStr2,len);
		//name resolved
		ret = filesystem_ini_get_int(&filesystem, hfile, section, "Ln", &numlines);
		if(ret==E_FAIL || numlines>MAXLINES)
			return E_FAIL;
		identifyControl.Nucls[i].num = numlines;
		identifyControl.Nucls[i].confidence = 0;
		//get lines
		for(int j=0;j<numlines;j++)
		{
			//energy
			sprintf(value_name, "L%de", j+1);
			ret = filesystem_ini_get_int(&filesystem, hfile, section, value_name, &ival);
			if(ret==E_FAIL)
				return E_FAIL;
			identifyControl.Nucls[i].energies[j] = ival;
			
			//no shield factor
			sprintf(value_name, "L%dn", j+1);
			ret = filesystem_ini_get_int(&filesystem, hfile, section, value_name, &ival);
			if(ret==E_FAIL)
				return E_FAIL;
			identifyControl.Nucls[i].factors_noshield[j] = ival;
			
			//shield factor
			sprintf(value_name, "L%ds", j+1);
			ret = filesystem_ini_get_int(&filesystem, hfile, section, value_name, &ival);
			if(ret==E_FAIL)
				return E_FAIL;
			identifyControl.Nucls[i].factors_shield[j] = ival;
		}
	}
	identifyControl.bHasLibrary = TRUE;
	return S_OK;
}













void identify_InitSigma1()
{
	if(identifyControl.ISpectrum == NULL)return;
	int i, j;
	i=identifyControl.NUMCHAN/2-50;
	j = pEnergy[i+100]-pEnergy[i];
	if(!j)return;
	identifyControl.CHANK=25600/j;
	identifyControl.CHANB=(long)i*256-identifyControl.CHANK*pEnergy[i];
	i=identifyControl.NUMCHAN-((/*1 to 3*/1*SigmaArray[identifyControl.NUMCHAN-1]+128)>>8);
	if(i>(identifyControl.NUMCHAN>>1)) identifyControl.MAXENERGY=pEnergy[i];
	else identifyControl.MAXENERGY=pEnergy[identifyControl.NUMCHAN-1];
}

/*short identify_ChannelFromEnergy(short energy)
{
	short k=(short)((identifyControl.CHANK*energy+identifyControl.CHANB)>>8);
	if (k<0) k=0;
	else if(k>=identifyControl.NUMCHAN) k=identifyControl.NUMCHAN-1;
	if (pEnergy[k]>energy)
		while (pEnergy[k]>energy){
			if(k) k--;
			else break;
		}
	else
		while (pEnergy[k]<energy){
			if(k<identifyControl.NUMCHAN-1) k++;
			else break;
		}
	return k;
}*/


int identify_ChannelFromEnergyNear(int energy)
{
	int k=(int)((identifyControl.CHANK*energy+identifyControl.CHANB)>>8);
	if (k<0) k=0;
	else if(k>=identifyControl.NUMCHAN) k=identifyControl.NUMCHAN-1;
	if (pEnergy[k]>energy)
	{
		while (pEnergy[k]>energy)
		{
			if(k)
				k--;
			else
				break;
		}
		if(abs(pEnergy[k+1]-energy) < abs(energy-pEnergy[k]))
			k++;
	}
	else
	{
		while (pEnergy[k]<energy)
		{
			if(k<identifyControl.NUMCHAN-1)
				k++;
			else
				break;
		}
		if(abs(energy-pEnergy[k-1]) <= abs(pEnergy[k]-energy))
			k--;
	}
	return k;
}

/*
������� ��������� ������ �������� ��������� ���������
*/
void identify_getnuclidesinreport(char sym, const char* pHeader)
{
	int amount = 0;
	for(int i=0;i<identifyControl.NUCLNUM;i++)
	{
		if(identifyControl.Nucls[i].selected==1 && identifyControl.Nucls[i].category==sym)
		{//identified
			if((sizeof(identifyControl.report)-strlen(identifyControl.report)) <= (1+2*NUCLNAMELENGTH))
				return;
			if(amount==0)
			{
				if(identifyControl.iNuclsIdentified>0)
				{
					strcat(identifyControl.report, "\r");
				}
				strcat(identifyControl.report, pHeader);
			}else
			{
				if(amount>0)
					strcat(identifyControl.report, " ");
			}
//			char buf[12];
	//		sprintf(buf,"%u-",(UINT)identifyControl.Nucls[i].confidence);
		//	strcat(identifyControl.report, buf);
			strncat(identifyControl.report, identifyControl.Nucls[i].NumStr, NUCLNAMELENGTH);
			strncat(identifyControl.report, identifyControl.Nucls[i].Name, NUCLNAMELENGTH);
			amount++;
			identifyControl.iNuclsIdentified++;
			identifyControl.bHaveAlreadyResult = TRUE;
		}
	}
}


//this is additional check for unknown, if some nuclides are questionable
void identify_checkForUnknown(void)
{
	for(int i=0;i<identifyControl.NUCLNUM;i++)
	{
		if(identifyControl.Nucls[i].selected==0)
		{
			identifyControl.bHaveUnknownResult = TRUE;
			break;
		}
	}
	












}



/*
�������� ������ ������� �� ������� ��� ������ � ���� �������
ret 0 if nuclide is not identified
ret -1 if no more nuclides
ret 1 if ok
*/
int identify_getnuclidetxt(int i, char* pstrNuc)
{
	if(i>=NUMNUCLIDES || i>=identifyControl.NUCLNUM)return -1;
	if(identifyControl.Nucls[i].selected!=1)return 0;
	//identified
	pstrNuc[0] = identifyControl.Nucls[i].category;
	pstrNuc[1] = '-';
	pstrNuc[2] = 0;
	strncat(pstrNuc, identifyControl.Nucls[i].Name, NUCLNAMELENGTH);
	strncat(pstrNuc, "-", NUCLNAMELENGTH);
	strncat(pstrNuc, identifyControl.Nucls[i].NumStr, NUCLNAMELENGTH);
	strncat(pstrNuc, "[", NUCLNAMELENGTH);
	char buf[2];
	buf[0]= '0'+identifyControl.Nucls[i].confidence;
	buf[1] = 0;
	strncat(pstrNuc, buf, NUCLNAMELENGTH);
	strncat(pstrNuc, "]; ", NUCLNAMELENGTH);
	return  1;
}



void identify_clearReport(void)
{
	memset(identifyControl.report, 0, sizeof(identifyControl.report));
	identifyControl.iNuclsIdentified = 0;
	identifyControl.bHaveAlreadyResult = FALSE;
	identifyControl.bHaveUnknownResult = FALSE;
}

void identify_prepareReport(BOOL bAddCategory)
{
        /*
	const char strInd[] = "Industrial: \0""Industrie: \0""Industrial: \0""������������: ";
	const char strMed[] = "Medicine: \0""Medizin: \0""Medicine: \0""�����������: ";
	const char strNuc[] = "Nuclear: \0""Nuklear: \0""Nuclear: \0""�������: ";
	const char strNat[] = "NORM: \0""NORM: \0""NORM: \0""������������: ";
	*/
  
        const char strInd[] = "Industrial: \0";
	const char strMed[] = "Medicine: \0";
	const char strNuc[] = "Nuclear: \0";
	const char strNat[] = "NORM: \0";
        
	const char* pText="";
	//if(bAddCategory)
	//	pText = Display_getTextByLang(strInd);
        pText =  strInd;
	identify_getnuclidesinreport('I',pText);
	pText="";
	//if(bAddCategory)
	//	pText = Display_getTextByLang(strMed);
        pText =  strMed;
	if((sizeof(identifyControl.report)-strlen(identifyControl.report)) <= sizeof(pText))
		return;
	identify_getnuclidesinreport('M',pText);
	pText="";
	//if(bAddCategory)
	//	pText = Display_getTextByLang(strNuc);
        pText =  strNuc;
	if((sizeof(identifyControl.report)-strlen(identifyControl.report)) <= sizeof(pText))
		return;
	identify_getnuclidesinreport('U',pText);
	pText="";
	//if(bAddCategory)
	//	pText = Display_getTextByLang(strNat);
        pText =  strNat;
	if((sizeof(identifyControl.report)-strlen(identifyControl.report)) <= sizeof(pText))
		return;
	identify_getnuclidesinreport('N',pText);
#ifndef BNC	
	//show unknown only for NON BNC version
	if(identifyControl.bHaveUnknownResult)
	{
		strcat(identifyControl.report, "UNKNOWN\0");
	}else
#endif
	if(!identifyControl.bHaveAlreadyResult)
	{
		strcat(identifyControl.report, "NOTHING\0");
	}

}



void identify_identify(BOOL bAddCategory)
{
	//identifyControl.ISpectrum = spectrumControl.pShowSpectrum->dwarSpectrum;
        if(identifyControl.ISpectrum == NULL)
          return;

	if(!identifyControl.bHasLibrary)
	{
		identify_clearReport();
		//strcat(identifyControl.report, Display_getTextByLang("DISABLED\0""DISABLED\0""DISABLED\0""�������������"));
	}else
	{
		//PowerControl_turboModeON();
		identify_clearReport();
		identify_DetectLines();
		identifyControl.bHaveUnknownResult = (identify_MakeNuclideIdentification()==-1)?(BOOL)TRUE:(BOOL)FALSE;
		identify_checkForUnknown();
		identify_prepareReport(bAddCategory);
		//PowerControl_turboModeOFF();
	}
	//strncpy(spectrumControl.pShowSpectrum->report,identifyControl.report,MAX_REPORT_SYMS);
}

//just convolution
BOOL identify_convolute(void)
{
	if(!identifyControl.ISpectrum
	   || !identifyControl.bHasLibrary)
	{
		return FALSE;
	}else
	{
		//PowerControl_turboModeON();
		identify_DetectLines();
		//PowerControl_turboModeOFF();
		return TRUE;
	}
}






void identify_DetectLines(void){
	identifyControl.LEFTBORDER=identify_ChannelFromEnergyNear(identifyControl.MINENERGY);
	identify_ApplyFilter(); //filters spectrum ISpectrum and places
				   //result in the identifyControl.BufSpec array and SD in the identifyControl.SDs array
	identify_Smoothing(); //smooths identifyControl.BufSpec for nicer look (averaging filter with window equal to sigma)
	identify_AnalyzePeaks(); //Fills Peak array with data, basing on the analysis of identifyControl.BufSpec filtered spectrum
}

/*
void identify_DetectLinesEx(int threshold, long  *Variance) //the same as DetectLines, plus takes spectrum variance from Variance (has to be used, if phon is subtracted)
{
	identifyControl.LEFTBORDER=identify_ChannelFromEnergyNear(identifyControl.MINENERGY);
	//Terminated=0;
	identify_ApplyFilterEx(Variance); //filters spectrum ISpectrum and places
				   //result in the identifyControl.BufSpec array and SD in the identifyControl.SDs array
//	if(Terminated) return;
	identify_Smoothing(); //smooths identifyControl.BufSpec for nicer look (averaging filter with window equal to sigma)
//	if(Terminated) return;
	identifyControl.varspec=Variance;
	identify_AnalyzePeaks(threshold); //Fills Peak array with data, basing on the analysis of identifyControl.BufSpec filtered spectrum
}
*/


#define sign(x) ((x)<0?-1:1)

/*UCHAR identify_GShape(short j, FIXED2 *sigma)
{
	long r;
	if(j<0) j=-j;
	r=((long)j*GSHPSIGM)<<8;
	r=r/sigma->w;
	if (r<0) return 0;
	else
		if(r<GSHPNUM) return GaussShape[r];
		else return 0;
}
*/

/*char identify_SShape(short j, FIXED2 *sigma)
{
	long r;
	if(j<0) j=-j;
	r=((long)j*GSHPSIGM)<<8;
	r=r/sigma->w;
	if(r<SRCHGSHPNUM) return identify_SearchShape[r];
	else return 0;
}

void identify_MultLongByte(unsigned char d, long *ps, unsigned long *res)
{
	*res=(unsigned long)(d*(*ps));
}*/

void identify_ApplyFilter() //nsigma*(sigma.WW.i+1) MUST NOT be more than 127!!! In other case change char type to short.
{
	long index;
	long  n,dn;
	long d;//,index;
//	WORD *sigma;
//	long  *ps;//, *pbs, *pbs1;
	long long  rr1, rr2, k;
//	identifyControl.ISpectrum[identifyControl.NUMCHAN-1]=0;
//	ps=&identifyControl.ISpectrum[identifyControl.NUMCHAN];
/*	for (i=identifyControl.NUMCHAN; i<SPECARRCHAN; i++){  //clear additional channels in spectrum
		*ps=0; ps++;
	}*/
//	pbs=identifyControl.BufSpec;
//	pbs1=identifyControl.SDs;
//	sigma=identifyControl.SigmaArray;
	int bufIndex = 0;
	int specIndex = 0;
	for (int i=0; i<identifyControl.NUMCHAN; i++)
	{
		index =(((long)identifyControl.nsigma_searchpeaks*(SigmaArray[bufIndex])+128)>>10);
		specIndex=i-index;
		if(specIndex<0)
		{
			identifyControl.BufSpec[bufIndex]=0;
			identifyControl.SDs[bufIndex++]=0;
//			*pbs=0;
	//		*pbs1=1;
//			pbs++;
	//		pbs1++;
		//	sigma++;
			continue;
		} //i1=0;
//		ps=&(identifyControl.ISpectrum[i1]);
		dn=(long)(GSHPSIGMx256/SigmaArray[bufIndex]);
		n=index; n*=dn; dn=-dn;
		rr1=0; rr2=0;
		(index<<=1); index++;
		while(1)
		{
			if(n>=SRCHGSHPNUM)
			{
				exception(__FILE__,__FUNCTION__,__LINE__, "n>=SRCHGSHPNUM");
			}
			d=(long)identify_SearchShape[n];
			//calculate k=(*ps)*d
			//k.LL.hi=0;
/*			if(d<0){
				//d^=0xFF; d++;
				d = -d;
				k = (*ps)*d;
				k=-k;
				rr1+=k;
				k=-k;
				k=k*d;
				//MultLongByte(d,ps,&(k.LL.lo));
				//InvertLONGLONG(&k);
				//AddLongLong(&k,&rr1);
				//InvertLONGLONG(&k);
				//MultLongLongByte(d,&k,&k);
			}
			else {*/

			if(specIndex>=SPECARRCHAN)break;
			
			k = (long long)d * identifyControl.ISpectrum[specIndex++];
			rr1+=k;
			k*=d;
			rr2+=k;
				//MultLongByte(d,ps,&(k.LL.lo));
				//calculate rr1+=k
				//AddLongLong(&k,&rr1);
				//calculate k*=d
				//MultLongLongByte(d,&k,&k);
			//}
			//calculate rr2+=k
			//AddLongLong(&k,&rr2);
			index--;
			if(!index) break;
			n+=dn; if(!n) dn=-dn;
		}
//		if(IsGZeroLONGLONG(&rr2)){ //if(rr2>0)
//		if(rr2>0)
	//	{ //if(rr2>0)
			identifyControl.BufSpec[bufIndex]=-(rr1>>8); //ISpectrum[i]=rr1/256
			identifyControl.SDs[bufIndex++]=(rr2>>16)+1; //identifyControl.SDs[i]=rr2/(256*256)
//				(*pbs)=-*(long*)(((char*)&rr1)+1); //ISpectrum[i]=rr1/256
//			(*pbs1)=*(long*)(((char*)&rr2)+2); //identifyControl.SDs[i]=rr2/(256*256)
//			identifyControl.SDs[bufIndex]++; //identifyControl.SDs[i]+=1;
//		}else
	//	{
		//	identifyControl.BufSpec[bufIndex]=0;
//			identifyControl.SDs[bufIndex]=1;
	//	}
//		pbs++; pbs1++; sigma++;
	}
	#undef nsigma
}


/*
void identify_ApplyFilterEx(long  *Variance) //The same as ApplyFilter, but spectrum variance is in separate array
{
	short i,i1;
	short  n,dn;
	char  d,index;
	FIXED2 *sigma;
	long  rrr, *ps,*pvs, *pbs, *pbs1;
	LONGLONGG  rr1, rr2, k;
	ISpectrum[identifyControl.NUMCHAN-1]=0; ps=&ISpectrum[identifyControl.NUMCHAN];
	Variance[identifyControl.NUMCHAN-1]=0; pvs=&Variance[identifyControl.NUMCHAN];
	for (i=identifyControl.NUMCHAN; i<SPECARRCHAN; i++){  //clear additional channels in spectrum
		*ps=0; ps++;
		*pvs=0; pvs++;
	}
	pbs=identifyControl.BufSpec; pbs1=identifyControl.SDs;
	sigma=identifyControl.SigmaArray;
	for (i=0; i<identifyControl.NUMCHAN; i++){
		i1=(short)(((long)identifyControl.nsigma_searchpeaks*sigma->w+128)>>10);
		index=(char)i1;
		i1=i-index; if(i1<0) {*pbs=0; *pbs1=1; pbs++; pbs1++; sigma++; continue;} //i1=0;
		ps=&(ISpectrum[i1]); pvs=&(Variance[i1]);
		dn=(short)(GSHPSIGMx256/sigma->w);
		n=index; n*=dn; dn=-dn;
		ZeroLONGLONG(&rr1); ZeroLONGLONG(&rr2);
		(index<<=1); index++;
		while(1){
			if(n>=SRCHGSHPNUM) exit(255);
			d=identify_SearchShape[n];
			//calculate k=(*ps)*d
			k.LL.hi=0;
			rrr=*ps;
			if(d<0){
				d^=0xFF; d++;
				if(rrr<0){
					rrr=-rrr;
					MultLongByte(d,&rrr,&(k.LL.lo));
				}
				else {
					MultLongByte(d,&rrr,&(k.LL.lo));
					InvertLONGLONG(&k);
				}
				AddLongLong(&k,&rr1);
			}
			else {
				if(rrr<0){
					rrr=-rrr;
					MultLongByte(d,&rrr,&(k.LL.lo));
					InvertLONGLONG(&k);
				}
				else MultLongByte(d,ps,&(k.LL.lo));
				//calculate rr1+=k
				AddLongLong(&k,&rr1);
			}
			k.LL.hi=0;
			MultLongByte(d,pvs,&(k.LL.lo));
			MultLongLongByte(d,&k,&k);
			AddLongLong(&k,&rr2);
			index--;
			if(!index) break;
			ps++; pvs++;
			n+=dn; if(!n) dn=-dn;
		}
		if(IsGZeroLONGLONG(&rr2)){ //if(rr2>0)
				(*pbs)=-*(long*)(((char*)&rr1)+1); //ISpectrum[i]=rr1/256
				(*pbs1)=*(long*)(((char*)&rr2)+2); //identifyControl.SDs[i]=rr2/(256*256)
			(*pbs1)++; //identifyControl.SDs[i]+=1;
		}
		else{
			*pbs=0; *pbs1=1;
		}
		pbs++; pbs1++; sigma++;
	}
	#undef nsigma
}
*/


void identify_SmoothingEx(long * pBuf, int sigmas)
{
	long  buf, x1=0, x2=0;
	int i1,i2, i01=-1, i02=-1;
	int sigma;
	BOOL nosum = 0;
	buf=0;
	for (int i=0;i<identifyControl.NUMCHAN;i++){
		sigma=(int)((SigmaArray[i]+128)>>sigmas);
		if(!sigma)
			sigma=1;
		i1=i-sigma; i2=i+sigma;
		if(i1<0)
		{
			*pBuf=0; pBuf++; continue;
		}// i1=0;
		if(i2>=identifyControl.NUMCHAN)
		{
			i2=identifyControl.NUMCHAN-1;
			nosum=1;
		}
		if(i01<0)
		{//������ ������������
			for(i01=i1;i01<=i2;i01++)
				buf+=identifyControl.BufSpec[i01];
		}else
		{
			if(i1>i01)
			{
				buf-=x1;
				i01++;
				if(i1>i01)
					buf-=x2;
			}
			
			if(i2>i02 && !nosum)
			{
				buf+=identifyControl.BufSpec[i2];
				i02++;
				if(i2>i02)
					buf+=identifyControl.BufSpec[i2-1];
			}
		}
		i02 = i2;
		i01 = i1;
		x2 = x1;
		x1 = identifyControl.BufSpec[i01];
		(*pBuf)=buf/(i2-i1+1);
		pBuf++;
	}
}


void identify_Smoothing()
{
	identify_SmoothingEx(identifyControl.BufSpec1,9);
	memcpy(identifyControl.BufSpec,identifyControl.BufSpec1,sizeof(long)*ARRCHAN);
}

int identify_CheckPeak(int jm, int j, int i)
// ��������� �������� ����� ���� �� ������������ ���� ������� ����������
// �������� �������������� �� �������
// jm - ����� ������ ������ ���������� ��������, j ����� ������ ���������� ���������,
// i - ����� ������ ������� ���������� ��������
{
	int  c=0,cc,w,w1,dl,dr;
	float r1;
	w=SigmaArray[j];
	cc=(int)((((long)w*identifyControl.msigma_peakcheck
#ifdef _THIN_SIGMA
		*SIGMA_THIN_FACTOR_M/SIGMA_THIN_FACTOR_D
#endif	//#ifdef _THIN_SIGMA
)/10+128)>>7);
	if(pEnergy[j]<ENERGYTHRESHOLD)
	{
		dr = cc&1;
		cc>>=1;
		cc+=dr;
	}
	if(!cc)
		cc=1;
	dl=(int)(((identifyControl.nsigma_peakcheck_left*
#ifdef _THIN_SIGMA
		SIGMA_THIN_FACTOR_M/SIGMA_THIN_FACTOR_D*
#endif	//#ifdef _THIN_SIGMA
(long)w)/10+128)>>8);
	dr=(int)(((identifyControl.nsigma_peakcheck_right*
#ifdef _THIN_SIGMA
		SIGMA_THIN_FACTOR_M/SIGMA_THIN_FACTOR_D*
#endif	//#ifdef _THIN_SIGMA
(long)w)/10+128)>>8);
	w=j-jm; w1=i-j;
	if ((abs(dr-w1)<cc))
	{ // �������� �� ���������� ��������� ��������� �� ���������
		if(abs(dl-w)<cc)
		{
			if(((pEnergy[j]>=ENERGYTHRESHOLD) &&
			(identifyControl.BufSpec[jm]<=0 || identifyControl.BufSpec[i]<=0)) ||
			(identifyControl.BufSpec[jm]<=0 && identifyControl.BufSpec[i]<=0))
				c=1;
		}
		else
		{
			jm=j-dl;
			if(jm<0)
				jm=0;
			if(identifyControl.BufSpec[jm]<=0)
				c=1;
			else
				c=0;
		}
	}
	if(c)
	{//�������������� ��������
		if(j<identifyControl.LEFTBORDER)
		{//�������� ��� ��� �� ����� ���� ��� ��� ������� ����� �������
			if(!identifyControl.ISpectrum[j])
				c=0;
			else
			{
				r1=(float)(identifyControl.ISpectrum[j]-identifyControl.ISpectrum[j+dr]);
				if(r1>0)
				{
					r1*=r1;
					r1/=(identifyControl.ISpectrum[j]+1);
					if (r1<25)
						c=0;
				}else
					c=0;
			}
		}else if(identifyControl.testNullPeak)
		{//��� ��������� ����� �������� �� ��������� ��������
			dl>>=1;
			if(dl==0)
				dl=1;
			dr>>=2;
			if(dr==0)
				dr=1;
			jm=j-dl; i=j+dr;
			if(i>=identifyControl.NUMCHAN)
				i = identifyControl.NUMCHAN-1;
			for(w=jm; w<=i; w++)
			{
				r1=(float)identifyControl.ISpectrum[w];
				if(r1<=0)
				{
					c=0;
					break;
				}
			}
		}
	}
	return c;
}

//+++++++++
void identify_AnalyzePeaks(void)
{
	int i, en;
	int bbegin=0, bcenter=0;
	int bmean1=0, bmean2=0;
	int pmean;
	long* pspec = identifyControl.BufSpec;
	long mean1 = *pspec;
	long mean2;
	long porog;
	identifyControl.BufSpec1[0] = 0;
	for(i=1;i<identifyControl.NUMCHAN;i++)
	{
		identifyControl.BufSpec1[i] = 0;
		pspec++;
		mean2 = *pspec;
		porog = mean2-mean1;
		mean1=mean2;
		if(porog<0)
			bmean1 = -1;
		else if(porog>0)
			bmean1 = 1;
		else if(i<identifyControl.NUMCHAN-1)
			continue;
		pmean = bmean1-bmean2;
		bmean2 = bmean1;
		if(!bbegin)
		{
			if(pmean>=1)//�������
				bbegin = i-1;
		}else if(!bcenter)
		{
			if(pmean==-2)//�������
				bcenter = i-1;
		}else
		{
			en = i-bcenter;
			if((pmean==2 || (pmean==0 && i==identifyControl.NUMCHAN-1)) && (en>1))//�������
			{
				if(en>2)
				{
					mean2 = identifyControl.BufSpec[bcenter];
					porog = (long)(sqrt((float)identifyControl.SDs[bcenter])*identifyControl.threshold);
					en = pEnergy[bcenter];
					if(en<=identifyControl.MAXENERGY && mean2>(en>ENERGYTHRESHOLD?porog:porog*2))
					{
						porog/=2;
						if(mean2-identifyControl.BufSpec[bbegin]>porog &&
							mean2-identifyControl.BufSpec[i-1]>porog)
						{
							if(identify_CheckPeak(bbegin, bcenter, i-1))
							{
								identifyControl.BufSpec1[bcenter] = 1;
							}
						}
					}
					bcenter=0;
					bbegin = i-1;
				}else if(pEnergy[bcenter]<ENERGYTHRESHOLD)
				{//������� ������� ��� ���� ����� ������� ������������������ ���� ��� �� ��������������
					//�� ��� ��� ��� ������� ������ ���� ����, �� ��� ������ �����, �� ����� � ����� ���������
					bcenter=0;
					bbegin = i-1;
				}
			}
		}
	}
}



#undef FIRSTWAY





int identify_CalcDeltaEnergyFromChannel(int l)
{
	int en = pEnergy[l];
	int ss = 6;
//!!!!!!!!	short s=(short)((((en>320 && en<390)?identifyControl.SCALEINSTABILITY*1.5:identifyControl.SCALEINSTABILITY)*(long)en+500)/1000);
	int s=(int)((identifyControl.SCALEINSTABILITY*en+500)/1000);
	if(s<ss)s=en<34?ss-ss/3:ss;
	return s;
}

int identify_CheckChannel(int E) //searches position of maximum near given channel (l+-identifyControl.SCALEINSTABILITY*l)
{
	int L=identify_ChannelFromEnergyNear(E);
	int di=identify_CalcDeltaEnergyFromChannel(L);
	long * pbs=&(identifyControl.BufSpec[L]);
	UINT s;
	//������� ������
	int e=E+di;
	if (e<identifyControl.MAXENERGY){
		//������� � ������� ����� �������� �������� �
		//�������� ������� ������
		s=identify_ChannelFromEnergyNear(e)-L;
	}
	else{
		//���� ������� ������ ����� ������������ ������� ��
		//��������������� ������� ������ ��� ������� �����
		e=E-di;
		//������� � ������� ����� �������� �������� �
		//�������� ������� �����
		s=L-identify_ChannelFromEnergyNear(e);
	}
	//s �� ������ ���� ������� � ������� ����� �������� �������� �
	//��������� �� ��������������
	di=(*pbs>*(pbs+1))?-1:1;
	if(di>0) s=L+s<identifyControl.NUMCHAN?s:identifyControl.NUMCHAN-L-1;
	else s=L-s>0?s:L;
	while(s>0){
		if(*pbs>*(pbs+di)) break;
		pbs+=di; L+=di; s--;
	}
	return L;
}


//����� �������������� ������� ��������� ����� �� ����������������� �������
//������ �������������� ���� �����, �.�. ��������������
//������� ���������� ��������� �����
void identify_getRightEnergy()
{
	int chan;
	int b, b1;
	int chan2;
	long lnVal,lnVal2;
	float temp,chantempd, fltVal;
	identifyControl.nLine=0;
	for(chan=0; chan<identifyControl.NUMCHAN; chan++)
	{ //calculate number of detected lines
		if (identifyControl.BufSpec1[chan]>0)
		{
			lnVal = identifyControl.BufSpec[chan]/2;
			temp=0;
			chantempd=0;
			chan2=chan;
			b1 = (SigmaArray[chan]+128)>>7;
			b = b1;
			do
			{
				lnVal2 = identifyControl.BufSpec[chan2];
				if(lnVal2<0)break;
				fltVal = (float)lnVal2;
				fltVal*=fltVal;
				temp+=fltVal;
				chantempd+=(float)fltVal*(float)chan2;
				chan2--;
			}while(lnVal2>lnVal && chan2>=0 && --b>0);
			chan2=chan+1;
			do
			{
				lnVal2 = identifyControl.BufSpec[chan2];
				if(lnVal2<0)break;
				fltVal = (float)lnVal2;
				fltVal*=fltVal;
				temp+=fltVal;
				chantempd+=(float)fltVal*(float)chan2;
				chan2++;
			}while(lnVal2>lnVal && chan2<identifyControl.NUMCHAN && --b1>0);
			chantempd = chantempd/temp;
			identifyControl.ni_channels[identifyControl.nLine]=chantempd;	//������ ������� � ���� ����
			identifyControl.deltas[identifyControl.nLine]=(short)identify_CalcDeltaEnergyFromChannel((int)(chantempd+0.5)); //�������������� ���� �����
			identifyControl.energies[identifyControl.nLine]=(short)(identify_EnergyFromChannel(chantempd)+0.5);//������� ����� � ���� ����
			identifyControl.ex_energy[identifyControl.nLine]=1;	//����� �������
			identifyControl.nLine++;
			if(identifyControl.nLine==MAXLINENUM) break; //maximum MAXLINENUM lines in spectrum
		}
	}
}




//��������� �������������
//������ ����� �����, � ����������� �� �� �������� � ������������
//����� ����� �� ��������������
//� ������������ ��������� ������ �� ������� �����
void identify_nucInit(TNucl* Nuc)
{
	long lnVal2, lnVal;
	UINT btInd, btInd2, btStored;
//	char b1=1;
	Nuc->selected=0; //clear unselect all nuclides
	Nuc->weight=0; Nuc->weightM=0;
	for(btInd=0; btInd<Nuc->num; btInd++)
	{
		Nuc->closeness[btInd]=0;
		if((Nuc->energies[btInd]>identifyControl.MAXENERGY)||(Nuc->energies[btInd]<identifyControl.MINENERGY))
		{ //remove all nuclide lines for energy greater than identifyControl.MAXENERGY or less than identifyControl.MINENERGY
			Nuc->factors_noshield[btInd]=0;
			Nuc->factors_shield[btInd]=0;
		}
		//����� ��������� ��������� ����� � ������ ������������
		lnVal2=0xFFFFFFF; Nuc->indexes[btInd]=BADINDEX;
		for(btInd2=0; btInd2<identifyControl.nLine; btInd2++)
		{
			lnVal=abs(Nuc->energies[btInd]-identifyControl.energies[btInd2]);
			lnVal=(lnVal<<8)/(long)identifyControl.deltas[btInd2];
			if(lnVal<lnVal2) {lnVal2=lnVal; btStored=btInd2;}
		}
		if (lnVal2<=256) //(lnVal<(2*256), that is difference does not exceed two sigma) mozhno vinesti v parametri
		{//����� �������� �� ������� ��������������
			//����� ���� �������������� ���� ��� ������ ��������� �����
			Nuc->closeness[btInd]=(short)lnVal2;
			//��������� � ��� ������������� ������� ������������� � ������ � ���� �� ���������� �������
			//�������������� �� ����� ���� ������������ ����� ������������ � ����� ���������
			//������������ ����� ������� ������ ������� �������������� �����������
			//����� ������� �����������
			for (btInd2=0; btInd2<btInd; btInd2++)
			{
				if(Nuc->indexes[btInd2]==btStored)
				{
					lnVal=Nuc->closeness[btInd2];
					if(lnVal<lnVal2)
						btStored=255; //there is a line, which is closer than that line
					else
						Nuc->indexes[btInd2]=BADINDEX; //that line is closer than already found line, so discard the previous line
					break;
				}
			}
			Nuc->indexes[btInd]=btStored;
		}
	}
}


int identify_MakeNuclideIdentification(void)
{
	int b1;
	int chan, chan2;
	UINT btFlag, btInd, btInd2, btStored, btNucl;
	TNucl  *Nuc;
	for(btInd=0;btInd<MAXLINENUM;btInd++)
	{
		identifyControl.ex_energy[btInd]=0;
		identifyControl.energies[btInd]=0;
		identifyControl.ni_channels[btInd]=0;
	}
	identify_getRightEnergy();
	if(!identifyControl.nLine)
	{ //if no lines
		Nuc=identifyControl.Nucls;
		for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++) {Nuc->selected=-1;	Nuc++;}
			goto beta_test;
	}else
		if(!identifyControl.NUCLNUM)
			goto beta_test;	//no nuclides in library, but there are detected lines.
	
	//Initialize
	Nuc=identifyControl.Nucls;
	for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
	{
		identify_nucInit(Nuc);
		identifyControl.Nucls[btNucl].confidence = 0;	//clear confidence
		Nuc++;
	}


	//����� � ����� �������� ��� ��� ������� ����� ����������� ������������������ ��������
	//����� � ����� �������� ��� ������� � �� ������� ����� ������ �� ������������ �� �������
	b1=1;
	while(1)
	{
		//Search and destroy nuclides, all whose lines belongs to identified nuclides
		//����� � ����� �������� ��� ��� ������� ����� ����������� ������������������ ��������
		Nuc=identifyControl.Nucls;
		for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
		{
			if(!Nuc->selected)
			{//������� ������� ��� ��������
				identify_testNoStrongLines(Nuc);
			}
			Nuc++;
		}
		if(!b1)
			break;//����� �� �����, ��� ����� �����������

		b1=0;
		//����� � ����� �������� ��� ������� ����� ������ �� ������������ �� �������
		//Search and destroy nuclides, which should have strong lines in the spectrum,
		//but whose lines are not present in the spectrum
		Nuc=identifyControl.Nucls;
		for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
		{
			if(!Nuc->selected)
			{
				//����� ������ �������� �� �����, ����� ��������� ����� ����������
				//������� ���������, ������� ��� �� ������ ����� �� �������,
				//���������� ����� �������, ������� ��������� � ������ �����������
				//��� ��� ����������
				b1 |= identify_testForLostPeak(Nuc);
			}
			Nuc++;
		}
		/*
		������� ������ ���������������� ���� �� �����
		������������ ������� ����� ��� ����������
		����� ����� ������� ��� ��������.
		��� ����� ����� �������� ������� �� ������, ������� ��������
		����� ��������� ���������� ����� ��������� ��������
		*/
		/*
		��� ���� ����� ����������:
		���� ������� ����� � ����� �� ������ ��������������
		�� ������ ���������� ���������� ������, ������� �������� ����� ������ �������� ��� �������
		���� �� ������� ��� �� ��������� � ������ �������� � �������� ���� ���� ���������� �����,
		�� ������, ������� ������� ����� ���� �� ������.
		*/
		//Identifiy nuclides, which has unique lines
		for(btInd=0; btInd<identifyControl.nLine; btInd++)
		{
			if(!identifyControl.ex_energy[btInd])continue;	//already busy
			Nuc=identifyControl.Nucls; btStored=255;
			for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
			{
				if(!Nuc->selected || Nuc->selected==-2)	//��������� ���� ��� �� ��������� ��� � �� ��������� �� ������� �� ������� ������
				{//����� ������ � ������ ������� ��������
					for(btInd2=0; btInd2<Nuc->num; btInd2++)
					{
						if(Nuc->indexes[btInd2]==btInd)
						{
							if(btStored!=255)
							{
								/*
								����� ��� ������
								��������� �� ���������
								*/
								goto l1;
							}
							btStored=btNucl; btFlag=btInd2;
							break;
						}
					}
				}
				Nuc++;
			}
			if(btStored!=255)
			{ //for that line only one nuclide is found
				Nuc=&identifyControl.Nucls[btStored];
				if(Nuc->factors_noshield[btFlag] && Nuc->factors_shield[btFlag]>1 && Nuc->selected!=-2)	//!!!!!!!��������� ������� ������� ���� �� ������ ���� ���������� �� ����� ������� �����
				{ //line is detective
					//�������� �� �� ��� � ������� ���� ��� ������� ����� ������� ������ ���� ���� �������
					btStored = 0;
					btNucl = 0;
					for(btInd2=0; btInd2<Nuc->num; btInd2++)
					{
						if(btFlag!=btInd2 && Nuc->factors_shield[btInd2])
						{
							if(Nuc->factors_shield[btInd2]>1)
								btStored++;	//have strong line, count it
							if(Nuc->indexes[btInd2]!=BADINDEX && identifyControl.ex_energy[Nuc->indexes[btInd2]])
								btNucl++;	//have this line as free, count it
						}
					}
					if(btStored==0 || btNucl>0)//!!!!!!!!!!!!!
					{//have strong lines and free lines
						//������� �����
						Nuc->selected=1; b1=1;
						//���������� ����� ������� �� �������� ������ �����
						//���������� �� ������������� �������, �� �� ���� � 2,5 ����
						//������������� �����, ���� ����������
						identify_excludeLines(Nuc);
					}
				}
			}
l1:;
		}


		/*
		�������� �� ����� �������� � ����������� �� ���������� ����� � �� ����������
		���������� ��� ������ � �������� ��� ����� 3 (�.�. ��� �������, ��� ���� ������� � ��� �� �������
		��� ������� � ������� ����� ������������ ��� ����� ������
		������ ������ ���� ��������� ������ ���� ����
		*/
		if(!b1)
		{
			//���� �� ���� �������� ������������
			for(btNucl=0;btNucl<identifyControl.NUCLNUM;btNucl++)
			{
				if(!identifyControl.Nucls[btNucl].selected)//!!!!!!!!!!!! ����������� �� ����� ���� ������� ������� �� ������ �������� �� ���������� � �������� ������� ����� ���� ��� ���� �����
				{
					btFlag=1;
					//������ ���� �������� �������
					chan = identify_checkWeight(&identifyControl.Nucls[btNucl]);
					Nuc=identifyControl.Nucls;
					btStored = 0;
					for(btInd2=0; btInd2<identifyControl.NUCLNUM;btInd2++)
					{
						if(!Nuc->selected || Nuc->selected==-2)//!!!!!!!!!!!!!! ��������� �� ����� ��������� ��� ��������
						{
							if(btInd2!=btNucl)
							{//������� ��������, �.�. �� ���� � ��� ��
								if(!identify_NuclsDifferOnly(Nuc,&identifyControl.Nucls[btNucl]))
								{//���� ���� �� ���� ������� ����� �������������
									btStored = 1;	//���� ���������
									chan2 = identify_checkWeight(Nuc);
									if(chan2>=chan || Nuc->selected==-2)
									{
										//���! ������ �������� ����� ��� � �� ����� ��������� �� ����� �������
										//��� ��� ���� ����� ������ ���������� ������
										btFlag = 0;
										break;
									}
								}
							}
						}
						Nuc++;
					}
					if(btFlag && chan>3 && btStored)
					{//������� �������� ����� ��� � �� ����� �����
						Nuc = &identifyControl.Nucls[btNucl];
//						if(!Nuc->selected)
//						{
							Nuc->selected = 1;
							b1=1;
							identify_excludeLines(Nuc);
							//����� �� �������� �����, ��� ���������� ������ ��������� ��������
							break;
//						}
					}
				}
			}
		}

		//!!!!!!!!!!!!!
		//���������� �������� �� select ���� ��� -2 (������� �� ������ ��������������� �� ������� �����)
		Nuc=identifyControl.Nucls;
		for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
		{
			if(Nuc->selected==-2)
				Nuc->selected=0;
			Nuc++;
		}


	}


	//Now check that all detected lines correspondent to identified nuclides or nuclides
	//under question (identified lines)
	for(btInd=0; btInd<identifyControl.nLine; btInd++) identifyControl.ex_energy[btInd]=0;

beta_test:
#ifdef _URANIUM_CALC
	btFlag = 0;	//���� ��� �� ���������
#endif	//#ifdef _URANIUM_CALC
	Nuc=identifyControl.Nucls;
	for(btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
	{
		b1 = Nuc->selected;
		if(b1>=0)
		{
			for(btInd=0; btInd<Nuc->num; btInd++)
			{
				btInd2=Nuc->indexes[btInd];
				if(btInd2!=BADINDEX)
				{
					identifyControl.ex_energy[btInd2]=1;
#ifdef _URANIUM_CALC

#ifndef WINVER
//��� embedded ������
					if(Nuc->Name[0]=='U' && (Nuc->NumStr[2]=='5' || Nuc->NumStr[2]=='8') && !btFlag)
#else	//#ifndef _WINDOWS
//��� windows ������
					if(Nuc->Name[2]=='U' && (Nuc->Name[6]=='5' || Nuc->Name[6]=='8') && !btFlag)
#endif	//#ifndef _WINDOWS
					{
						identify_calcUranium();
						btFlag = 1;	//���� ��� ���������
					}

#endif	//#ifdef _URANIUM_CALC
				}
#ifndef _WINDOWS
//��� embedded ������
					if(Nuc->Name[0]=='B' && Nuc->Name[1]=='a' && (Nuc->NumStr[1]=='3' && Nuc->NumStr[2]=='3'))
#else	//#ifndef _WINDOWS
//��� windows ������
					if(Nuc->Name[2]=='B' && Nuc->Name[3]=='a' && (Nuc->Name[6]=='3' && Nuc->Name[7]=='3'))
#endif	//#ifndef _WINDOWS
					{
						identify_testForPuBa(Nuc);
					}

			}
		}else if(Nuc->num==1 && Nuc->energies[0]==0)
		{//��������� ��������-90
			if((identifyControl.nLine==0 || (identifyControl.nLine==1 && (abs(identifyControl.energies[0]-1461)<28 || abs(identifyControl.energies[0]-78)<6)) ||
				(identifyControl.nLine==2 && (abs(identifyControl.energies[1]-1461)<28 && abs(identifyControl.energies[0]-78)<6)))
				&& identifyControl.collectionTime>30)
			{
				float ni_cr = 0;
				for(chan=0;chan<identifyControl.NUMCHAN;chan++)
					ni_cr+=(float)identifyControl.ISpectrum[chan];
				ni_cr/=(float)identifyControl.collectionTime;
				if(ni_cr>1000)
					Nuc->selected = 1;
			}
		}
		Nuc++;
	}


	identify_calcConfidence();

	btInd=0; for(btInd2=0; btInd2<identifyControl.nLine; btInd2++) btInd+=identifyControl.ex_energy[btInd2]; //calculate number of identified lines
	if(btInd<identifyControl.nLine) return -1;
	else if(identifyControl.nLine>0)return 1;
	else return 0;
	//On output identifyControl.energies array contains identifyControl.energies of detected lines
	//identifyControl.ex_energy contain 0, if line does not have correspondent nuclide
	//and 1, if line corresponds to detected nuclide or nuclide under question
}

#ifdef _URANIUM_CALC
void identify_calcUranium(void)
{
	int en186=identify_CheckChannel(186);
	int en1001=identify_CheckChannel(1001);
	float fmean = (float)identifyControl.BufSpec[en1001];
	if(fmean>0)
	{
		fmean = (float)identifyControl.BufSpec[en186]/fmean;
		fmean = (float)(100.0*(0.027*pow((float)fmean, (float)0.52)-0.038));
		if(fmean>90)
			fmean = 90;
		else if(fmean<1)
			fmean = 0;
	}else
		fmean = 90;
	uranium = (short)(fmean);
}
#endif	//#ifdef _URANIUM_CALC







/*
���������� ����� � ��������� �� ��������� �����
����������� ������������ ������� �����, ����� �������������������� ����� �
������ �����, ��� ������ ���� ������������� ����� � 2,5 ����
*/
void identify_excludeLines(TNucl* Nuc)
{
	UINT btInd, btInd2;
	int chan;
	long lnVal;
	UINT btFlag=255;
	float temp,temp1, tempd, tempd1;
	for(btInd=0; btInd<Nuc->num; btInd++)
	{
		btInd2=Nuc->indexes[btInd];
		if(btInd2!=BADINDEX && identifyControl.ex_energy[btInd2])
		{
			lnVal = Nuc->factors_noshield[btInd];
			if(lnVal && Nuc->factors_shield[btInd])
			{
				chan=identify_CheckChannel(Nuc->energies[btInd]); //find closest maximum
//				flt = sqrt((float)identifyControl.SDs[chan]);
				temp1 = 256.0f*(float)identifyControl.BufSpec[chan];
				temp = 256.0f*(float)lnVal;
				identifyControl.ex_energy[btInd2]=0;
				tempd1 = temp1;
				tempd = temp;					
				btFlag=btInd;
				break;
			}
		}
	}
	if(btFlag!=255)
	{
		for(btInd=0; btInd<Nuc->num; btInd++)
		{
			if(btInd!=btFlag)
			{
				btInd2=Nuc->indexes[btInd];
				if(btInd2!=BADINDEX && identifyControl.ex_energy[btInd2])
				{
					//���������� ���� ����� (������� � �������) ����� �� �������, ������������� ������� �� ������� ���������� ������
					//����� ���������� �� ������ ���� �����
					//���� ��� �������, �� ���������
					//���� ��� ������� ��� �� �������, �� ������� �� �� ����� �� ������ �� ���� ������� ��� �� ����������
					lnVal = Nuc->factors_noshield[btInd];
					if(lnVal)
					{
						chan=identify_CheckChannel(Nuc->energies[btInd]); //find closest maximum
//						//flt = sqrt((float)identifyControl.SDs[chan]);
						temp1 = 256.0f*(float)identifyControl.BufSpec[chan];
						temp = 256.0f*(float)lnVal;

						//����� ��������� �� ������
						temp = temp/tempd;
						temp1 = temp1/tempd1;
						//temp - ��������� ����� ������������ ���� � ������ ������������� �� ������������ �������, �.�. ������
						//temp1 - ��������� ����� ������������ ���� � ������ ������������� �� ���������� �������
						if(temp1/temp<2.5)
						{//��� �� ����������, �.�. �������
							identifyControl.ex_energy[btInd2]=0;
						}else
						{//��� ���� ���������� � 2,5 ����
							//��� �� ���������, �.�. �� ����� ����������� �����
							//identifyControl.ex_energy[btInd2]=identifyControl.ex_energy[btInd2];
							;
						}
					}else
					{
						identifyControl.ex_energy[btInd2]=0;
					}
				}
			}
		}
	}
}



int identify_checkWeight(TNucl* Nuc)
{
	UINT btInd, btInd2;
	int cnt=0;
	for(btInd=0; btInd<Nuc->num; btInd++)
	{
		btInd2=Nuc->indexes[btInd];
		if(btInd2!=BADINDEX && identifyControl.ex_energy[btInd2])
		{
			cnt++;
//!!!!!!			if(Nuc->factors_noshield[btInd])
//!!!!!!				cnt++;
			if(Nuc->factors_shield[btInd])
				cnt++;
		}
	}
	return cnt;
}


/*
����� ������ ����������� �����, ������� ������� ��������� ��� �����
*/
BOOL identify_findOtherStrong(int en)
{
	UINT i;
	int chan, chanPrim, si;

	chanPrim=identify_ChannelFromEnergyNear(en); //find closest maximum
	for(i=0;i<identifyControl.nLine;i++)
	{
		chan = (int)(identifyControl.ni_channels[i]+0.5);
		si = (int)((SigmaArray[chan]*2+128)>>8);
		if(chanPrim>(chan-si) && chanPrim<(chan+si))
		{//���� ����� ���
			return TRUE;	//������ ��������� �� �����
		}
	}
	return FALSE;
}


/*
����������� ������������ �������� �� ������� ������
0-������� ����� ���� ������� ��������������� �����
1-������� ��������
*/
UINT identify_NuclsDifferOnly(TNucl *Nuc, TNucl *NucMain)
{
	UINT i,j,btFlag=1,n;
	UCHAR  *ind1, *ind2;
	ind1=NucMain->indexes;
	for(i=0; i<NucMain->num; i++)
	{
		n = *ind1;
		if(n!=BADINDEX)
		{
			//����� ���������� ������������ ��� ������������ ������� ������ ������� �����
			if(identifyControl.ex_energy[n] 
//&& //����� ��� �� ������
//				NucMain->factors_shield[i]
)
			{
				ind2=Nuc->indexes; btFlag=1;
				for(j=0; j<Nuc->num; j++)
				{
					if(*ind2==n 
//&& Nuc->factors_shield[j]
)
					{
						btFlag=0;
						break;
					}
					ind2++;
				}
			}
		}
		ind1++;
		if(!btFlag) break;	//���� ��������������� �����
	}
	//0, ���� ���� �� ���� ������� ����� �������� �������������
	//1, ���� ��� ���������� ����� �� �������������
	return btFlag;
}


/*
�������� �� ������� ����� ����� �� ������� ���� ������� ���� � ����������
���� �� ������� ������ ��� ��� ������ ������� ����
� � ������� ��� ������ ������� �����, �� ������ �����������
*/
int identify_testForLostPeak(TNucl* Nuc)
{
	long threshold = identifyControl.threshold<<8;
	//�������� ���� �������� ��� �������� �� ��������� ����� �����
	UINT num, k, j=255, n=0, n1=0, n2=0, ind;
	int btFlag=0, b1=0;
	int chan;
	UINT sh, nsh;
	float temp=0, temp1=0, tempd=0, tempd1=0;
	num = Nuc->num;
	for(k=0; k<num; k++)
	{ //high energy lines go first
		ind=Nuc->indexes[k];
		nsh = Nuc->factors_noshield[k];
		if(ind!=BADINDEX)
		{
			chan=(int)(identifyControl.ni_channels[ind]+0.5);// lnVal2=0x7FFFFFFF; lnVal=0x7FFFFFFF;
			if(j==255 && nsh)
				j=k; //����������� ������� ����� ����� ������������������ �� �������
			sh = Nuc->factors_shield[k];
			if(nsh && sh)
			{
				temp+=(float)identifyControl.BufSpec[chan]*(float)nsh/(float)identifyControl.SDs[chan];
				tempd+=(float)nsh*(float)nsh/(float)identifyControl.SDs[chan];
				temp1+=(float)identifyControl.BufSpec[chan]*(sh==1 ? (float)0 : (float)sh)/(float)identifyControl.SDs[chan];
				tempd1+=(float)(sh==1 ? (float)0 : (float)sh*(float)sh)/(float)identifyControl.SDs[chan];
				n++;
				if(sh>0) n1++;
				if(sh>1) n2++;
			}
		}
		else
		{
			if(nsh) btFlag=1;
		}
	}
	if(btFlag)
	{//���� ���� �� ���� ��������� ����� � ����������, ������� �� ����������
		if(!n)
		{//�� ���� ����� �� ����������
			//������ �����������
			Nuc->selected=-1; b1=1;
		}else
		{
			if(n2)//���������� ���� �� ���� ������� �����, �� �� �������
				temp1/=tempd1;
			//���������� ���� �� ���� ��������� �����
			temp/=tempd;
			for(k=0; k<num; k++)
			{
				ind=Nuc->indexes[k];
				nsh = Nuc->factors_noshield[k];
				sh = Nuc->factors_shield[k];
//before 24/08/2010				if((ind==BADINDEX)&&(nsh)&&(sh>1))
				//after 24/08/2010 it helps to exclude RPu on Cs
				if((nsh)&&(sh>1))
				{//���� ����� ��������� � ���������
					if(k>j)
					{//if checked line has higher energy, then process it as if there is no shielding, else as if there is shielding
						//��� �� ����� ������ �������������� �����
						if(!n1)
						{//�� ���� ������� ��� ������� ����� �� ����������
							//��������� ������
							Nuc->selected=-1; b1=1; break;
						}
						tempd=temp1*(float)sh;
					}else
					{//����� �������������������� �����
						tempd=temp*(float)nsh;
					}
					sh = Nuc->energies[k];
					chan=identify_CheckChannel(sh); //find closest maximum
					tempd1=sqrt((float)identifyControl.SDs[chan]);
					tempd=256.0f*tempd/tempd1; //calculate amplitude of line in units of standard deviation
					//!!!!!!!!!!!� ���� ��������� ������� ���� ������, ���� ������ �� �������� �����
					//���� ��� ����������� ������� �����, �� ���� ������ ����� �� ����� ����
					//��������� ����� �� ����� ����� �� �������� �����
					//!!!!!!!!!!!���� �� ����� �� ����� ������� � ���� � ���� �� ���� �� ������
					//��� ������� �������, �� ���� ������ �� ������ ���� ��������� �� ����� �����
					if(!Nuc->selected && sh>=90)	//������� ���������� ����� ��� �������� ����� ���� 90 ���
						Nuc->selected = -2;	//���� ������ �� ����� ���� ������, �.�. ��� �� ������ ������� ����������
					if(tempd>threshold/*+768.0f*/)//������ ���� ���� ��� ����� �� ���������� ����� ��� ����� ��������� ����� � ������� ����������
					{ //768 = 3*sigma . If calculated amplitude is higher then threshold by three sigma
						//!!!!!!!!!!���� ������: ����� �� ����� ������ ������� ������� ��� ��������������, �� ���� ��� ������ ����� �� ��� �������� �������������?
						if(Nuc->selected==-2)
							Nuc->selected = -3;	//� ���� ������ ��� ����� ���� ������ �.�. ������ ������� ����������
						//��� �� ������������ ������ �� ������� ������� ��������� �������� �����
						tempd1=256.0f*(float)identifyControl.BufSpec[chan]/tempd1;
						if(tempd1/*+768.0f*/<threshold/2)	//!!!!!!! ������ 768 ��-�� ���� ��� ����� ���������� ����� ������������
						{//check it for missed peak
							//��� ������� �� ������ �����
							//�.�. ��� ����� ������ ������ ����, ��� ������ ������
							//���������� �� ������� �� ��� ����� ���� ��������
							//���� ������� �� ������ �� �����������
//before 24/08/2010				if(!findOtherStrong(sh))
//after 24/08/2010 it helps to exclude Rpu on Cs
							if((ind!=BADINDEX) || !identify_findOtherStrong(sh))
							{
								//������ �����������
								Nuc->selected=-1; b1=1;
								break;
							}
						}
						else
						{//��� ������� ������ �����
//before 24/08/2010		
//							if((tempd>(float)(threshold<<2))&&(tempd-tempd1>(10.0f*256)))
//after 24/08/2010 it helps to exclude Rpu on Cs
							if((k!=j) /*not for the max energy lines*/
								&& (tempd>(float)(threshold<<2))&&(tempd-tempd1>(10.0f*256)))
							{ //very large peak here but we see very small
								//��� ������������ �����, �� ����� ������ ��� �� �������
								//���������� �� ������� �� ��� ����� ���� ��������
								//���� ������� �� ������ �� �����������

//before 24/08/2010				if(!findOtherStrong(sh))
//after 24/08/2010 it helps to exclude Rpu on Cs
								if((ind!=BADINDEX) || !identify_findOtherStrong(sh))
								{
									Nuc->selected=-1; b1=1;
									break;
								}
							}
						}
					}
				}
			}
			if(Nuc->selected==-3)
				Nuc->selected=0;
		}
	}
	return b1;
}

/*
���� �� ���������� � ������� ������� �����
���� ������� ���, �� ������ �����������
*/
void identify_testNoStrongLines(TNucl* Nuc)
{
	UINT k, num, j;
	num = Nuc->num;
	for(k=0; k<num; k++)
	{ //high energy lines go first
		j=Nuc->indexes[k];
		if((j!=BADINDEX) && ((Nuc->factors_shield[k]>0) && (Nuc->factors_noshield[k]>0)))
		{//������������ ������� ������� �����
			//����������� ������� �� �� ������� ����� �� �����
			if(identifyControl.ex_energy[j])//���� ���� ����� �� �����������, �� ���� ������ ����� ��������������� ������
			{
				return;
			} //if one of important nuclide's line is not excluded, then break
		}
	}
	//��� ������� ����� ��� ���������, ������ ��������� ������
	Nuc->selected=-1; //all lines of that nuclide belongs to other nuclide
}




float identify_EnergyFromChannel(float chan)
{
	float en1, en2;
	UINT ch = (UINT)chan;
	en1 = pEnergy[ch];
	en2 = pEnergy[ch+1];
	chan -= ch;
	return en1+(en2-en1)*chan;
}




void identify_calcConfidence(void)
{
	long threshold = identifyControl.threshold<<8;
	TNucl  *Nuc;
	char b1;
	short chan, chan2;
	BYTE btInd2, btStored;
	uint32_t lval;
	Nuc=identifyControl.Nucls;
	for(int btNucl=0; btNucl<identifyControl.NUCLNUM; btNucl++)
	{
		Nuc->confidence = 0;
		b1 = Nuc->selected;
		if(b1>0)
		{
			//�����������: � ���������� 0-255 = �������� 0-1
			//start check for amount of strongline and calc max line probability
			//start check closeness of each line and calc its probability
			chan=0;
			btStored = 0;
			for(int btInd=0; btInd<Nuc->num; btInd++)
			{
				if((Nuc->factors_noshield[btInd] && Nuc->factors_shield[btInd]))
				{
					btInd2=Nuc->indexes[btInd];
					if(btInd2!=BADINDEX && identifyControl.ex_energy[btInd2])
					{//������ ��� ��������� �����
						//��� ����� = (1-��������� �����)*(�������/(�����*���))
						chan2 = (USHORT)identifyControl.ni_channels[btInd2];
						chan2 = (ULONG)(identifyControl.BufSpec[chan2]/(sqrt((float)identifyControl.SDs[chan2])*(ULONG)threshold/256));
						if(chan2>5)chan2=5;
						chan2 = chan2*51;
						lval = (USHORT)(((ULONG)((((USHORT)256-abs(Nuc->closeness[btInd]))*(USHORT)255)>>8)*
							(ULONG)chan2)>>8);
						if(lval>255)lval = 255;
						chan += lval;
						btStored++;
					}
				}
			}
//			chan/=btStored;
			if(chan>255)chan=255;
			Nuc->confidence = chan/30+1;	//�������� �� 0 �� 9
		}
		Nuc++;
	}
}





//special test for mixture Pu+Ba
void identify_testForPuBa(TNucl* thisnuc)
{//if calls then we have already Ba

	short j,i, en, to, foundKev = 0;
	short bbegin=0, bcenter=0;
	signed char bmean1=0, bmean2=0;
	signed char pmean;
	long* pspec;
	long mean1;
	long mean2;
	long porog;
	TNucl* Nuc;

	i=identify_ChannelFromEnergyNear(400);//start finding peak from 400 kev
	to = identify_ChannelFromEnergyNear(500);//stop finding peak at 500 kev
	pspec = &identifyControl.BufSpec[i];
	mean1 = *pspec;
	for(;i<to;i++)
	{
		pspec++;
		mean2 = *pspec;
		porog = mean2-mean1;
		mean1=mean2;
		if(porog<0)
			bmean1 = -1;
		else if(porog>0)
			bmean1 = 1;
		else if(i<identifyControl.NUMCHAN-1)
			continue;
		pmean = bmean1-bmean2;
		bmean2 = bmean1;
		if(!bbegin)
		{
			if(pmean>=1)//�������
				bbegin = i-1;
		}else if(!bcenter)
		{
			if(pmean==-2)//�������
				bcenter = i;
		}else 
		{
			en = i-bcenter;
			if((pmean==2 || (pmean==0 && i==identifyControl.NUMCHAN-1)) && (en>1))//�������
			{
				if(en>2)
				{
					mean2 = identifyControl.BufSpec[bcenter];
					j=1;
					for(;j<=10;j++)
					{
						if(identifyControl.BufSpec[bcenter+j]<=mean2)
							mean2 = identifyControl.BufSpec[bcenter+j];
						else
							break;
					}
					if(j>=6)
					{
						foundKev = (short)identify_EnergyFromChannel(bcenter);
						break;
					}
				}
			}
		}
	}
	if(foundKev>400 && foundKev<435)
	{//we have Pu
		Nuc=identifyControl.Nucls;
		for(i=0; i<identifyControl.NUCLNUM; i++)
		{
#ifndef _WINDOWS
//��� embedded ������
			if(Nuc->Name[0]=='R' && Nuc->Name[1]=='P' && Nuc->Name[2]=='u')
#else	//#ifndef _WINDOWS
//��� windows ������
			if(Nuc->Name[2]=='R' && Nuc->Name[3]=='P' && Nuc->Name[4]=='u')
#endif	//#ifndef _WINDOWS
			{
				Nuc->selected = thisnuc->selected;//select RPu as Ba
				identifyControl.Nucls[i].confidence = 6;
				break;
			}
			Nuc++;
		}
	}//nothing
}








///////////////////////////////////////////////////////////////////////////////////////////////




const unsigned char identify_GaussShape[GSHPNUM]={
255,255,255,255,255,255,255,255,255,255,
254,254,254,254,254,254,254,253,253,253,
253,253,252,252,252,251,251,251,251,250,
250,250,249,249,249,248,248,247,247,247,
246,246,245,245,244,244,243,243,242,242,
241,241,240,240,239,238,238,237,237,236,
235,235,234,233,233,232,231,231,230,229,
229,228,227,227,226,225,224,224,223,222,
221,220,220,219,218,217,216,216,215,214,
213,212,211,210,210,209,208,207,206,205,
204,203,202,201,201,200,199,198,197,196,
195,194,193,192,191,190,189,188,187,186,
185,184,183,182,181,180,179,178,177,176,
175,174,173,172,171,170,169,168,167,166,
165,164,163,162,161,160,159,158,157,156,
155,154,153,152,151,150,148,147,146,145,
144,143,142,141,140,139,138,137,136,135,
134,133,132,131,130,129,128,127,126,125,
124,123,122,121,120,119,118,117,116,115,
114,113,112,111,110,110,109,108,107,106,
105,104,103,102,101,100,99,98,97,97,
96,95,94,93,92,91,90,90,89,88,
87,86,85,84,84,83,82,81,80,80,
79,78,77,76,76,75,74,73,72,72,
71,70,69,69,68,67,66,66,65,64,
64,63,62,61,61,60,59,59,58,57,
57,56,55,55,54,54,53,52,52,51,
50,50,49,49,48,47,47,46,46,45,
45,44,44,43,42,42,41,41,40,40,
39,39,38,38,37,37,36,36,35,35,
35,34,34,33,33,32,32,31,31,31,
30,30,29,29,29,28,28,27,27,27,
26,26,25,25,25,24,24,24,23,23,
23,22,22,22,21,21,21,20,20,20,
20,19,19,19,18,18,18,18,17,17,
17,17,16,16,16,15,15,15,15,15,
14,14,14,14,13,13,13,13,13,12,
12,12,12,12,11,11,11,11,11,10,
10,10,10,10,10,9,9,9,9,9,
9,9,8,8,8,8,8,8,8,7,
7,7,7,7,7,7,7,6,6,6,
6,6,6,6,6,6,5,5,5,5,
5,5,5,5,5,5,5,4,4,4,
4,4,4,4,4,4,4,4,4,4,
3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,2,2,2,
2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,
2,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1};

const int8_t identify_SearchShape[SRCHGSHPNUM]={
-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,
-126,-126,-126,-126,-125,-125,-125,-125,-124,-124,
-124,-123,-123,-123,-122,-122,-121,-121,-120,-120,
-120,-119,-118,-118,-117,-117,-116,-116,-115,-114,
-114,-113,-113,-112,-111,-110,-110,-109,-108,-108,
-107,-106,-105,-104,-104,-103,-102,-101,-100,-99,
-98,-98,-97,-96,-95,-94,-93,-92,-91,-90,
-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,
-79,-78,-77,-76,-75,-73,-72,-71,-70,-69,
-68,-67,-66,-65,-63,-62,-61,-60,-59,-58,
-56,-55,-54,-53,-52,-51,-50,-48,-47,-46,
-45,-44,-43,-41,-40,-39,-38,-37,-36,-34,
-33,-32,-31,-30,-29,-27,-26,-25,-24,-23,
-22,-21,-19,-18,-17,-16,-15,-14,-13,-12,
-11,-10,-8,-7,-6,-5,-4,-3,-2,-1,
0,1,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,15,16,17,18,
19,20,21,22,22,23,24,25,26,26,
27,28,29,29,30,31,32,32,33,34,
34,35,36,36,37,38,38,39,39,40,
41,41,42,42,43,43,44,44,45,45,
46,46,47,47,48,48,48,49,49,50,
50,50,51,51,51,52,52,52,52,53,
53,53,53,54,54,54,54,55,55,55,
55,55,55,56,56,56,56,56,56,56,
56,56,56,56,57,57,57,57,57,57,
57,57,57,57,57,57,57,56,56,56,
56,56,56,56,56,56,56,56,56,55,
55,55,55,55,55,55,54,54,54,54,
54,53,53,53,53,53,52,52,52,52,
52,51,51,51,51,50,50,50,50,49,
49,49,49,48,48,48,47,47,47,47,
46,46,46,45,45,45,45,44,44,44,
43,43,43,42,42,42,42,41,41,41,
40,40,40,39,39,39,38,38,38,37,
37,37,36,36,36,36,35,35,35,34,
34,34,33,33,33,32,32,32,31,31,
31,31,30,30,30,29,29,29,28,28,
28,28,27,27,27,26,26,26,25,25,
25,25,24,24,24,24,23,23,23,22,
22,22,22,21,21,21,21,20,20,20,
20,19,19,19,19,18,18,18,18,17,
17,17,17,17,16,16,16,16,15,15,
15,15,15,14,14,14,14,14,13,13,
13,13,13,13,12,12,12,12,12,11,
11,11,11,11,11,10,10,10,10,10,
10,10,9,9,9,9,9,9,9,8,
8,8,8,8,8,8,7,7,7,7,
7,7,7,7,7,6,6,6,6,6,
6,6,6,6,6,5,5,5,5,5,
5,5,5,5,5,5,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,
3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,2,2,2,
2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,
2,2,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};


//default identify.ini
const char identify_ini[247]=
";identify.ini\r"
"[identifyControl]\r"
"MINENERGY=20\r"
"MAXENERGY=3000\r"
"nsigma_searchpeaks=16\r"
"SCALEINSTABILITY=20\r"
"testNullPeak=1\r"
"nsigma_peakcheck_left=26\r"
"nsigma_peakcheck_right=24\r"
"msigma_peakcheck=20\r"
"threshold=3\r"
"identifyStartDeadTime=60\r"
"libraryFileName=main\r";





/*
//default main.lib
const char main_lib
#ifdef _SNM
[6061]=
#include "librarySNM.lib"
#else
[4667]=
#include "library.lib"
#endif	//_SNM
;
*/
