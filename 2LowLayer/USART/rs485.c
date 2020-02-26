#include "rs485.h"	 
#include "delay.h"
#include "bsp_time.h"


//���ջ����� 	
static uint8_t RS485_RX_BUF[RS485_MAX_LEN];  	//���ջ���,���255���ֽ�.
//���յ������ݳ���
static uint8_t RS485_RX_CNT=0;  

//static uint8_t RS485_RecvOne (uint8_t *Str);
//volatile uint16_t RecvEnd2 = 0;
//volatile uint16_t RecvTop2 = 0;


void USART2_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 	
	  res =USART_ReceiveData(USART2);//;��ȡ���յ�������USART2->DR
	  
		if(RS485_RX_CNT < RS485_MAX_LEN)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
			RS485_RX_CNT++;						//������������1 
		} 
	}  											 
} 

//void USART2_IRQHandler(void)
//{
//	uint8_t res;	    
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
//	{	 	
//	  res =USART_ReceiveData(USART2);//;��ȡ���յ�������USART2->DR
//	  
//      if ( (RecvEnd2 == (RecvTop2 - 1) ) || ( (RecvTop2 == 0) && (RecvEnd2 == (RS485_MAX_LEN - 1) ) ) )
//      {
//          //�������
//          RecvTop2 = 0;
//          RecvEnd2 = 0;
//      }
//      else
//      {
//          RS485_RX_BUF[RecvEnd2] = res;
//          RecvEnd2++;
//      
//          if (RecvEnd2 >= RS485_MAX_LEN) RecvEnd2 = 0;

//          dbg("top = %d, end = %d \r\n",RecvTop2,RecvEnd2);
//          dbh("USART2_IRQHandler", RS485_RX_BUF, RecvEnd2-1);
//      }

//	}  											 
//} 



								 
//��ʼ��IO ����2
//bound:������	  
void RS485_Init(uint32_t bound)
{  	 
	
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(USART2_GPIO_CLK|RCC_RS485_TXEN,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(USART2_CLK,ENABLE);//ʹ��USART2ʱ��
	
  //����2���Ÿ���ӳ��
	GPIO_PinAFConfig(USART2_GPIO_PORT,USART2_TX_SOURCE,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(USART2_GPIO_PORT,USART2_RX_SOURCE,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2    
    GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN | USART2_TX_PIN; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(USART2_GPIO_PORT,&GPIO_InitStructure); //��ʼ��PA2��PA3
	
	//PG8���������485ģʽ����  
    GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN; //GPIOE2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(PORT_RS485_TXEN,&GPIO_InitStructure); //��ʼ��PE2
	

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
    USART_Cmd(USART2, ENABLE);  //ʹ�ܴ��� 2
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	RS485_TX_EN=0;				//Ĭ��Ϊ����ģʽ	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(uint8_t *buf,uint16_t len)
{
	uint8_t t;
	RS485_TX_EN=1;			//����Ϊ����ģʽ
	
  	for(t=0;t<len;t++)		//ѭ����������
	{
	    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //�ȴ����ͽ���		
        USART_SendData(USART2,buf[t]); //��������
	}	
    
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //�ȴ����ͽ���		
	
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}


//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(uint8_t *buf,uint16_t *len)
{
	uint8_t rxlen=RS485_RX_CNT;
	uint8_t i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���    
    
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
	}
}

//static uint8_t RS485_RecvOne (uint8_t *Str)
//{
//	if (RecvTop2 == RS485_RX_CNT) return 0;//read nothing

//	*Str = RS485_RX_BUF[RecvTop2];
//	RecvTop2++;

//	if (RecvTop2 >= RS485_MAX_LEN) RecvTop2 = 0;

//	return 1;//read one


//}


//uint16_t RS485_Receive_Data (uint8_t *buf,uint16_t len,uint32_t timeout_MilliSeconds)
//{
//	u16 RecvLen = 0;
//	u8 tmp[1] = {0};

//	if (len == 0) return 0;

//	g1msTimerUART2 = timeout_MilliSeconds;

//	while (1)
//	{
//		if (g1msTimerUART2 == 0) return RecvLen;

//		if (RS485_RecvOne (tmp) == 1) 
//		{
//			buf[RecvLen++] = tmp[0];
//		}

//		if (RecvLen >= len) return RecvLen;
//	}
//	
//}






