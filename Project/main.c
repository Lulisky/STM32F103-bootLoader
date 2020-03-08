/**************************************************************************
 * @�ļ�  : main.c
 * @����  : caochao
 * @����  : 10-1-2018
 * @ժҪ  : ���C�ļ�ʱʵ����ϵͳ�ĳ�ʼ����ucosII������
 *
 * �޸��� :
 *  ����  :
 *
 * �޸��� :
 *  ����  :
 **************************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include "ff.h"
#include "rcc.h"
#include "usart.h"
#include "sdio_sd.h"

#define IAP_ADDR_START    0x8000000  //bootloder��ʼ��ַ
#define IAP_FLASH_SIZE    0x0010000  //bootloderflash��С
#define APP_ADDR_START    (IAP_ADDR_START+IAP_FLASH_SIZE) //�û�������ʼ��ַ

/*----------------�ļ�ϵͳ��ر���------------------------------------*/
FIL  fp;
FATFS  fs;
FRESULT Res;
/*------------------------------------------------------------------*/

/*------------------------FLASHҳ��С�Ķ��� -------------------------*/
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

/*------------------���ݻ����С------------------------------------*/
uint16_t dataBuf0[FLASH_PAGE_SIZE/2];  //д������ݻ���
uint16_t dataBuf1[FLASH_PAGE_SIZE/2];  //��ȡ�����ݻ���

/***********************************************************************
*   ������: NVIC_DeInit
*   ��  ��: �ָ�NVICΪ��λ״̬.ʹ�жϲ��ٷ���.
*   ��  ��:   
*           �� 
*   ��  ��:   
*           ��
***********************************************************************/
void NVIC_DeInit(void) 
{ 
    uint32_t index = 0; 
    
    NVIC->ICER[0] = 0xFFFFFFFF; 
    NVIC->ICER[1] = 0x000007FF; 
    NVIC->ICPR[0] = 0xFFFFFFFF; 
    NVIC->ICPR[1] = 0x000007FF; 
    
    for(index = 0; index < 0x0B; index++) 
    { 
        NVIC->IP[index] = 0x00000000; 
    }  
}

/***********************************************************************
*   ������: Jumpto_APP
*   ��  ��: IAP��ת��APP����
*   ��  ��:   
*           �� 
*   ��  ��:   
*           ��
***********************************************************************/
void Jumpto_APP(void)
{
    uint32_t IapSpInitVal;
    uint32_t IapJumpAddr;
    void (*pIapFun)(void);

    RCC_DeInit();//�ر�����
    NVIC_DeInit(); //�ָ�NVICΪ��λ״̬.ʹ�жϲ��ٷ���

    __set_PRIMASK(1); //IAP���ж� APP���û��UCOSϵͳ��APP��ʼ����Ҫ���ж� ��UCOS�����������Ὺ�ж�
    NVIC_SetVectorTable(NVIC_VectTab_FLASH,IAP_FLASH_SIZE);
    
    IapSpInitVal = *(uint32_t *)APP_ADDR_START;
    IapJumpAddr =  *(uint32_t *)(APP_ADDR_START + 4);

    if((IapSpInitVal & 0x2FFE0000) != 0x20000000)//���ջ����ַ�Ƿ�Ϸ�.
    {
        /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ ��������ѭ��
         *������ָʾ�ƻ����Ǿ���
         *
         *
         *
         */
    }
    
    if ((IapJumpAddr& 0xFFF8000) != 0x8000000)
    {
         /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ ��������ѭ��
         *������ָʾ�ƻ����Ǿ���
         *
         *
         *
         */
    }

    __set_MSP (IapSpInitVal);
    pIapFun = (void (*)(void))IapJumpAddr; //������ת����.
    (*pIapFun)();  //��ת��ָ��λ�����г���
}

///***********************************************************************
//*   ������: JumpToIAP
//*   ��  ��: APP��ת��IAP����
//*   ��  ��:   
//*           �� 
//*   ��  ��:   
//*           ��
//***********************************************************************/
//void JumpToIAP(void)
//{
//    uint32_t IapSpInitVal;
//    uint32_t IapJumpAddr;
//    void (*pIapFun)(void);

////    RCC_DeInit(); //�ر�����
////    NVIC_DeInit(); //�ָ�NVICΪ��λ״̬.ʹ�жϲ��ٷ���
//    
////    __set_PRIMASK(1); //���ж�
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x00);

//    // APP����תǰ���жϣ���ת��IAP��IAP��ʼ����Ҫ���ж�
//    IapSpInitVal = *(uint32_t *)IAP_ADDR_START;
//    IapJumpAddr =  *(uint32_t *)(IAP_ADDR_START + 4);

//    if((IapSpInitVal & 0x2FFE0000) != 0x20000000)//���ջ����ַ�Ƿ�Ϸ�.
//    {
//        /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ  ��������ѭ��
//         *������ָʾ�ƻ����Ǿ���
//         *
//         *
//         *
//         */
//    }
//    
//    if ((IapJumpAddr& 0xFFF8000) != 0x8000000)
//    {
//         /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ ��������ѭ��
//         *������ָʾ�ƻ����Ǿ��� 
//         *
//         *
//         *
//         */
//    }
// 
//    __set_CONTROL(0);

