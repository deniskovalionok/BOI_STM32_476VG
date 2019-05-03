#ifndef _TYPES_H
#define _TYPES_H

//user types
//31/10/2008 Bystrov, Nikolaev 03-10




#ifndef VOID
#define VOID void
//typedef signed char CHAR;
typedef short SHORT;
typedef long LONG;
#endif




#ifndef BASETYPES
#define BASETYPES
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
#endif  /* !BASETYPES */

#define MAX_PATH          260

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif



#undef FAR
#undef  NEAR
#define FAR
#define NEAR
#ifndef CONST
#define CONST               const
#endif

/*
struct OneBit {
    unsigned char value:1;
};
*/

typedef unsigned long       DWORD;
typedef unsigned long       BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL            *PBOOL;
typedef BOOL             *LPBOOL;
typedef BYTE            *PBYTE;
typedef BYTE             *LPBYTE;
typedef int             *PINT;
typedef int              *LPINT;
typedef WORD            *PWORD;
typedef WORD             *LPWORD;
typedef long             *LPLONG;
typedef DWORD           *PDWORD;
typedef DWORD            *LPDWORD;
typedef void             *LPVOID;
typedef CONST void       *LPCVOID;


typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;




#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;

//
// Add Windows flavor DWORD_PTR types
//

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;


#define MAKEFLOAT(val, a, b, c, d)      *((char*)&val)=a;*((char*)&val+1)=b;*((char*)&val+2)=c;*((char*)&val+3)=d;
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKESHORT(a, b)      ((SHORT)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((SHORT)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define MAKEINT(val,a,b,c,d)     val=(((unsigned int) a)&0xff)|(((unsigned int)b<<8)&0xff00)|(((unsigned int)c<<16)&0xff0000)|(((unsigned int)d<<24)&0xff000000)// | ((unsigned int)b << 8)| ((unsigned int)c << 16)| ((unsigned int) d << 24));
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 24))
#define HIBYTE_W(w)           ((WORD)((DWORD_PTR)(w) >> 8))
//#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))

#ifndef LOBYTE
#define LOBYTE(w)           ((uint8_t)(((ULONG_PTR)(w)) & 0xff))
#endif  //#ifndef LOBYTE

//#define LO2BYTE(w)           ((BYTE)(((DWORD_PTR)(w) >> 8) & 0xff))

#ifndef LO2BYTE
#define LO2BYTE(w)           ((uint8_t)(((ULONG_PTR)(w) >> 8) & 0xff))
#endif  //#ifndef LO2BYTE

#define LO3BYTE(w)           ((BYTE)(((DWORD_PTR)(w) >> 16) & 0xff))

# define PI           (double)3.14159265358979323846  /* pi */ 


//receiver buffer len in bytes
#define RCV_BUF_LEN 300



//these values must be from 0-4
enum RCV_ERRORS
{
	RCV_OK,
	RCV_ERR_TIMEOUT,
	RCV_ERR_BYTE,
	RCV_ERR_BUF_OVERFLOW,
	RCV_ERR_CRC,
};

#pragma pack(1)
struct tagCommonGPS
{
	double Lat;
	double Lon;
	float Alt;
	float Spd;
	float Dir;
	unsigned char Vld;
	unsigned char Sat;
};
#pragma pack()


typedef DWORD HFILE;


typedef long HRESULT;


#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)


#ifndef SUCCEEDED
#define SUCCEEDED(hr)  ((HRESULT)(hr) >= 0)
#endif

#ifndef FAILED
#define FAILED(hr)  ((HRESULT)(hr) < 0)
#endif

#ifndef S_OK
#define S_OK  ((HRESULT)0x00000000L)
#endif



//
// Codes 0x4000-0x40ff are reserved for OLE
//
//
// Error codes
//
//
// MessageId: E_UNEXPECTED
//
// MessageText:
//
//  Catastrophic failure
//
#define E_UNEXPECTED                     _HRESULT_TYPEDEF_(0x8000FFFFL)

//
// MessageId: E_NOTIMPL
//
// MessageText:
//
//  Not implemented
//
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)

//
// MessageId: E_OUTOFMEMORY
//
// MessageText:
//
//  Ran out of memory
//
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)

//
// MessageId: E_INVALIDARG
//
// MessageText:
//
//  One or more arguments are invalid
//
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)

//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
//  No such interface supported
//
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)

//
// MessageId: E_POINTER
//
// MessageText:
//
//  Invalid pointer
//
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)

//
// MessageId: E_HANDLE
//
// MessageText:
//
//  Invalid handle
//
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)

//
// MessageId: E_ABORT
//
// MessageText:
//
//  Operation aborted
//
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)

//
// MessageId: E_FAIL
//
// MessageText:
//
//  Unspecified error
//
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)

//
// MessageId: E_ACCESSDENIED
//
// MessageText:
//
//  General access denied error
//
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)



//макс кол элементов в табл
#define MAX_TABLE_RECORDS 30


#endif	//#ifndef _TYPES_H