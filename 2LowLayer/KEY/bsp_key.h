/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : key.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��25��
  ����޸�   :
  ��������   : ��������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��25��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __BSP_KEY_H
#define __BSP_KEY_H	 

#include "sys.h" 
#include "delay.h"


/* �����ڶ�Ӧ��RCCʱ�� */
#define RCC_ALL_KEY     (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC)

#define GPIO_PORT_KEY       GPIOA

#define GPIO_PIN_KEY_SET    GPIO_Pin_4
#define GPIO_PIN_KEY_RR     GPIO_Pin_0
#define GPIO_PIN_KEY_LL     GPIO_Pin_1

#define GPIO_PORT_KEY_OK    GPIOC
#define GPIO_PIN_KEY_OK     GPIO_Pin_3


#define KEY_SET     PAin(4)   	//PA4
#define KEY_RR 		PAin(0)		//PA0 
#define KEY_LL 		PAin(1)		//PA1
#define KEY_OK		PCin(3)		//PC3 

//#define KEY_SET 	GPIO_ReadInputDataBit(GPIO_PORT_KEY,GPIO_PIN_KEY_SET)   //PA4
//#define KEY_RR 		GPIO_ReadInputDataBit(GPIO_PORT_KEY,GPIO_PIN_KEY_RR)	//PA0  
//#define KEY_LL 		GPIO_ReadInputDataBit(GPIO_PORT_KEY,GPIO_PIN_KEY_LL)    //PA1
//#define KEY_OK 	    GPIO_ReadInputDataBit(GPIO_PORT_KEY_OK,GPIO_PIN_KEY_OK)	//PC3 


#define KEY_NONE        0   //�ް���
#define KEY_SET_PRES 	1	//KEY0����
#define KEY_RR_PRES	    2	//KEY1����
#define KEY_LL_PRES	    3	//KEY2����
#define KEY_OK_PRES     4	//KEY3����


void bsp_key_Init(void);	//IO��ʼ��
u8 bsp_key_Scan(u8);  		//����ɨ�躯��		


#endif