//    //�����û����߳�ģʽ �������жϺ�ſ��Իص���Ȩ���߳�ģʽ
//    //APP��ʹ��ϵͳ��ucos����Ҫ�д˹��̷�������IAP���޷��ٴ�����APP
//    __set_MSP (IapSpInitVal);

//    pIapFun = (void (*)(void))IapJumpAddr;
//    (*pIapFun) ();
//}

/***********************************************************************
*   ������: System_LoadUpdateFile
*   ��  ��: ����̼�����
*   ��  ��:   
*           addr��SD���е�·�� 
*   ��  ��:   
*           ��
***********************************************************************/
void System_LoadUpdateFile(const char* addr)
{
    uint16_t i = 0;
    uint16_t page = 0;
    uint32_t readNum;   
    uint32_t tempAddr;
    FLASH_Status  status;  
    
    Res = f_mount (&fs,"0:",1);
	if (Res != FR_OK)  
	{
        printf("�ļ�ϵͳ������,����ʧ�ܣ�");
		Jumpto_APP(); //��ת��ԭ���ĳ���ִ��APP
	}
    else
    {
        printf("SD�� ���ڳɹ��� \r\n");
    }

    Res = f_open (&fp,addr, FA_READ);	
    if (Res != FR_OK)
    {
        printf("�ļ��̼�������,����ʧ�ܣ�");
        Jumpto_APP(); //��ת��ԭ���ĳ���ִ��APP
    }
    else
    {
        printf("���ļ��ɹ��� \r\n");
    }
    
    while (1)
    {
        tempAddr = APP_ADDR_START+page*FLASH_PAGE_SIZE; //����Ҫ���µ�flashҳ��
        
        Res = f_read(&fp,(uint8_t *)dataBuf0,FLASH_PAGE_SIZE,&readNum);	
        if (Res == FR_OK)
        {
            FLASH_Unlock();  //���STM32��FALSHд����
            
            status = FLASH_ErasePage(tempAddr);
            if ((status != FLASH_COMPLETE) && (page == 0))
            {
                FLASH_Lock(); //ʹ��STM32��FALSHд����
                
                printf("flash����ʧ��,����ʧ�ܣ�");
                Jumpto_APP(); //��ת��ԭ���ĳ���ִ��APP  
                
                break;  //�����ȷ��ת��ԭ���ĳ���ִ ���᷵����
            }
            else if ((status != FLASH_COMPLETE) && (page != 0))
            {
                while(1)
                {
                   /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ ��������ѭ��
                    *������ָʾ�ƻ����Ǿ��� 
                    *��ʱ�����Ѿ��������ش���
                    */
                    
                    printf("�̼������������ش���\r\n");
                }
            }
            
            for (i = 0; i < readNum/2; i++)
            {
                status = FLASH_ProgramHalfWord(tempAddr+2*i,dataBuf0[i]);  //���STM32��FALSHд����
                if (status != FLASH_COMPLETE)
                {
                    while(1)
                    {
                        /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ ��������ѭ��
                        *������ָʾ�ƻ����Ǿ��� 
                        *��ʱ�����Ѿ��������ش���
                        */
                        
                        printf("�̼������������ش���\r\n");
                    }
                } 
            }

            FLASH_Lock(); //ʹ��STM32��FALSHд����
        }
        else
        { 
            if (page == 0)
            {
                printf("�ļ���ȡʧ��,����ʧ�ܣ�");
                Jumpto_APP(); //��ת��ԭ���ĳ���ִ��APP
                
                break; //�����ȷ��ת��ԭ���ĳ���ִ ���᷵����
            }
            else
            { 
                while(1)
                {
                   /*����Լ���bootloder����ʧ�ܵĸ澯��Ϣ ��������ѭ��
                    *������ָʾ�ƻ����Ǿ��� 
                    *��ʱ�����Ѿ��������ش���
                    */
                    
                    printf("�̼������������ش���\r\n");
                }
            }
        }
        
        page++;  //����flashҳ��
    }
    
    printf("�̼������ɹ���\r\n");
    Jumpto_APP(); //��ת��ԭ���ĳ���ִ��APP
}

/**************************************************************************
* ������:  main
* ��  ��:  �����������
* ��  ��:   
*         �� 
* ��  ��:   
*         ��
**************************************************************************/
int main(void)
{
    RCC_Configuration();         //��ʼ��ϵͳʱ��
    USART1_Configuration();      //��ʼ��usart1 Ϊ�˴�ӡ����״̬

    System_LoadUpdateFile("0:/System/Template.bin");     //���ع̼�	

	while(1)
	{
         ;
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    /* Infinite loop */
    while (1)
    {
		printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    }
}
#endif

/**
  * @}
  */

/***********************************�ļ�����***************************************/


