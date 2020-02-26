#include "def.h"


//�������ȼ�  
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)

//�����ջ��С	

#define LED_STK_SIZE 		256
#define UPDATE_STK_SIZE 	1024*2

//������
static TaskHandle_t xHandleTaskUpdate = NULL;

//������
static void vTaskUpdate(void *pvParameters);





int main(void)
{ 

	bsp_Init();
	
    

	//������ʼ����
    xTaskCreate((TaskFunction_t )vTaskUpdate,            //������
                (const char*    )"update",               //��������
                (uint16_t       )UPDATE_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )UPDATE_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t*  )&xHandleTaskUpdate);   //������        
                
    vTaskStartScheduler();          //�����������
}
 


static void vTaskUpdate(void *pvParameters)
{
    uint32_t file_total_size = 0;
    char *spi_flash_value;
    char *mcu_flash_value;
    char *file_size;

    int8_t spi_flash_flag = 0;
    int8_t mcu_flash_flag = 0;
    
    while(1)
    {   

        //��ӡ�²�����DEBUGʱʹ�ã�RELEASE����ɾ��
        //ef_print_env();

        //��ȡ������־λ
        spi_flash_value = ef_get_env("WSPIFLASH");
        spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flash_value);

        mcu_flash_value = ef_get_env("WMCUFLASH"); 
        mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value);       
        
        //������ת
        if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
        {
            //����Ҫ����������APP
			if(IAP_JumpToApplication())
			{
                printf("iap jump error,please download app\r\n");

                //��תʧ�ܣ�����Ҫ�������б�־λ������дSPI FLASH��MCU FLASH
                ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);  

                //add 2019.09.05���Է��֣���Ҫ�������������ɹ�
                NVIC_SystemReset();
            }            
        }
        else
        {
            //�ж�SPI FLASH�Ƿ���д�꣬MCU FLASHδд�꣬��FLASH��дMCU FLASH
            if(spi_flash_flag == 0) //�����Ѵ���flash�ڲ�������δд��mcu
            {
                printf("IAP STATR! ---> Write MCU FLASH\r\n");
                //��ȡ�ļ���С
                file_size = ef_get_env((const char * )"FileSize");
                file_total_size = str2int((const char *)file_size);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //д��MCU FLASH ��ɱ�־λ
                    if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                    {
                        printf("STM_FLASH_Write success\r\n");
                    } 
                }
            }
            else //SPI FLASH�޳����ļ����������س����ļ���д�뵽MCU FLASH��
            {     
                printf("IAP STATR! ---> Write SPI FLASH\r\n");
                
                //��Ҫ����������IAP��������
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    printf("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //д��MCU FLASH ��ɱ�־λ
                        if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                        {
                            printf("STM_FLASH_Write success\r\n");
                        }  
                    }
                    else
                    {
                        ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                        
                    }
                }
            }
        }        

        vTaskDelay(100);
    }
}





