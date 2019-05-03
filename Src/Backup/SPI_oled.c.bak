#include "SPI_oled.h"
//#include "exeptions.h"                //сдесь нельзя использовать исключения т.к. при краше мы используем sendSpiData для обращения с монитором.
uint8_t buf;

/**
 * @brief Посылка данных по интерфейсу SPI
 * После посылки сообщения сразу же приходит ответ
 * 
 * @param data Данные для передачи
 * @return Принятые данные
 */
uint8_t sendSpiData(SPI_TypeDef *spi, char data){
    /*
    if(spi == NULL || data == 0)
    {
      exception(__FILE__,__FUNCTION__,__LINE__,"invalid buffer");
      return 0;
    }
    */
    if (!(spi->CR1 & SPI_CR1_SPE)) spi->CR1 |= SPI_CR1_SPE;
        
    //Ждем пока не очистится буфер передачи
    while(!(spi->SR & SPI_SR_TXE)); 

    //Ждем пока не очистится буфер приема
    while(spi->SR & SPI_SR_RXNE){
        buf = spi->DR;
    }
    
    //Важно!! Компилятор не может понять какой размер использовать
    //для буфера SPIx->DR, основываясь на правостороннем аргументе
    //data, поэтому необходимо ему помочь приведением регистра DR
    //к типу uint8_t, иначе будет посылать по 2 байта
    //
    //https://electronics.stackexchange.com/questions/324439/stm32-spi-slave-send-data-problem
    *(uint8_t *)&spi-> DR = data; 
    
    while(spi->SR & SPI_SR_BSY); 
    
    return spi->DR;
}


/**
 * @brief Функция, которая отвечает за обмен данными между МК и FT8xx. 
 * Переопределяет функцию из ft8xx
 * 
 * @param data Данные для передачи
 * @return Принятые данные
 */
uint8_t SPI_send(uint8_t data){
    return sendSpiData(SPI1, data);
}

/**
 * @brief Выбор в качестве слейва микросхему FT8xx
 */
void FT_spi_select(void){
    SPI1->CR1 &= ~(1<<3 | 1<<1 | 1<<0); //Установить уровень CLK в 0, когда неактивен
                                        //и предделитель частоты на 1/4
    HAL_GPIO_WritePin( CSN_EVE_GPIO_Port,  CSN_EVE_Pin,  GPIO_PIN_RESET);  // Set chip select low 
}

/**
 * @brief Отключить микросхему FT8xx от линии SPI
 */
void FT_spi_deselect(void){
    HAL_GPIO_WritePin( CSN_EVE_GPIO_Port,  CSN_EVE_Pin,  GPIO_PIN_SET);  // Set chip select low 
}


/**
 * @brief Выбор в качестве слейва микросхему FT8xx
 */
void OLED_spi_select(void){
    SPI1->CR1 |= 3<<3 | 1<<1 | 1<<0; //Установить уровень CLK в 1, когда неактивен
                                     //и предделитель частоты на 1/16
    HAL_GPIO_WritePin( NCSB_GPIO_Port,  NCSB_Pin,  GPIO_PIN_RESET);  // Set chip select low 
}

/**
 * @brief Отключить микросхему FT8xx от линии SPI
 */
void OLED_spi_deselect(void){
    HAL_GPIO_WritePin( NCSB_GPIO_Port,  NCSB_Pin,  GPIO_PIN_SET);  // Set chip select low 
}
