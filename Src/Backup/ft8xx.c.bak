/**
  * @file    ft800.c
  * @brief   Библиотека графических контроллеров FT80x/FT81x
  *          Этот файл содержит функции инициализации и управления 
  *          данной микросхемой
  */
#include "spi_oled.h"
#include "ft8xx.h"
#include <stdlib.h>
#include <string.h>
#include "exeptions.h"

/**
 * @brief Функция чтения из памяти хоста
 * @details Для операций чтения по протоколу SPI хост отправляет два 
 *          нулевых бита, с последующими 22-мя битами адреса.
 *          затем идет пустой байт. После пустого байта микросхема 
 *          отвечает на каждый байт хоста прочитанными байтами данных
 * 
 * @param addr 24-х битный адрес команды
 * @param pnt Буффер для сохранения прочитанных данных
 * @param len Количество данных, которые необходимо прочитать
 */
void HOST_MEM_READ_STR(uint32_t addr, uint8_t *pnt, uint8_t len) {
//  FT_spi_select();
    if(pnt == NULL || len == 0)
    {
      exception(__FILE__,__FUNCTION__,__LINE__,"invalid buffer");
      return;
    }
    SPI_send(((addr>>16)&0x3F) );			// Send out bits 23:16 of addr, bits 7:6 of this byte must be 00 
    SPI_send(((addr>>8)&0xFF));       	// Send out bits 15:8 of addr
    SPI_send((addr&0xFF));            	// Send out bits 7:0 of addr

    SPI_send(0);                      	// Send out DUMMY (0) byte

    while(len--)                      	// While Len > 0 Read out n bytes
    *pnt++ = SPI_send(0);
  
//  FT_spi_deselect();
}

/*
    Function: HOST_MEM_WR_STR
    ARGS:     addr: 24 Bit Command Address 
              pnt:  input buffer of data to send
              len:  length of bytes to be send

    Description: Writes len(n) bytes of data from pnt (buffer) to addr
*/
/**
 * @brief Функция записи в память хоста
 * @details Для записи по протоколу SPI хост шлет бит 1 и бит 0, с
 *          последующими 22 битами адреса. Затем идут данные для 
 *          записи.
 * 
 * @param addr 24-х битный адрес команды
 * @param pnt Буффер из которого необходимо брать данные
 * @param len Количество данных, которые необходимо записать
 */
void HOST_MEM_WR_STR(uint32_t addr, uint8_t *pnt, uint32_t len) {
    if(pnt == NULL || len == 0)
    {
      exception(__FILE__,__FUNCTION__,__LINE__,"invalid buffer");
      return;
    }    
    FT_spi_select();
    SPI_send(((addr>>16)&0x3F)|0x80);     // Send out 23:16 of addr, bits 7:6 of this byte must be 10
    SPI_send(((addr>>8)&0xFF));           // Send out bits 15:8 of addr
    SPI_send((addr&0xFF));                // Send out bits 7:0 of addr

    while(len--)                          // While Len > 0 Write *pnt (then increment pnt)
    SPI_send(*pnt++);

    FT_spi_deselect();
}


/**
 * @brief Функция записи команды
 * @details При посылке команды хост отправляет 3-х байтную команду.
 *          Сначала посылаются биты 0 и 1, затем 6 бит команды. 
 *          Второй байт может быть либо 0, либо параметром этой 
 *          команды. Третий байт всегда равен 0х00.
 *          
 * 
 * @param CMD Команда, которую необходимо передать
 */
void HOST_CMD_WRITE(uint8_t CMD) {
    FT_spi_select(); 
    SPI_send((uint8_t)(CMD|0x40));        // Send out Command, bits 7:6 must be 01
    SPI_send(0x00);
    SPI_send(0x00);
    FT_spi_deselect();
}


/**
 * @brief Функция, приводящая микросхему в активное состояние
 * @details Является частным случаем команды HOST_CMD_WRITE
 */
void HOST_CMD_ACTIVE(void) {
    FT_spi_select();
    SPI_send(0x00);      
    SPI_send(0x00);
    SPI_send(0x00);
    FT_spi_deselect();
}


/**
 * @brief Функция записи слова размером 8 бит
 * 
 * @param addr Адрес места, куда необходимо провести запись
 * @param data Данные, которые необходимо записать
 */
void HOST_MEM_WR8(uint32_t addr, uint8_t data) {
    FT_spi_select();
    SPI_send((addr>>16)|0x80);
    SPI_send(((addr>>8)&0xFF));
    SPI_send((addr&0xFF));

    SPI_send(data);

    FT_spi_deselect();  
}

