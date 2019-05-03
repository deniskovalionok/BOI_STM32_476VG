#ifndef _EXEPTIONS_H
#define _EXEPTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif  //#ifdef __cplusplus

void exception(const char* pModuleName,const char* pFuncName, int LineNum, const char* pDesc);

void exeption_proc(const char* pDesc);

void interrupts_get_Registers(void);

#ifdef __cplusplus
}
#endif  //#ifdef __cplusplus
#endif	//#ifndef _EXEPTIONS_H