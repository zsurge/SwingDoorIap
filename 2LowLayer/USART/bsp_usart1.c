/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : drv_usart1.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����1��ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#include "stm32f4xx_conf.h"
#include "bsp_usart1.h"

//Private Function Prototype
static uint32_t drv_Usart1_ReadByte(uint8_t *key);

//USART2 get a byte DR
//key: pointer to store data
//return: 
//      0:fail
//      1:success
static uint32_t drv_Usart1_ReadByte(uint8_t *key)
{

  if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)USART1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}


//USART2 get a byte from HyperTerminal
//return: Rx byte
uint8_t bsp_Usart1_GetByte(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (drv_Usart1_ReadByte((uint8_t*)&key)) break;
    
  }
  return key;

}

//Private Function Prototype
//DMA2 for usart1 configuration
static void Usart1_DMA2_Config(void)  
{  
    uint8_t dummy_data = 0;

    DMA_InitTypeDef DMA_InitStructure;
    

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
    
    //reset 
    DMA_DeInit(DMA2_Stream7); //���ʹ�ô���1��ֻ�� DMA2 ͨ��4��������7
    
    //DMA configuration  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; //DMA�����ַ
    DMA_InitStructure.DMA_BufferSize = 1;   //���ݴ���������ʼ��ʱ�ȶ�Ϊ1
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&dummy_data; //DMA �洢��0��ַ     
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; //�洢��������ģʽ    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�洢������ģʽ 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������ݳ���:8λ 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //�洢�����ݳ���:8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;  // ʹ����ͨģʽ    
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //�е����ȼ�    
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  //�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //����ͻ�����δ���
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_Full; 
                
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);    //��ʼ��DMA Stream     
    DMA_Cmd(DMA2_Stream7,ENABLE);                  //����DMA���� 
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���1��DMA����  
}

//usart1 send data via DMA
//@ int8_t *buf: data buffer ready to send
//@ int16_t len: data length
void bsp_Usart1_DMA_Send(uint8_t *buf, uint16_t len)
{  
    DMA_InitTypeDef DMA_InitStructure;
    //wait unitl last package is sent
    while(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7)==RESET); //�ȴ�DMA������ 
    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
    //DMA configuration  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; 
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buf;      
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;     
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_HalfFull; 
                
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);         //��ʼ��DMA Stream   
    DMA_Cmd(DMA2_Stream7,ENABLE);                       //����DMA����
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);      //ʹ�ܴ���1��DMA����
}

//usart2 configuration
//default setting is 8,n,1
//@ int baudrate: the desired baudrate
void bsp_Usart1_Init(int BaudRate)
{
     //GPIO�˿�����
     GPIO_InitTypeDef GPIO_InitStructure;
     USART_InitTypeDef USART_InitStructure;
//     NVIC_InitTypeDef NVIC_InitStructure;
    
     RCC_AHB1PeriphClockCmd(USART1_GPIO_CLK,ENABLE); //ʹ��GPIOAʱ��
     RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);//ʹ��USART1ʱ��
    
     //����1��Ӧ���Ÿ���ӳ��
     GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF); //GPIOA9����ΪUSART1
     GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF); //GPIOA10����ΪUSART1
     
     //USART1�˿�����
     GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN; //GPIOA9��GPIOA10
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //�ٶ�50MHz
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
     GPIO_Init(USART1_GPIO_PORT,&GPIO_InitStructure); //��ʼ��PA9��PA10
    
    //USART1 ��ʼ������
     USART_InitStructure.USART_BaudRate = BaudRate;//����������
     USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
     USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
     USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
     USART_Init(USART1, &USART_InitStructure); //��ʼ������1       
     USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1   
	 
    //Usart1 NVIC ����
//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       //�����ȼ�3
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
//    NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���    
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�    


	//ʹ��USART1 DMA 
	Usart1_DMA2_Config();	
}