/**
 * @brief Функция записи слова размером 16 бит
 * 
 * @param addr Адрес места, куда необходимо провести запись
 * @param data Данные, которые необходимо записать
 */
void HOST_MEM_WR16(uint32_t addr, uint32_t data) {
    FT_spi_select();
    SPI_send((addr>>16)|0x80);
    SPI_send(((addr>>8)&0xFF));
    SPI_send((addr&0xFF));

    /* Little-Endian: Least Significant Byte to: smallest address */
    SPI_send( (uint8_t)((data&0xFF)) );    //byte 0
    SPI_send( (uint8_t)((data>>8)) );      //byte 1

    FT_spi_deselect();  
}


/**
 * @brief Функция записи слова размером 32 бита
 * 
 * @param addr Адрес места, куда необходимо провести запись
 * @param data Данные, которые необходимо записать
 */
void HOST_MEM_WR32(uint32_t addr, uint32_t data) {
    FT_spi_select();
    SPI_send((addr>>16)|0x80);
    SPI_send(((addr>>8)&0xFF));
    SPI_send((addr&0xFF));

    SPI_send( (uint8_t)(data&0xFF) );
    SPI_send( (uint8_t)((data>>8)&0xFF) );
    SPI_send( (uint8_t)((data>>16)&0xFF) );
    SPI_send( (uint8_t)((data>>24)&0xFF) );

    FT_spi_deselect();  
}
//void HOST_MEM_WR32(uint32_t addr, uint32_t data)
//{
//  FT_spi_select();
//  SPI_send((addr>>16)|0x80);
//  SPI_send(((addr>>8)&0xFF));
//  SPI_send((addr&0xFF));
//  SPI_send(0x00);
//
//  SPI_send( (uint8_t)((data>>24)&0xFF) );
//  SPI_send( (uint8_t)((data>>16)&0xFF) );
//  SPI_send( (uint8_t)((data>>8)&0xFF) );
//  SPI_send( (uint8_t)(data&0xFF) );
//  
//  FT_spi_deselect();  
//}


/**
 * @brief Функция чтения слова размером 8 бит
 * 
 * @param addr Адрес места, откуда необходимо провести чтение
 * @return Прочитанные данные
 */   
uint8_t HOST_MEM_RD8(uint32_t addr) {
    static volatile uint8_t data_in;


    FT_spi_select();
    SPI_send((uint8_t)((addr>>16)&0x3F));
    SPI_send((uint8_t)((addr>>8)&0xFF));
    SPI_send((uint8_t)(addr));
    SPI_send(0);

    data_in = SPI_send(0);

    FT_spi_deselect();
    return data_in;
}

/**
 * @brief Функция чтения слова размером 16 бит
 * 
 * @param addr Адрес места, откуда необходимо провести чтение
 * @return Прочитанные данные
 */ 
uint32_t HOST_MEM_RD16(uint32_t addr) {
    uint8_t data_in = 0;
    uint32_t data = 0;
    uint8_t i;

    FT_spi_select();
    SPI_send(((addr>>16)&0x3F));
    SPI_send(((addr>>8)&0xFF));
    SPI_send((addr&0xFF));
    SPI_send(0);

    for(i=0;i<2;i++) {
        data_in = SPI_send(0);
        data |= ( ((uint32_t)data_in) << (8*i) );
    }

    FT_spi_deselect();
    return data;
}

/**
 * @brief Функция чтения слова размером 32 бита
 * 
 * @param addr Адрес места, откуда необходимо провести чтение
 * @return Прочитанные данные
 */ 
uint32_t HOST_MEM_RD32(uint32_t addr) {
    uint8_t data_in = 0;
    uint32_t data = 0;
    uint8_t i;

    FT_spi_select();
    SPI_send(((addr>>16)&0x3F));
    SPI_send(((addr>>8)&0xFF));
    SPI_send((addr&0xFF));
    SPI_send(0);

    for(i=0;i<4;i++) {
        data_in = SPI_send(0);
        data |= ( ((uint32_t)data_in) << (8*i) );
    }

    FT_spi_deselect();
    return data;
}

/* CMD Functions *****************************************************************/
/**
 * @brief ???Функция посылает д
 * @details [long description]
 * 
 * @param data [description]
 * @return [description]
 */
