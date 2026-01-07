//DMA.c
#include "dma.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_dma.h"


void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//????????
	SysTick->VAL = 0x00;					  //???????
	SysTick->CTRL = 0x00000005;			//??????HCLK,?????
	while(!(SysTick->CTRL & 0x00010000));	//?????0
	SysTick->CTRL = 0x00000004;			//?????
}


void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 

void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 



uint16_t MyDMA_Size;                    // ���������

void MyDMA_Init(uint32_t *AddrA, uint32_t *AddrB, uint16_t Size)
{
	DMA_InitTypeDef DMA_InitStructure;                      // ��ʼ���ṹ��

	MyDMA_Size = Size;                    
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // ����DMA1����ʱ��
																													
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)AddrA;       // ����վ�����ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;  // ÿ��ת�˵����ݿ��ȣ��ֽڴ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;          // ת�����һ��֮���ַ����
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AddrB;           // Ŀ��վ�����ַ��������������ԭ����ǰ��һ��
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;      // ����վ����ΪԴ���ݵ�ַ ���䷽��Ϊ A -- B
	DMA_InitStructure.DMA_BufferSize = Size;                // ���䵥Ԫ�ĸ����������ʹ���
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;           // ����ģʽ�������Ǵ������飬ѡ���ѭ��ģʽ��ֻ����һ�֣����Զ���װ��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;             // �������������ڴ��е����飬ѡ��洢�����洢��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;   // ָ��ͨ�������ȼ���һ��ͨ�����ѡ
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);            // DMA1��ͨ��1
	
	DMA_Cmd(DMA1_Channel1, DISABLE);                        // ��ʼ��������ʧ��DMA
}

void MyDMA_Transfer(uint32_t size)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);                        // ת��֮ǰ�ȹر�DMA
	DMA1_Channel1->CNDTR = size; //ע������ֲ������������һ���ݼ���������ָʾʣ��Ҫ��������ݸ���
//	DMA_SetCurrDataCounter(DMA1_Channel1, MyDMA_Size);      // ���ô����������С��V3.5���ϵİ汾�����������3.0����
	DMA_Cmd(DMA1_Channel1, ENABLE);                         // ����DMA����ʼת�� 
	
	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);      // �ȴ�ת����� DMA1_FLAG_TC1 DMA1��ͨ��1ת����ɱ�־λ
	DMA_ClearFlag(DMA1_FLAG_TC1);                           // �����־λ
}



