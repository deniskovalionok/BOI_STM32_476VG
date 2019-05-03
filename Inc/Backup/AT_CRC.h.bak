#ifndef _AT_CRC_H
#define _AT_CRC_H

#ifdef __cplusplus

#include <stdint.h>

namespace CRC_USR
{
#ifndef LOBYTE
#define LOBYTE(w)           ((uint8_t)(((ULONG_PTR)(w)) & 0xff))
#endif  //#ifndef LOBYTE

#ifndef LO2BYTE
#define LO2BYTE(w)           ((uint8_t)(((ULONG_PTR)(w) >> 8) & 0xff))
#endif  //#ifndef LO2BYTE
  
#ifndef ULONG_PTR
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#endif  //#ifndef ULONG_PTR

//
// Add Windows flavor DWORD_PTR types
//

#ifdef DWORD_PTR
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
#endif  //#ifdef DWORD_PTR
  
  enum tagCRC_type{
    AT_CRC,
    MB_CRC
  };

  class cCRC
  {
  public:
    uint16_t calcByte(uint8_t Sbyte, uint16_t CRCbuf);
    uint16_t calc(uint8_t volatile * pData, uint32_t len, tagCRC_type type);
    bool check(const uint8_t * pData, uint32_t len, tagCRC_type type);
  protected:
  };
  
  extern cCRC CRC_global;
};

extern "C"
{
#endif  //#ifdef __cplusplus
  
  uint16_t CRC_calcByte (uint8_t Sbyte, uint16_t CRCbuf);

  uint16_t CRC_calc(uint8_t volatile * pData, uint32_t len);
  
  uint32_t CRC_check(const uint8_t * pData, uint32_t len);
  
#ifdef __cplusplus
}
#endif  //#ifdef __cplusplus
#endif	//#ifndef _AT_CRC_H