uint8_t cmd_execute(uint32_t data)
{
	uint32_t cmdBufferRd = 0;
    uint32_t cmdBufferWr = 0;
    
    cmdBufferRd = HOST_MEM_RD32(REG_CMD_READ);
    cmdBufferWr = HOST_MEM_RD32(REG_CMD_WRITE);
    
    uint32_t cmdBufferDiff = cmdBufferWr-cmdBufferRd;
    
    if( (4096-cmdBufferDiff) > 4)
    {
        HOST_MEM_WR32(RAM_CMD + cmdBufferWr, data);
        HOST_MEM_WR32(REG_CMD_WRITE, cmdBufferWr + 4);
        return 1;
    }
    return 0;
}

/**
 * @brief ???Функция 
 * @details [long description]
 * 
 * @param data Данные, которые необходимо послать
 * @return Возвращает 1, если команда послана успешно, 0 - когда 
 *          неуспешно
 */
uint8_t cmd(uint32_t data)
{
	uint8_t tryCount = 255;
	for(tryCount = 255; tryCount > 0; --tryCount)
	{
		if(cmd_execute(data)) { return 1; }
	}
	return 0;
}

/**
 * @brief ???Возвращает состояние микросхемы
 * @details [long description]
 * 
 * @return 
 */
uint8_t cmd_ready(void)
{
    uint32_t cmdBufferRd = HOST_MEM_RD32(REG_CMD_READ);
    uint32_t cmdBufferWr = HOST_MEM_RD32(REG_CMD_WRITE);
    
    return (cmdBufferRd == cmdBufferWr) ? 1 : 0;
}

/*** Track *************************************************************************/
/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param x [description]
 * @param y [description]
 * @param w [description]
 * @param h [description]
 * @param tag [description]
 */
void cmd_track(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag)
{
    cmd(CMD_TRACK);
    cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)h<<16)|(w & 0xffff) );
    cmd( (uint32_t)tag );
}

/*** Draw Spinner ******************************************************************/
/**
 * @brief Нарисовать спиннер
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param style Стиль спиннера
 * @param scale Размер спиннера
 */
void cmd_spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale)
{    
    cmd(CMD_SPINNER);
    cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)scale<<16)|style );
    
}

    
    
/**
 * @brief Нарисовать переключатель
 * 

 */
void cmd_toogle(int16_t x, 
                int16_t y, 
                int16_t w, 
                int16_t font,
                uint16_t options,
                uint16_t state,
                const uint8_t* s) {    
                    
            if(s == NULL)
            {
              exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
              return;
            }            
	uint16_t i,j,q;
	const uint16_t length = strlen((char const *) s);
	if(!length) return ;	
    
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
    
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)s[q+3]<<24 | (uint32_t)s[q+2]<<16 | (uint32_t)s[q+1]<<8 | (uint32_t)s[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)s[q] << (j*8);
	}
	

    
    
    cmd(CMD_TOGGLE);
    cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)font<<16)|(w & 0xffff) );
    cmd( ((uint32_t)state<<16)| options );
    
    

	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
    
    free(data);    
}    
    
/*** Draw Slider *******************************************************************/
/**
 * @brief Нарисовать слайдер
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param w Ширина
 * @param h Высота
 * @param options Опции
 * @param val Значение
 * @param range Пределы
 */
void cmd_slider(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
	cmd(CMD_SLIDER);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
	cmd( ((uint32_t)h<<16)|(w & 0xffff) );
	cmd( ((uint32_t)val<<16)|(options & 0xffff) );
	cmd( (uint32_t)range );
}



void cmd_setfont(uint32_t font, uint32_t ptr){
    cmd(CMD_SETFONT);
    cmd((uint32_t)font);
    cmd((uint32_t)ptr);
}


/*** Draw Text *********************************************************************/
/**
 * @brief Написать текст по заданным координатам
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param font Шрифт
 * @param options Опции
 * @param str Указатель на строку, которую необходимо вывести на экран
 */
void cmd_text(int16_t x, int16_t y, int16_t font, uint16_t options, const uint8_t* str)
{
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	if(str == NULL)
            {
              exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
              return;
            }  
	uint16_t i,j,q;
	const uint16_t length = strlen((const char*) str);
	if(!length) return ;	
//	uint32_t data[20];
    
//    uint32_t* data = (uint32_t*) realloc((length/4)+1, sizeof(uint32_t));
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
//	uint32_t* data = (uint32_t*) pvPortMalloc((length/4)+1);
    
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_TEXT);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
//	vPortFree(data);
    free(data);
}


/*** Draw Number *********************************************************************/
/**
 * @brief Вывести число на экран
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param font Шрифт
 * @param options Опции
 * @param number Число
 */
