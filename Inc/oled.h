#ifndef __OLED_H_
#define __OLED_H_

void writeOledData(uint16_t data, uint8_t register_select);
uint16_t readOledData(uint8_t register_select);
void writeOledRegister(uint8_t reg, uint16_t data);
void writeOledNop(void);
uint16_t readOledStatus(void);
uint16_t readOledRegister(uint8_t reg);

void oledInit(void);
void oledControl(void);

#endif //__OLED_H_