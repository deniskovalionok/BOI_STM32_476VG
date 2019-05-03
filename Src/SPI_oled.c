#include "SPI_oled.h"
//#include "exeptions.h"                //����� ������ ������������ ���������� �.�. ��� ����� �� ���������� sendSpiData ��� ��������� � ���������.
uint8_t buf;

/**
 * @brief ������� ������ �� ���������� SPI
 * ����� ������� ��������� ����� �� �������� �����
 * 
 * @param data ������ ��� ��������
 * @return �������� ������
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
        
    //���� ���� �� ��������� ����� ��������
    while(!(spi->SR & SPI_SR_TXE)); 

    //���� ���� �� ��������� ����� ������
    while(spi->SR & SPI_SR_RXNE){
        buf = spi->DR;
    }
    
    //�����!! ���������� �� ����� ������ ����� ������ ������������
    //��� ������ SPIx->DR, ����������� �� �������������� ���������
    //data, ������� ���������� ��� ������ ����������� �������� DR
    //� ���� uint8_t, ����� ����� �������� �� 2 �����
    //
    //https://electronics.stackexchange.com/questions/324439/stm32-spi-slave-send-data-problem
    *(uint8_t *)&spi-> DR = data; 
    
    while(spi->SR & SPI_SR_BSY); 
    
    return spi->DR;
}


/**
 * @brief �������, ������� �������� �� ����� ������� ����� �� � FT8xx. 
 * �������������� ������� �� ft8xx
 * 
 * @param data ������ ��� ��������
 * @return �������� ������
 */
uint8_t SPI_send(uint8_t data){
    return sendSpiData(SPI1, data);
}

/**
 * @brief ����� � �������� ������ ���������� FT8xx
 */
void FT_spi_select(void){
    SPI1->CR1 &= ~(1<<3 | 1<<1 | 1<<0); //���������� ������� CLK � 0, ����� ���������
                                        //� ������������ ������� �� 1/4
    HAL_GPIO_WritePin( CSN_EVE_GPIO_Port,  CSN_EVE_Pin,  GPIO_PIN_RESET);  // Set chip select low 
}

/**
 * @brief ��������� ���������� FT8xx �� ����� SPI
 */
void FT_spi_deselect(void){
    HAL_GPIO_WritePin( CSN_EVE_GPIO_Port,  CSN_EVE_Pin,  GPIO_PIN_SET);  // Set chip select low 
}


/**
 * @brief ����� � �������� ������ ���������� FT8xx
 */
void OLED_spi_select(void){
    SPI1->CR1 |= 3<<3 | 1<<1 | 1<<0; //���������� ������� CLK � 1, ����� ���������
                                     //� ������������ ������� �� 1/16
    HAL_GPIO_WritePin( NCSB_GPIO_Port,  NCSB_Pin,  GPIO_PIN_RESET);  // Set chip select low 
}

/**
 * @brief ��������� ���������� FT8xx �� ����� SPI
 */
void OLED_spi_deselect(void){
    HAL_GPIO_WritePin( NCSB_GPIO_Port,  NCSB_Pin,  GPIO_PIN_SET);  // Set chip select low 
}
