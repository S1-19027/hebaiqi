#include "sys.h"
#include "core_cm3.h"
extern volatile u32 g_ms_tick;
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK  STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//static u8  fac_us=0;							//us��ʱ������			
//void delay_us(u32 nus)
//{		
//		int i,cnt ;
//		cnt = nus<<1  ;
//		for ( i = 0 ; i < cnt ; i++ );
//	
//}
////��ʱnms
////ע��nms�ķ�Χ
////SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
////nms<=0xffffff*8*1000/SYSCLK
////SYSCLK��λΪHz,nms��λΪms
////��72M������,nms<=1864 
//void delay_ms(u16 nms)
//{	 		  	  
//	int i ;
//	for( i = 0 ; i < nms; i ++ )
//		delay_us(1000) ;
//} 

static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������
//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init(u8 SYSCLK)
{
//	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 start = g_ms_tick;
	while ((u32)(g_ms_tick - start) < (u32)nms) {
	}
}   

void delay_s(u16 ns) //����delay_ms�����1864��������ʱ���Ļ����ñ�����
{
   u16 i=0;
   for(i=0;i<ns;i++)
    delay_ms(1000);

}
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 i;
	RCC_ClocksTypeDef clocks;
	u32 cycles;
	RCC_GetClocksFreq(&clocks);
	cycles = (clocks.HCLK_Frequency / 1000000u) * nus;
	cycles = cycles / 6u + 1u;
	for (i = 0; i < cycles; i++) {
		__ASM volatile("nop");
	}
}