void cmd_number(int16_t x, int16_t y, int16_t font, uint16_t options, uint32_t number)
{
	cmd(CMD_NUMBER);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	cmd( (uint32_t) number);
}

void cmd_text_var(int16_t x, int16_t y, int16_t font, uint16_t options, char* str, uint8_t length)
{
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	if(str == NULL)
            {
              exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
              return;
            }  
	uint16_t i,j,q;
	if(!length) return ;	
//    uint32_t data[20];
//    uint32_t* data = (uint32_t*) realloc((length/4)+1, sizeof(uint32_t));
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
//	uint32_t* data = (uint32_t*) pvPortMalloc((length/4)+1);
    
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_TEXT);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
//    for(j=length; j>0; --j)
//	{
//		cmd(str[j]);
//	}
//	vPortFree(data);
    free(data);
}

/*** Draw Button *******************************************************************/
/**
 * @brief Нарисовать кнопку
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param w Ширина
 * @param h Высота
 * @param font Шрифт
 * @param options Опции
 * @param str Указатель на строку, которую необходимо вывести на 
 *              кнопку
 */
void cmd_button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str)
{	
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	if(str == NULL)
            {
              exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
              return;
            } 
	uint16_t i,j,q;
	const uint16_t length = strlen(str);
	if(!length) return ;
	
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
	
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_BUTTON);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
	cmd( ((uint32_t)h<<16)|(w & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
	free(data);
}

/**
 * @brief Нарисовать клавиатуру
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param w Ширина
 * @param h Высота
 * @param font Шрифт
 * @param options Опции
 * @param str Указатель на строку, которую необходимо вывести на 
 *              клавиатуру
 */
void cmd_keys(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str)
{
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	if(str == NULL)
            {
              exception(__FILE__,__FUNCTION__,__LINE__,"invalid pointer");
              return;
            } 
	uint16_t i,j,q;
	const uint16_t length = strlen(str);
	if(!length) return ;
	
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
	
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_KEYS);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
	cmd( ((uint32_t)h<<16)|(w & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
	free(data);
}

/**
 * @brief Нарисовать индикатор выполнения процесса
 * @details [long description]
 * 
 * @param x Расположение по оси Х
 * @param y Расположение по оси У
 * @param w Ширина
 * @param h Высота
 * @param options Опции
 * @param val Отображаемое значение (от 0 до range)
 * @param range Максимальное значение
 */
void cmd_progress( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range ){
	cmd(CMD_PROGRESS);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)h<<16)|(w & 0xffff) );
	cmd( ((uint32_t)val<<16)|(options & 0xffff) );
	cmd( (uint32_t)range );   
}

/**
 * @brief Заполнить нулями участок памяти
 * 
 * @param ptr Начальный адрес участка памяти
 * @param num Количество байт в участке
 */
void cmd_memzero(uint32_t ptr, uint32_t num)
{
	cmd(CMD_MEMZERO);
	cmd(ptr);
	cmd(num);
}

/**
 * @brief Установить цвет переднего плана
 * 
 * @param c Цвет
 */
void cmd_fgcolor(uint32_t c)
{
	cmd(CMD_FGCOLOR);
	cmd(c);
}

/**
 * @brief Установить цвет заднего плана
 * 
 * @param c Цвет
 */
void cmd_bgcolor(uint32_t c)
{
	cmd(CMD_BGCOLOR);
	cmd(c);
}

/**
 * @brief Установить цвет градиента
 * 
 * @param c Цвет
 */
void cmd_gradcolor(uint32_t c)
{
	cmd(CMD_GRADCOLOR);
	cmd(c);
}

/*** Draw Gradient *****************************************************************/
/**
 * @brief Нарисовать градиент
 * 
 * @param x0 Координата Х левого верхнего угла
 * @param y0 Координата У левого верхнего угла
 * @param rgb0 Цвет левого верхнего угла
 * @param x1 Координата Х правого нижнего угла
 * @param y1 Координата У правого нижнего угла
 * @param rgb1 Цвет правого нижнего угла
 */
void cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
	cmd(CMD_GRADIENT);
	cmd( ((uint32_t)y0<<16)|(x0 & 0xffff) );
	cmd(rgb0);
	cmd( ((uint32_t)y1<<16)|(x1 & 0xffff) );
	cmd(rgb1);
}

/*** Matrix Functions **************************************************************/
void cmd_loadidentity(void)
{
	cmd(CMD_LOADIDENTITY);
}

void cmd_setmatrix(void)
{
	cmd(CMD_SETMATRIX);
}

