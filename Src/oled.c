#include "SPI_oled.h"
#include "oled.h"
#include "ft8xx.h"

#define OLED_ID             0   //Задается сигналом на входе ID_MIB дисплея
#define OLED_CHIP_ADDRESS   ((0x0E<<1) | OLED_ID)
uint8_t counter;

/**
 * @brief Отправка данных на контроллер дисплея
 * @details [long description]
 * 
 * @param data Данные, которые необходимо отправить
 * @param register_select Выбор регистра, куда отправлять данные:
 * 		0 - Index register
 * 		1 - instruction parameter, GRAM data
 */
void writeOledData(uint16_t data, uint8_t register_select)
{
    uint8_t start_byte;
    uint8_t rw_bit = 0;
    uint8_t rs_bit = register_select;
    
    if (rs_bit <= 1)
        start_byte = (OLED_CHIP_ADDRESS<<2) | (rs_bit<<1) | rw_bit;
        
    //Стартовый байт, в нем передается RW, RS и ID
    OLED_spi_select();
    sendSpiData(SPI1, start_byte);
    sendSpiData(SPI1, (uint8_t) (data>>8));
    sendSpiData(SPI1, (uint8_t) data);
    OLED_spi_deselect();
}

/**
 * @brief Прием данных от контроллера дисплея
 * @details [long description]
 * 
 * @param data Данные, которые необходимо отправить
 * @param register_select Выбор регистра, куда отправлять данные:
 * 		0 - Index register
 * 		1 - instruction parameter, GRAM data
 */
uint16_t readOledData(uint8_t register_select)
{
    uint16_t result = 0;
    uint8_t start_byte;
    uint8_t rw_bit = 1;
    uint8_t rs_bit = register_select;
    
    if (rs_bit <= 1)
        start_byte = (OLED_CHIP_ADDRESS<<2) | (rs_bit<<1) | rw_bit;
        
    //Стартовый байт, в нем передается RW, RS и ID
    OLED_spi_select();
    sendSpiData(SPI1, start_byte);
//    sendSpiData(SPI1, 0xFF);
    result |= sendSpiData(SPI1, 0xFF) << 8;
    result |= sendSpiData(SPI1, 0xFF);
    OLED_spi_deselect();
    
    return result;
}


/**
 * @brief Функция записи данных в регистр дисплея
 * @details [long description]
 * 
 * @param reg Регистр дисплея
 * @param data Данные, которые необходимо записать в регистр
 */
void writeOledRegister(uint8_t reg, uint16_t data)
{
    switch(reg){
      case 0x23:
      case 0x24:
        writeOledData((uint16_t) reg, 0);
        break;
      default:
        writeOledData((uint16_t) reg, 0);
        writeOledData(data, 1);
    }

}

/**
 * @brief Функция оправляющая пустую команду на дисплей
 */
void writeOledNop(void)
{
    writeOledData(0, 0);
}

/**
 * @brief Чтение регистра статуса контроллера дисплея
 * @details [long description]
 * 
 * @return Значение регистра
 */
uint16_t readOledStatus(void)
{
    return readOledData(0);
}

/**
 * @brief Чтение регистра контроллера дисплея
 * @details [long description]
 * 
 * @param reg Адрес регистра
 * @return Значение регистра
 */
uint16_t readOledRegister(uint8_t reg)
{
    writeOledData((uint16_t) reg, 0);
    return readOledData(1);
}

uint16_t oledInited = 0;