/**
 * @brief Применить поворот к текущей матрице
 * 
 * @param angle Угол поворота
 */
void cmd_rotate(int32_t angle)
{
	cmd(CMD_ROTATE);
	cmd(angle);
}

/**
 * @brief Масштабировать текущую матрицу
 * @details [long description]
 * 
 * @param sx Множитель по оси Х (знаковое 16-тибитное)
 * @param sy Множитель по оси У (знаковое 16-тибитное)
 */
void cmd_scale(float sx,
               float sy ){
    cmd(CMD_SCALE);
	cmd((uint32_t) (sx * (float)0xFFFF) );
	cmd((uint32_t) (sy * (float)0xFFFF));    
}
void cmd_translate(int32_t tx, 
                   int32_t ty) {
	cmd(CMD_TRANSLATE);
	cmd(tx);
	cmd(ty);
}


/**
 * @brief Инициализация контроллера дисплея
 * @details [long description]
 * @return Результат выполнения операции
 */
uint8_t initFT800(void){   

    
#ifdef FT81x
    #ifdef LCD_HITACHI
    
        //Ext Clock
        HOST_CMD_WRITE(CMD_CLKEXT);         // Send CLK_EXT Command (0x44)
//            for (uint8_t j=0; j<10; j++)
//                for(uint32_t i = 0; i<16000; i++); 
            
        FT_spi_select();
        SPI_send(0x62);      
        SPI_send(1<<6 | 4<<0);
        SPI_send(0x00);
        FT_spi_deselect();

//            for (uint8_t j=0; j<10; j++)
//                for(uint32_t i = 0; i<16000; i++);    
        
        //    for (uint16_t j=0; j<500; j++)
    //        for(uint32_t i = 0; i<16000; i++);
        HOST_CMD_ACTIVE();
        

        
    //    for (uint16_t j=0; j<100; j++)
    //        for(uint32_t i = 0; i<16000; i++);
        
        for (uint32_t i = 0; i<0xFF; i++);
        //Read Dev ID
        dev_id = HOST_MEM_RD8(REG_ID);      // Read device id
        if(dev_id != 0x7C)                  // Device ID should always be 0x7C
        {   
            return 1;
        }
  
        HOST_MEM_WR16(REG_PWM_HZ, 0);                // Backlight PWM frequency
        HOST_MEM_WR8(REG_PWM_DUTY, 0x00);                 // Backlight PWM duty   
//        for (uint8_t j=0; j<100; j++)
//            for(uint32_t i = 0; i<16000; i++);
        
        HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
        HOST_MEM_WR8(REG_GPIO, 0x00);			// Set REG_GPIO to 0 to turn off the LCD DISP signal
        HOST_MEM_WR8(REG_PCLK, 0x00);      		// Pixel Clock Output disable  

        
        HOST_MEM_WR16(REG_HCYCLE, 548);         // Set H_Cycle to 548
        HOST_MEM_WR16(REG_HSIZE, 480);          // Set H_SIZE to 480
        HOST_MEM_WR16(REG_HOFFSET, 43);         // Set H_Offset to 43
        HOST_MEM_WR16(REG_HSYNC0, 0);           // Set H_SYNC_0 to 0
        HOST_MEM_WR16(REG_HSYNC1, 41);          // Set H_SYNC_1 to 41
        HOST_MEM_WR16(REG_VCYCLE, 292);         // Set V_Cycle to 292
        HOST_MEM_WR16(REG_VSIZE, 272);          // Set V_SIZE to 272
        HOST_MEM_WR16(REG_VOFFSET, 12);         // Set V_OFFSET to 12
        HOST_MEM_WR16(REG_VSYNC0, 0);           // Set V_SYNC_0 to 0
        HOST_MEM_WR16(REG_VSYNC1, 10);          // Set V_SYNC_1 to 10
        HOST_MEM_WR8(REG_SWIZZLE, 0);           // Set SWIZZLE to 0
        HOST_MEM_WR8(REG_PCLK_POL, 1);          // Set PCLK_POL to 1
        HOST_MEM_WR8(REG_CSPREAD, 1);           // Set CSPREAD to 1
        
    //	/* configure touch & audio */
    	HOST_MEM_WR8(REG_TOUCH_MODE, 0x03);     	//set touch on: continous
    	HOST_MEM_WR8(REG_TOUCH_ADC_MODE, 0x01); 	//set touch mode: differential
    	HOST_MEM_WR8(REG_TOUCH_OVERSAMPLE, 0x0F); 	//set touch oversampling to max
    	HOST_MEM_WR16(REG_TOUCH_RZTHRESH, 5000);	//set touch resistance threshold
    //	HOST_MEM_WR8(REG_VOL_SOUND, 0xFF);      	//set the volume to maximum

        /* write first display list */
        HOST_MEM_WR32(RAM_DL+0, CLEAR_COLOR_RGB(0,127,0));  // Set Initial Color to BLACK
        HOST_MEM_WR32(RAM_DL+4, CLEAR(1,1,1));            // Clear to the Initial Color
        HOST_MEM_WR32(RAM_DL+8, DISPLAY());               // End Display List

        HOST_MEM_WR8(REG_DLSWAP, DLSWAP_FRAME);           // Make this display list active on the next frame

//        HOST_MEM_WR16(REG_PWM_HZ, 0);                // Backlight PWM frequency
//        HOST_MEM_WR8(REG_PWM_DUTY, 0x00);                 // Backlight PWM duty   
//        for (uint8_t j=0; j<100; j++)
//            for(uint32_t i = 0; i<16000; i++);
        
//        HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
//        HOST_MEM_WR8(REG_GPIO, 0x00);			// Set REG_GPIO to 0 to turn off the LCD DISP signal
//        HOST_MEM_WR8(REG_PCLK, 0x00);      		// Pixel Clock Output disable    


         for (uint8_t j=0; j<200; j++)
            for(uint32_t i = 0; i<16000; i++);      

        
        HOST_MEM_WR8(REG_PCLK, 0x05);                     // After this display is visible on the LCD
        HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
        HOST_MEM_WR8(REG_GPIO, 0x80);                     // Enable Disp (if used)    
        
//        for (uint8_t j=0; j<100; j++)
//            for(uint32_t i = 0; i<16000; i++);
        HOST_MEM_WR16(REG_PWM_HZ, 1000);                // Backlight PWM frequency
        HOST_MEM_WR8(REG_PWM_DUTY, 100);                 // Backlight PWM duty          


    #elif defined LCD_MITSUBISHI
        //Ext Clock
//            for (uint8_t j=0; j<100; j++)
//                for(uint32_t i = 0; i<16000; i++); 
//            
//        FT_spi_select();
//        SPI_send(0x62);      
//        SPI_send(0x44);
//        SPI_send(0x00);
//        FT_spi_deselect();
//
//            for (uint8_t j=0; j<10; j++)
//                for(uint32_t i = 0; i<16000; i++);    
        
        //    for (uint16_t j=0; j<500; j++)
    //        for(uint32_t i = 0; i<16000; i++);
//        HOST_CMD_WRITE(CMD_CORERST);
//        HOST_CMD_WRITE(CMD_SLEEP);
         
        HOST_CMD_WRITE(CMD_CLKEXT);         // Send CLK_EXT Command (0x44)
        FT_spi_select();
        SPI_send(0x61);      
        SPI_send(1<<6 | 5<<0);
        SPI_send(0x00);
        FT_spi_deselect();
        HOST_CMD_ACTIVE();
        

        
    //    for (uint16_t j=0; j<100; j++)
    //        for(uint32_t i = 0; i<16000; i++);
        
        for (uint32_t i = 0; i<0xFF; i++);
        //Read Dev ID
        dev_id = HOST_MEM_RD8(REG_ID);      // Read device id
        if(dev_id != 0x7C)                  // Device ID should always be 0x7C
        {   
            return 1;
        }
        
        HOST_MEM_WR16(REG_HCYCLE, 1056);         // Set H_Cycle to 548
        HOST_MEM_WR16(REG_HSIZE, 800);          // Set H_SIZE to 480
        HOST_MEM_WR16(REG_HOFFSET, 203);         // Set H_Offset to 43
        HOST_MEM_WR16(REG_HSYNC0, 0);           // Set H_SYNC_0 to 0
        HOST_MEM_WR16(REG_HSYNC1, 128);          // Set H_SYNC_1 to 41
        
        HOST_MEM_WR16(REG_VCYCLE, 525);         // Set V_Cycle to 292
        HOST_MEM_WR16(REG_VSIZE, 480);          // Set V_SIZE to 272
        HOST_MEM_WR16(REG_VOFFSET, 35);         // Set V_OFFSET to 12
        HOST_MEM_WR16(REG_VSYNC0, 0);           // Set V_SYNC_0 to 0
        HOST_MEM_WR16(REG_VSYNC1, 2);          // Set V_SYNC_1 to 10
        
        HOST_MEM_WR8(REG_SWIZZLE, 0);           // Set SWIZZLE to 0
        HOST_MEM_WR8(REG_PCLK_POL, 1);          // Set PCLK_POL to 1
        HOST_MEM_WR8(REG_CSPREAD, 1);           // Set CSPREAD to 1

        
        
        
    //	/* configure touch & audio */
    //	HOST_MEM_WR8(REG_TOUCH_MODE, 0x03);     	//set touch on: continous
    //	HOST_MEM_WR8(REG_TOUCH_ADC_MODE, 0x01); 	//set touch mode: differential
    //	HOST_MEM_WR8(REG_TOUCH_OVERSAMPLE, 0x0F); 	//set touch oversampling to max
    //	HOST_MEM_WR16(REG_TOUCH_RZTHRESH, 5000);	//set touch resistance threshold
    //	HOST_MEM_WR8(REG_VOL_SOUND, 0xFF);      	//set the volume to maximum

        /* write first display list */
        HOST_MEM_WR32(RAM_DL+0, CLEAR_COLOR_RGB(127,0,0));  // Set Initial Color to BLACK
        HOST_MEM_WR32(RAM_DL+4, CLEAR(1,1,1));            // Clear to the Initial Color
        HOST_MEM_WR32(RAM_DL+8, DISPLAY());               // End Display List

        HOST_MEM_WR8(REG_DLSWAP, DLSWAP_FRAME);           // Make this display list active on the next frame

        HOST_MEM_WR16(REG_PWM_HZ, 0);                // Backlight PWM frequency
        HOST_MEM_WR8(REG_PWM_DUTY, 0x00);                 // Backlight PWM duty   
//        for (uint8_t j=0; j<100; j++)
//            for(uint32_t i = 0; i<16000; i++);
        
        HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
        HOST_MEM_WR8(REG_GPIO, 0x00);			// Set REG_GPIO to 0 to turn off the LCD DISP signal
        HOST_MEM_WR8(REG_PCLK, 0x00);      		// Pixel Clock Output disable    

        for (uint8_t j=0; j<200; j++)
            for(uint32_t i = 0; i<16000; i++);
        
        
        HOST_MEM_WR8(REG_PCLK, 2);                     // After this display is visible on the LCD
        HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
        HOST_MEM_WR8(REG_GPIO, 0x80);                     // Enable Disp (if used)    
        
//        for (uint8_t j=0; j<100; j++)
//            for(uint32_t i = 0; i<16000; i++);
        HOST_MEM_WR16(REG_PWM_HZ, 1000);                // Backlight PWM frequency
        HOST_MEM_WR8(REG_PWM_DUTY, 0x50);                 // Backlight PWM duty    

       
    #endif    

    

    
#elif defined FT80x
    LPC_GPIO1->CLR.BIT.P29 = 1;
    LPC_GPIO1->SET.BIT.P29 = 1;

	//WAKE
	HOST_CMD_ACTIVE();
   
    //Ext Clock
	HOST_CMD_WRITE(CMD_CLKEXT);         // Send CLK_EXT Command (0x44)

    //	//PLL (48M) Clock
	HOST_CMD_WRITE(CMD_CLK48M);         // Send CLK_48M Command (0x62)

    
    for (uint32_t i = 0; i<0xFF; i++);
	//Read Dev ID
	while(dev_id != 0x7C)
        dev_id = HOST_MEM_RD8(REG_ID);      // Read device id
//	if(dev_id != 0x7C)                  // Device ID should always be 0x7C
//	{   
//		return 1;
//	}
    
	HOST_MEM_WR8(REG_GPIO, 0x00);			// Set REG_GPIO to 0 to turn off the LCD DISP signal
	HOST_MEM_WR8(REG_PCLK, 0x00);      		// Pixel Clock Output disable

//	HOST_MEM_WR16(REG_HCYCLE, 408);         // Set H_Cycle to 548
//	HOST_MEM_WR16(REG_HOFFSET, 70);         // Set H_Offset to 43
//	HOST_MEM_WR16(REG_HSYNC0, 0);           // Set H_SYNC_0 to 0
//	HOST_MEM_WR16(REG_HSYNC1, 10);          // Set H_SYNC_1 to 41
//	HOST_MEM_WR16(REG_VCYCLE, 263);         // Set V_Cycle to 292
//	HOST_MEM_WR16(REG_VOFFSET, 13);         // Set V_OFFSET to 12
//	HOST_MEM_WR16(REG_VSYNC0, 0);           // Set V_SYNC_0 to 0
//	HOST_MEM_WR16(REG_VSYNC1, 2);          // Set V_SYNC_1 to 10
//	HOST_MEM_WR8(REG_SWIZZLE, 0);           // Set SWIZZLE to 0
//	HOST_MEM_WR8(REG_PCLK_POL, 0);          // Set PCLK_POL to 1
//	HOST_MEM_WR8(REG_CSPREAD, 1);           // Set CSPREAD to 1
//	HOST_MEM_WR16(REG_HSIZE, 320);          // Set H_SIZE to 480
//	HOST_MEM_WR16(REG_VSIZE, 240);          // Set V_SIZE to 272

    HOST_MEM_WR16(REG_HCYCLE, 548);         // Set H_Cycle to 548
    HOST_MEM_WR16(REG_HOFFSET, 43);         // Set H_Offset to 43
    HOST_MEM_WR16(REG_HSYNC0, 0);           // Set H_SYNC_0 to 0
    HOST_MEM_WR16(REG_HSYNC1, 41);          // Set H_SYNC_1 to 41
    HOST_MEM_WR16(REG_VCYCLE, 292);         // Set V_Cycle to 292
    HOST_MEM_WR16(REG_VOFFSET, 12);         // Set V_OFFSET to 12
    HOST_MEM_WR16(REG_VSYNC0, 0);           // Set V_SYNC_0 to 0
    HOST_MEM_WR16(REG_VSYNC1, 10);          // Set V_SYNC_1 to 10
    HOST_MEM_WR8(REG_SWIZZLE, 0);           // Set SWIZZLE to 0
    HOST_MEM_WR8(REG_PCLK_POL, 1);          // Set PCLK_POL to 1
    HOST_MEM_WR8(REG_CSPREAD, 1);           // Set CSPREAD to 1
    HOST_MEM_WR16(REG_HSIZE, 480);          // Set H_SIZE to 480
    HOST_MEM_WR16(REG_VSIZE, 272);          // Set V_SIZE to 272

    
	/* configure touch & audio */
//	HOST_MEM_WR8(REG_TOUCH_MODE, 0x03);     	//set touch on: continous
//	HOST_MEM_WR8(REG_TOUCH_ADC_MODE, 0x01); 	//set touch mode: differential
//	HOST_MEM_WR8(REG_TOUCH_OVERSAMPLE, 0x0F); 	//set touch oversampling to max
//	HOST_MEM_WR16(REG_TOUCH_RZTHRESH, 5000);	//set touch resistance threshold
//	HOST_MEM_WR8(REG_VOL_SOUND, 0xFF);      	//set the volume to maximum

	/* write first display list */
	HOST_MEM_WR32(RAM_DL+0, CLEAR_COLOR_RGB(127,0,0));  // Set Initial Color to BLACK
	HOST_MEM_WR32(RAM_DL+4, CLEAR(1,1,1));            // Clear to the Initial Color
	HOST_MEM_WR32(RAM_DL+8, DISPLAY());               // End Display List

	HOST_MEM_WR8(REG_DLSWAP, DLSWAP_FRAME);           // Make this display list active on the next frame

	HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
	HOST_MEM_WR8(REG_GPIO, 0x80);                     // Enable Disp (if used)
	HOST_MEM_WR16(REG_PWM_HZ, 0x00FA);                // Backlight PWM frequency
	HOST_MEM_WR8(REG_PWM_DUTY, 0x80);                 // Backlight PWM duty    

	HOST_MEM_WR8(REG_PCLK, 0x05);                     // After this display is visible on the LCD

#endif
	return 0;
}


/* Clear Screen */
/**
 * @brief Очистить экран
 * @details Функция очищает экран полностью заполняя его черным цветом
 */
void clrscr(void){
	cmd(CMD_DLSTART);
	cmd(CLEAR_COLOR_RGB(0,0,0));
	cmd(CLEAR(1,1,1));
    cmd(DISPLAY());
	cmd(CMD_SWAP);
    
}

//Воробей 17.07.2018

//суперудобная функция нарисовать линию
void cmd_line(uint16_t x1, uint16_t y1,//координаты первой точки
              uint16_t x2, uint16_t y2,//координаты второй точки
              uint16_t width)//толщина
{
        cmd( BEGIN(LINES) );
        cmd( LINE_WIDTH(width * 16) );
	cmd( VERTEX2F(x1 * 16,y1 * 16) );
	cmd( VERTEX2F(x2 * 16,y2 * 16) );
        cmd( END() );
}