void oledInit(void)
{
  OLED_spi_deselect();

    /************************
    * Запуск  и чтение id   
    ************************/
    HOST_CMD_ACTIVE();

    //Чтение Dev ID
    static uint8_t dev_id;
    while(dev_id != 0x7C){               // Device ID should always be 0x7C
        dev_id = HOST_MEM_RD8(REG_ID);      // Read device id
    }

    //Выключим DOTCLK от FTDI
    HOST_MEM_WR8(REG_PCLK, 0x00);      		// Pixel Clock Output disable  

    //Сбросим дисплей через вход RESET
    HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
    HOST_MEM_WR8(REG_GPIO, 0x80);                     // Enable Disp (if used)    
    HAL_Delay(500);
    HOST_MEM_WR8(REG_GPIO, 0x00);                     // Disable Disp (if used)    
    HAL_Delay(1);
    HOST_MEM_WR8(REG_GPIO, 0x80);                     // Enable Disp (if used)    
    HAL_Delay(10);

    //Выведем дисплей из режима Standby
    writeOledRegister(0x10, 0);
    
    HAL_Delay(100);

    //Настройка сигналов синхронизации дисплея
    HOST_MEM_WR16(REG_HCYCLE, 260);         // Set H_Cycle to 548
    HOST_MEM_WR16(REG_HSIZE, 240);          // Set H_SIZE to 480
    HOST_MEM_WR16(REG_HOFFSET, 10);         // Set H_Offset to 43
    HOST_MEM_WR16(REG_HSYNC0, 0);           // Set H_SYNC_0 to 0
    HOST_MEM_WR16(REG_HSYNC1, 8);          // Set H_SYNC_1 to 41
    HOST_MEM_WR16(REG_VCYCLE, 336);         // Set V_Cycle to 292
    HOST_MEM_WR16(REG_VSIZE, 320);          // Set V_SIZE to 272
    HOST_MEM_WR16(REG_VOFFSET, 8);         // Set V_OFFSET to 12
    HOST_MEM_WR16(REG_VSYNC0, 0);           // Set V_SYNC_0 to 0
    HOST_MEM_WR16(REG_VSYNC1, 2);          // Set V_SYNC_1 to 10
    HOST_MEM_WR8(REG_SWIZZLE, 2);           // Set SWIZZLE to 0
    HOST_MEM_WR8(REG_PCLK_POL, 1);          // Set PCLK_POL to 1
    HOST_MEM_WR8(REG_CSPREAD, 1);           // Set CSPREAD to 1
    HOST_MEM_WR8(REG_ROTATE, 0);           //
    
    //Вывод первого фрейма
    HOST_MEM_WR32(RAM_DL+0, CLEAR_COLOR_RGB(255,0,0));  // Set Initial Color to green
    HOST_MEM_WR32(RAM_DL+4, CLEAR(1,1,1));            // Clear to the Initial Color
//    cmd_number(5, 5, 30, 0, 2345);
//    cmd_number(105, 105, 30, 0, 2345);
    HOST_MEM_WR32(RAM_DL+8, DISPLAY());               // End Display List
    HOST_MEM_WR8(REG_DLSWAP, DLSWAP_FRAME);           // Make this display list active on the next frame

    //Запуск DOTCLK
    HOST_MEM_WR8(REG_PCLK, 0x0A);                     // After this display is visible on the LCD

    HAL_Delay(250);
    
    /*
    //Чтение кода устройства контроллера дистплея и его статуса
    static uint16_t device_code; 
    device_code = readOledRegister(0x0F);
    device_code = readOledStatus(); //status read
    */

    //Направление луча дисплея
    writeOledRegister(0x03, 0x8020);        
    
    //Переведем в режим RGB
    writeOledRegister(0x02, 0x0180 | 1<<3 | 1<<2 | 1<<1); 
//    writeOledRegister(0x02, 0x0180 | 1<<1);

    //Настройки питания(??)
    writeOledRegister(0xF8, 0x000F);
    writeOledRegister(0xF9, 0x000F);

    //Настройка размеров окна
    writeOledRegister(0x35, 0x0000);
    writeOledRegister(0x36, 0x013F);
    writeOledRegister(0x37, 0x00EF);
  

    //Установка начального адреса GRAM
    writeOledRegister(0x20, 0x0000);
    writeOledRegister(0x21, 0x0000);        


    HAL_Delay(250); 
    
    //Запуск преобразователя для питания дисплея (светодиодов)
    HAL_GPIO_WritePin( OLED_ON_GPIO_Port,  OLED_ON_Pin,  GPIO_PIN_SET); 
    
    HAL_Delay(100);

    //Включить дисплей
    writeOledRegister(0x05, 0x0001);         
    writeOledRegister(0x22, 0x0000);
    oledInited = 1;
}

void oledControl(void)
{
    /*
    cmd(CMD_DLSTART);
    cmd(CLEAR_COLOR_RGB(0,0,0));
    cmd(CLEAR(1,1,1)); 
    
    cmd( COLOR_RGB(153, 204, 204) );
    cmd_number(45, 45, 30, 0, counter++);
    
    cmd(DISPLAY());
    cmd(CMD_SWAP); 
    */
  
    
    
}