#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"
#include <stdio.h>

// LCD�Ļ�����ɫ�ͱ���ɫ
u16 POINT_COLOR = 0x0000; // ������ɫ
u16 BACK_COLOR = 0xeeee;  // ����ɫ

// ����LCD��Ҫ����
// Ĭ��Ϊ����
_lcd_dev lcddev;
int PressFlag = 0;

void LCD_Configuration(void)
{

	//	GPIO_InitTypeDef GPIO_InitStructure;
	//	/* Enable the GPIO_LED Clock */
	//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO , ENABLE);
	////	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag	,Enable SWD
	//
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_7 |GPIO_Pin_11|GPIO_Pin_12 ;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//	GPIO_SetBits(GPIOD, GPIO_Pin_12);  //reset pin
	//
	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//

	/*****************�Ĵ�����**********************/
	RCC->APB2ENR |= 7 << 4; // ʹ��PORTC,D,Eʱ��
	GPIOD->CRL &= 0X0F00FFFF;
	GPIOD->CRL |= 0X30330000;
	GPIOD->CRH &= 0XFFF00FFF;
	GPIOD->CRH |= 0X00033000; // PD4 5 7 11 12�������
	GPIOD->ODR |= 0X1000;	  // PD12�ø�

	GPIOC->CRL &= 0XFFFFFFF0;
	GPIOC->CRL |= 0X00000003; // PC0�������
}

void LCD_Config_DIN(void)
{
	/*GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0xC703;////GPIO_Pin_14 | GPIO_Pin_15 |GPIO_Pin_1|GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,0xC703);
	GPIO_InitStructure.GPIO_Pin = 0xff80 ; //PE7-15
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,0xff80);*/

	/*****************�Ĵ�����**********************/
	// GPIOD->CRL&=0XFFFFFF00;
	// GPIOD->CRL|=0X00000088;
	// GPIOD->CRH&=0X00FFF000;
	// GPIOD->CRH|=0X88000888;

	GPIOD->CRL = 0x34334488;
	GPIOD->CRH = 0x88433888;
	GPIOD->BRR = (uint16_t)0xC703;

	// GPIOE->CRL&=0X0FFFFFFF;
	// GPIOE->CRL|=0X80000000;
	// GPIOE->CRH&=0XFFFFFFFF;
	// GPIOE->CRH|=0X88888888;
	GPIOE->CRL = 0X84444444;
	GPIOE->CRH = 0X88888888;
	GPIOE->BRR = (uint16_t)0xff80;
}

void LCD_Config_DOUT(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0xC703; ////GPIO_Pin_14 | GPIO_Pin_15 |GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//
	GPIO_InitStructure.GPIO_Pin = 0xff80; // PE7-15
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	//
	/*GPIOD->CRL&=0XFFFFFF00;
	GPIOD->CRL|=0X00000033;
	GPIOD->CRH&=0X00FFF000;
	GPIOD->CRH|=0X33000333;

	GPIOE->CRL&=0XFFFFFFF0;
	GPIOE->CRL|=0X00000003;
	GPIOE->CRH&=0XFFFFFFFF;
	GPIOE->CRH|=0X33333333;*/

	// GPIOD->CRL=0x34334433;
	// GPIOD->CRH=0x33333333;
	// GPIOE->CRL=0X34444444;
	// GPIOE->CRH=0X33333333;
}

//
void DATAOUT(u16 x) // �������
{
	LCD_Config_DOUT();

	GPIOD->BRR = (0x03 << 14); // LCD_D0,D1; PD14,PD15
	GPIOD->BSRR = (x & 0x0003) << 14;

	GPIOD->BRR = (0x03 << 0); // LCD_D2,D3; PD0,PD1
	GPIOD->BSRR = (x & 0x000c) >> 2;

	GPIOE->BRR = (0x1ff << 7); // LCD_D4-12; PE7-15
	GPIOE->BSRR = (x & 0x1ff0) << 3;

	GPIOD->BRR = (0x07 << 8); // LCD_D13,D14,D15; PD8-10
	GPIOD->BSRR = (x & 0xE000) >> 5;
}

u16 DATAIN() // ��������
{
	u16 tmp = 0;
	LCD_Config_DIN();
	tmp = GPIOD->IDR >> 14;
	tmp |= ((GPIOD->IDR & 0x3) << 2);
	tmp |= ((GPIOE->IDR & 0xff80) >> 3);
	tmp |= ((GPIOD->IDR & 0x0700) << 5);
	return tmp;
}

// д�Ĵ�������
// data:�Ĵ���ֵ
void LCD_WR_REG(u16 data)
{
	LCD_RS_CLR; // д��ַ
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}
// д���ݺ���
// �������LCD_WR_DATAX��,��ʱ�任�ռ�.
// data:�Ĵ���ֵ
void LCD_WR_DATAX(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}
// ��LCD����
// ����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{
	u16 t;

	DATAIN();

	LCD_RS_SET;
	LCD_CS_CLR;
	// ��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;

	t = DATAIN();
	LCD_RD_SET;
	LCD_CS_SET;

	DATAOUT(0xFFFF);
	return t;
}
// д�Ĵ���
// LCD_Reg:�Ĵ������
// LCD_RegValue:Ҫд���ֵ
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}
// ���Ĵ���
// LCD_Reg:�Ĵ������
// ����ֵ:������ֵ
u16 LCD_ReadReg(u16 LCD_Reg)
{
	LCD_WR_REG(LCD_Reg); // д��Ҫ���ļĴ�����
	return LCD_RD_DATA();
}
// ��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}
// LCDдGRAM
// RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{
	LCD_WR_DATA(RGB_Code); // дʮ��λGRAM
}

// ��mdk -O1ʱ���Ż�ʱ��Ҫ����
// ��ʱi
void opt_delay(u8 i)
{
	while (i--)
		;
}
// ��ȡ��ĳ�����ɫֵ
// x,y:����
// ����ֵ:�˵����ɫ
u16 LCD_ReadPoint(u16 x, u16 y)
{
	u16 r, g, b;
	if (x >= lcddev.width || y >= lcddev.height)
		return 0; // �����˷�Χ,ֱ�ӷ���
	LCD_SetCursor(x, y);
	if (lcddev.id == 0X9341)
		LCD_WR_REG(0X2E); // 9341/6804/3510/1963 ���Ͷ�GRAMָ��
	DATAIN();			  //

	LCD_RS_SET;
	LCD_CS_CLR;
	// ��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)
	LCD_RD_CLR;
	opt_delay(2); // ��ʱ
	r = DATAIN(); // ʵ��������ɫ
	LCD_RD_SET;

	// dummy READ
	LCD_RD_CLR;
	opt_delay(2); // ��ʱ
	r = DATAIN(); // ʵ��������ɫ
	LCD_RD_SET;
	if (lcddev.id == 0X9341) // 9341/NT35310/NT35510Ҫ��2�ζ���
	{
		LCD_RD_CLR;
		opt_delay(2); // ��ʱ
		b = DATAIN(); // ��ȡ��ɫֵ
		LCD_RD_SET;
		g = r & 0XFF; // ����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
		g <<= 8;
	}
	LCD_CS_SET;
	DATAOUT(0xFFFF); // ȫ�������

	if (lcddev.id == 0X9341)
		return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); // ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
}
// LCD������ʾ
void LCD_DisplayOn(void)
{
	if (lcddev.id == 0X9341)
		LCD_WR_REG(0X29); // ������ʾ
}
// LCD�ر���ʾ
void LCD_DisplayOff(void)
{
	if (lcddev.id == 0X9341)
		LCD_WR_REG(0X28); // �ر���ʾ
}
// ���ù��λ��
// Xpos:������
// Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	if (lcddev.id == 0X9341)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos >> 8);
		LCD_WR_DATA(Xpos & 0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos >> 8);
		LCD_WR_DATA(Ypos & 0XFF);
	}
}
// ����LCD���Զ�ɨ�跽��
// ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
// ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
// dir:0~7,����8������(���嶨���lcd.h)
// 9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���
void LCD_Scan_Dir(u8 dir)
{
	u16 regval = 0;
	u16 dirreg = 0;
	u16 temp;

	if (lcddev.id == 0x9341) // 9341/6804/5310/5510/1963,���⴦��
	{
		switch (dir)
		{
		case L2R_U2D: // ������,���ϵ���
			regval |= (0 << 7) | (0 << 6) | (0 << 5);
			break;
		case L2R_D2U: // ������,���µ���
			regval |= (1 << 7) | (0 << 6) | (0 << 5);
			break;
		case R2L_U2D: // ���ҵ���,���ϵ���
			regval |= (0 << 7) | (1 << 6) | (0 << 5);
			break;
		case R2L_D2U: // ���ҵ���,���µ���
			regval |= (1 << 7) | (1 << 6) | (0 << 5);
			break;
		case U2D_L2R: // ���ϵ���,������
			regval |= (0 << 7) | (0 << 6) | (1 << 5);
			break;
		case U2D_R2L: // ���ϵ���,���ҵ���
			regval |= (0 << 7) | (1 << 6) | (1 << 5);
			break;
		case D2U_L2R: // ���µ���,������
			regval |= (1 << 7) | (0 << 6) | (1 << 5);
			break;
		case D2U_R2L: // ���µ���,���ҵ���
			regval |= (1 << 7) | (1 << 6) | (1 << 5);
			break;
		}
		dirreg = 0X36;
		LCD_WriteReg(dirreg, regval);
	}
}
// ����
// x,y:����
// POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x, u16 y)
{
	LCD_SetCursor(x, y);	// ���ù��λ��
	LCD_WriteRAM_Prepare(); // ��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR);
}
// ���ٻ���
// x,y:����
// color:��ɫ
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color) // ��ʹ������λ�á�дGRAM��������һ����ɫ������������LCD_DrawPointʱһ����
{
	if (lcddev.id == 0X9341)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(x >> 8);
		LCD_WR_DATA(x & 0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(y >> 8);
		LCD_WR_DATA(y & 0XFF);
	}
	LCD_RS_CLR;
	LCD_CS_CLR;
	DATAOUT(lcddev.wramcmd); // дָ��
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
	LCD_WR_DATA(color); // д����
}
// SSD1963 ��������
// pwm:����ȼ�,0~100.Խ��Խ��.
void LCD_SSD_BackLightSet(u8 pwm)
{
	LCD_WR_REG(0xBE);		 // ����PWM���
	LCD_WR_DATA(0x05);		 // 1����PWMƵ��
	LCD_WR_DATA(pwm * 2.55); // 2����PWMռ�ձ�
	LCD_WR_DATA(0x01);		 // 3����C
	LCD_WR_DATA(0xFF);		 // 4����D
	LCD_WR_DATA(0x00);		 // 5����E
	LCD_WR_DATA(0x00);		 // 6����F
}
// ����LCD��ʾ����
// dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
	if (dir == 0) // ����
	{
		lcddev.dir = 0; // ����
		lcddev.width = 240;
		lcddev.height = 320;
		if (lcddev.id == 0X9341)
		{
			lcddev.wramcmd = 0X2C;
			lcddev.setxcmd = 0X2A;
			lcddev.setycmd = 0X2B;
		}
	}
	else // ����
	{
		lcddev.dir = 1; // ����
		lcddev.width = 320;
		lcddev.height = 240;
		if (lcddev.id == 0X9341)
		{
			lcddev.wramcmd = 0X2C;
			lcddev.setxcmd = 0X2A;
			lcddev.setycmd = 0X2B;
		}
	}
	LCD_Scan_Dir(DFT_SCAN_DIR); // Ĭ��ɨ�跽��
}
// ���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
// sx,sy:������ʼ����(���Ͻ�)
// width,height:���ڿ��Ⱥ͸߶�,�������0!!
// �����С:width*height.
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
	u8 hsareg, heareg, vsareg, veareg;
	u16 hsaval, heaval, vsaval, veaval;
	u16 twidth, theight;
	twidth = sx + width - 1;
	theight = sy + height - 1;
	if (lcddev.id == 0X9341)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(sx >> 8);
		LCD_WR_DATA(sx & 0XFF);
		LCD_WR_DATA(twidth >> 8);
		LCD_WR_DATA(twidth & 0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(sy >> 8);
		LCD_WR_DATA(sy & 0XFF);
		LCD_WR_DATA(theight >> 8);
		LCD_WR_DATA(theight & 0XFF);
	}
}
// ��ʼ��lcd
// �ó�ʼ���������Գ�ʼ������ALIENTEK��Ʒ��LCDҺ����
// ������ռ�ýϴ�flash,�û����Ը����Լ���ʵ�����,ɾ��δ�õ���LCD��ʼ������.�Խ�ʡ�ռ�.
void LCD_Init(void)
{
	delay_ms(5); // delay 50 ms
	LCD_WriteReg(0x0000, 0x0001);
	delay_ms(5); // delay 50 ms
	lcddev.id = LCD_ReadReg(0x0000);

	// ����9341 ID�Ķ�ȡ
	LCD_WR_REG(0XD3);
	LCD_RD_DATA();			   // dummy read
	LCD_RD_DATA();			   // ����0X00
	lcddev.id = LCD_RD_DATA(); // ��ȡ93
	lcddev.id <<= 8;
	lcddev.id |= LCD_RD_DATA(); // ��ȡ41

	if (lcddev.id == 0X9341) // 9341��ʼ��
	{
		LCD_WR_REG(0xCF);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0xC1);
		LCD_WR_DATAX(0X30);
		LCD_WR_REG(0xED);
		LCD_WR_DATAX(0x64);
		LCD_WR_DATAX(0x03);
		LCD_WR_DATAX(0X12);
		LCD_WR_DATAX(0X81);
		LCD_WR_REG(0xE8);
		LCD_WR_DATAX(0x85);
		LCD_WR_DATAX(0x10);
		LCD_WR_DATAX(0x7A);
		LCD_WR_REG(0xCB);
		LCD_WR_DATAX(0x39);
		LCD_WR_DATAX(0x2C);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x34);
		LCD_WR_DATAX(0x02);
		LCD_WR_REG(0xF7);
		LCD_WR_DATAX(0x20);
		LCD_WR_REG(0xEA);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_REG(0xC0);	// Power control
		LCD_WR_DATAX(0x1B); // VRH[5:0]
		LCD_WR_REG(0xC1);	// Power control
		LCD_WR_DATAX(0x01); // SAP[2:0];BT[3:0]
		LCD_WR_REG(0xC5);	// VCM control
		LCD_WR_DATAX(0x30); // 3F
		LCD_WR_DATAX(0x30); // 3C
		LCD_WR_REG(0xC7);	// VCM control2
		LCD_WR_DATAX(0XB7);
		LCD_WR_REG(0x36); // Memory Access Control
		LCD_WR_DATAX(0x48);
		LCD_WR_REG(0x3A);
		LCD_WR_DATAX(0x55);
		LCD_WR_REG(0xB1);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x1A);
		LCD_WR_REG(0xB6); // Display Function Control
		LCD_WR_DATAX(0x0A);
		LCD_WR_DATAX(0xA2);
		LCD_WR_REG(0xF2); // 3Gamma Function Disable
		LCD_WR_DATAX(0x00);
		LCD_WR_REG(0x26); // Gamma curve selected
		LCD_WR_DATAX(0x01);
		LCD_WR_REG(0xE0); // Set Gamma
		LCD_WR_DATAX(0x0F);
		LCD_WR_DATAX(0x2A);
		LCD_WR_DATAX(0x28);
		LCD_WR_DATAX(0x08);
		LCD_WR_DATAX(0x0E);
		LCD_WR_DATAX(0x08);
		LCD_WR_DATAX(0x54);
		LCD_WR_DATAX(0XA9);
		LCD_WR_DATAX(0x43);
		LCD_WR_DATAX(0x0A);
		LCD_WR_DATAX(0x0F);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_REG(0XE1); // Set Gamma
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x15);
		LCD_WR_DATAX(0x17);
		LCD_WR_DATAX(0x07);
		LCD_WR_DATAX(0x11);
		LCD_WR_DATAX(0x06);
		LCD_WR_DATAX(0x2B);
		LCD_WR_DATAX(0x56);
		LCD_WR_DATAX(0x3C);
		LCD_WR_DATAX(0x05);
		LCD_WR_DATAX(0x10);
		LCD_WR_DATAX(0x0F);
		LCD_WR_DATAX(0x3F);
		LCD_WR_DATAX(0x3F);
		LCD_WR_DATAX(0x0F);
		LCD_WR_REG(0x2B);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x01);
		LCD_WR_DATAX(0x3f);
		LCD_WR_REG(0x2A);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0xef);
		LCD_WR_REG(0x11); // Exit Sleep
		delay_ms(10);
		LCD_WR_REG(0x29); // display on
	}

	LCD_Display_Dir(0); // Ĭ��Ϊ����
	LCD_LED = 1;		// ��������
	LCD_Clear(WHITE);
}

// ��������
// color:Ҫ���������ɫ
void LCD_Clear(u16 color)
{
	u32 index = 0;
	u32 totalpoint = lcddev.width;
	totalpoint *= lcddev.height; // �õ��ܵ���
	LCD_SetCursor(0x00, 0x0000); // ���ù��λ��
	LCD_WriteRAM_Prepare();		 // ��ʼд��GRAM
	for (index = 0; index < totalpoint; index++)
		LCD_WR_DATA(color);
}
// ��ָ�����������ָ����ɫ
// �����С:(xend-xsta+1)*(yend-ysta+1)
// xsta
// color:Ҫ������ɫ
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
	u16 i, j;
	u16 xlen = 0;
	u16 temp;
	{
		xlen = ex - sx + 1;
		for (i = sy; i <= ey; i++)
		{
			LCD_SetCursor(sx, i);	// ���ù��λ��
			LCD_WriteRAM_Prepare(); // ��ʼд��GRAM
			for (j = 0; j < xlen; j++)
				LCD_WR_DATA(color); // ���ù��λ��
		}
	}
}
// ��ָ�����������ָ����ɫ��
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
// color:Ҫ������ɫ
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
	u16 height, width;
	u16 i, j;
	width = ex - sx + 1;  // �õ����Ŀ���
	height = ey - sy + 1; // �߶�
	for (i = 0; i < height; i++)
	{
		LCD_SetCursor(sx, sy + i); // ���ù��λ��
		LCD_WriteRAM_Prepare();	   // ��ʼд��GRAM
		for (j = 0; j < width; j++)
			LCD_WR_DATA(color[i * width + j]); // д������
	}
}
// ����
// x1,y1:�������
// x2,y2:�յ�����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // ������������
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // ���õ�������
	else if (delta_x == 0)
		incx = 0; // ��ֱ��
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // ˮƽ��
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // ѡȡ��������������
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // �������
	{
		LCD_DrawPoint(uRow, uCol); // ����
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
// ������
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1, y1, x2, y1);
	LCD_DrawLine(x1, y1, x1, y2);
	LCD_DrawLine(x1, y2, x2, y2);
	LCD_DrawLine(x2, y1, x2, y2);
}
// ��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
// r    :�뾶
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - (r << 1); // �ж��¸���λ�õı�־
	while (a <= b)
	{
		LCD_DrawPoint(x0 + a, y0 - b); // 5
		LCD_DrawPoint(x0 + b, y0 - a); // 0
		LCD_DrawPoint(x0 + b, y0 + a); // 4
		LCD_DrawPoint(x0 + a, y0 + b); // 6
		LCD_DrawPoint(x0 - a, y0 + b); // 1
		LCD_DrawPoint(x0 - b, y0 + a);
		LCD_DrawPoint(x0 - a, y0 - b); // 2
		LCD_DrawPoint(x0 - b, y0 - a); // 7
		a++;
		// ʹ��Bresenham�㷨��Բ
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
}
// ��ָ��λ����ʾһ���ַ�
// x,y:��ʼ����
// num:Ҫ��ʾ���ַ�:" "--->"~"
// size:�����С 12/16/24
// mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
	u8 temp, t1, t;
	u16 y0 = y;
	u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // �õ�����һ���ַ���Ӧ������ռ���ֽ���
	num = num - ' ';										   // �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for (t = 0; t < csize; t++)
	{
		if (size == 12)
			temp = asc2_1206[num][t]; // ����1206����
		else if (size == 16)
			temp = asc2_1608[num][t]; // ����1608����
		else if (size == 24)
			temp = asc2_2412[num][t]; // ����2412����
		else
			return; // û�е��ֿ�
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)
				LCD_Fast_DrawPoint(x, y, POINT_COLOR);
			else if (mode == 0)
				LCD_Fast_DrawPoint(x, y, BACK_COLOR);
			temp <<= 1;
			y++;
			if (y >= lcddev.height)
				return; // ��������
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				if (x >= lcddev.width)
					return; // ��������
				break;
			}
		}
	}
}
// m^n����
// ����ֵ:m^n�η�.
u32 LCD_Pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}
// ��ʾ����,��λΪ0,����ʾ
// x,y :�������
// len :���ֵ�λ��
// size:�����С
// color:��ɫ
// num:��ֵ(0~4294967295);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
	}
}
// ��ʾ����,��λΪ0,������ʾ
// x,y:�������
// num:��ֵ(0~999999999);
// len:����(��Ҫ��ʾ��λ��)
// size:�����С
// mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				if (mode & 0X80)
					LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);
				else
					LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
	}
}
// ��ʾ�ַ���
// x,y:�������
// width,height:�����С
// size:�����С
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
	u8 x0 = x;
	width += x;
	height += y;
	while ((*p <= '~') && (*p >= ' ')) // �ж��ǲ��ǷǷ��ַ�!
	{
		if (x >= width)
		{
			x = x0;
			y += size;
		}
		if (y >= height)
			break; // �˳�
		LCD_ShowChar(x, y, *p, size, 0);
		x += size / 2;
		p++;
	}
}

const unsigned char CN_DataBuf[][16] = {
	/*--  ��  --*/
	/*--  ??12;  ??????????:?x?=16x16   --*/
	0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0xFF, 0xFE, 0x01, 0x00, 0x01, 0x00,
	0x02, 0x80, 0x02, 0x80, 0x04, 0x40, 0x04, 0x40, 0x08, 0x20, 0x10, 0x10, 0x20, 0x08, 0xC0, 0x06,

	/*--  С  --*/
	/*--  ??12;  ??????????:?x?=16x16   --*/
	0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x10, 0x11, 0x08, 0x11, 0x04,
	0x21, 0x04, 0x21, 0x02, 0x41, 0x02, 0x81, 0x02, 0x01, 0x00, 0x01, 0x00, 0x05, 0x00, 0x02, 0x00};

u8 SPI_SndRecv(u8 data);

#define TOUCH_READ_TIMES 4 // ��������
u16 x_addata[TOUCH_READ_TIMES], z_addata[TOUCH_READ_TIMES], y_addata[TOUCH_READ_TIMES];
u32 Temp, r, sx, sy = 0;
int xScreenAD, yScreenAD;
int xScreen, yScreen;
u16 PointX_ADmin = 0;
u16 PointY_ADmin = 0;
u16 PointX_ADmax = 0;
u16 PointY_ADmax = 0;

#define QPINTERVAL 48
#define QZSIZE 20
#define QPSIZEX 240 // �ֱ���
#define QPSIZEY 320
#define QPSTARTX 0
#define QPSTARTY 0

#define T_WR_SET GPIOC->BSRR = 1 << 3
#define T_SCK_SET GPIOC->BSRR = 1 << 0

#define T_WR_CLR GPIOC->BRR = 1 << 3
#define T_SCK_CLR GPIOC->BRR = 1 << 0

#define T_IN_STATUE ((GPIOC->IDR & (1 << 2)) ? 1 : 0)

static void touch_delay_us(u32 us)
{
	volatile u32 i;
	while (us--)
	{
		for (i = 0; i < 12; i++)
		{
		}
	}
}

static void touch_delay_ms(u32 ms)
{
	while (ms--)
	{
		touch_delay_us(1000);
	}
}

typedef struct
{
	GPIO_TypeDef *cs_gpio;
	uint16_t cs_pin;
	GPIO_TypeDef *clk_gpio;
	uint16_t clk_pin;
	GPIO_TypeDef *mosi_gpio;
	uint16_t mosi_pin;
	GPIO_TypeDef *miso_gpio;
	uint16_t miso_pin;
	GPIO_TypeDef *pen_gpio;
	uint16_t pen_pin;
	uint8_t pen_active_low;
} TouchWiring;

static GPIO_TypeDef *g_touch_cs_gpio = GPIOA;
static uint16_t g_touch_cs_pin = GPIO_Pin_15;
static GPIO_TypeDef *g_touch_clk_gpio = GPIOA;
static uint16_t g_touch_clk_pin = GPIO_Pin_5;
static GPIO_TypeDef *g_touch_mosi_gpio = GPIOB;
static uint16_t g_touch_mosi_pin = GPIO_Pin_5;
static GPIO_TypeDef *g_touch_mosi2_gpio = GPIOA;
static uint16_t g_touch_mosi2_pin = GPIO_Pin_7;
static uint8_t g_touch_use_mosi2 = 1;
static GPIO_TypeDef *g_touch_miso_gpio = GPIOA;
static uint16_t g_touch_miso_pin = GPIO_Pin_6;
static GPIO_TypeDef *g_touch_pen_gpio = GPIOC;
static uint16_t g_touch_pen_pin = GPIO_Pin_2;
static uint8_t g_touch_pen_active_low = 1;

// CS 操作宏
#define TP_GPIO_SET(gpio, pin) ((gpio)->BSRR = (pin))
#define TP_GPIO_CLR(gpio, pin) ((gpio)->BRR = (pin))
#define TP_CS_LOW() TP_GPIO_CLR(g_touch_cs_gpio, g_touch_cs_pin)
#define TP_CS_HIGH() TP_GPIO_SET(g_touch_cs_gpio, g_touch_cs_pin)
#define TP_CLK_HIGH() TP_GPIO_SET(g_touch_clk_gpio, g_touch_clk_pin)
#define TP_CLK_LOW() TP_GPIO_CLR(g_touch_clk_gpio, g_touch_clk_pin)
#define TP_MOSI_HIGH()                                          \
	do                                                          \
	{                                                           \
		TP_GPIO_SET(g_touch_mosi_gpio, g_touch_mosi_pin);       \
		if (g_touch_use_mosi2)                                  \
			TP_GPIO_SET(g_touch_mosi2_gpio, g_touch_mosi2_pin); \
	} while (0)
#define TP_MOSI_LOW()                                           \
	do                                                          \
	{                                                           \
		TP_GPIO_CLR(g_touch_mosi_gpio, g_touch_mosi_pin);       \
		if (g_touch_use_mosi2)                                  \
			TP_GPIO_CLR(g_touch_mosi2_gpio, g_touch_mosi2_pin); \
	} while (0)
#define TP_MISO_READ() ((g_touch_miso_gpio->IDR & g_touch_miso_pin) ? 1 : 0)

static uint32_t touch_gpio_rcc(GPIO_TypeDef *gpio)
{
	if (gpio == GPIOA)
		return RCC_APB2Periph_GPIOA;
	if (gpio == GPIOB)
		return RCC_APB2Periph_GPIOB;
	if (gpio == GPIOC)
		return RCC_APB2Periph_GPIOC;
	if (gpio == GPIOD)
		return RCC_APB2Periph_GPIOD;
	if (gpio == GPIOE)
		return RCC_APB2Periph_GPIOE;
	return 0;
}

static uint8_t touch_gpio_port_source(GPIO_TypeDef *gpio)
{
	if (gpio == GPIOA)
		return GPIO_PortSourceGPIOA;
	if (gpio == GPIOB)
		return GPIO_PortSourceGPIOB;
	if (gpio == GPIOC)
		return GPIO_PortSourceGPIOC;
	if (gpio == GPIOD)
		return GPIO_PortSourceGPIOD;
	if (gpio == GPIOE)
		return GPIO_PortSourceGPIOE;
	return 0;
}

static uint8_t touch_pin_source(uint16_t pin)
{
	uint8_t i;
	for (i = 0; i < 16; i++)
	{
		if (pin == (uint16_t)(1u << i))
			return i;
	}
	return 0;
}

static uint32_t touch_exti_line(uint16_t pin)
{
	uint8_t s = touch_pin_source(pin);
	return (uint32_t)(1u << s);
}

static IRQn_Type touch_exti_irq(uint16_t pin)
{
	uint8_t s = touch_pin_source(pin);
	if (s <= 4)
		return (IRQn_Type)(EXTI0_IRQn + s);
	if (s <= 9)
		return EXTI9_5_IRQn;
	return EXTI15_10_IRQn;
}

static void TOUCH_SCREEN_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Explicitly disable SPI1 to prevent hardware conflict with bit-banging on PA5/PA6/PA7
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);

	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_AFIO |
			touch_gpio_rcc(g_touch_cs_gpio) |
			touch_gpio_rcc(g_touch_clk_gpio) |
			touch_gpio_rcc(g_touch_mosi_gpio) |
			(g_touch_use_mosi2 ? touch_gpio_rcc(g_touch_mosi2_gpio) : 0) |
			touch_gpio_rcc(g_touch_miso_gpio) |
			touch_gpio_rcc(g_touch_pen_gpio),
		ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = g_touch_cs_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(g_touch_cs_gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = g_touch_pen_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_touch_pen_gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = g_touch_clk_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(g_touch_clk_gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = g_touch_mosi_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(g_touch_mosi_gpio, &GPIO_InitStructure);

	if (g_touch_use_mosi2)
	{
		GPIO_InitStructure.GPIO_Pin = g_touch_mosi2_pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(g_touch_mosi2_gpio, &GPIO_InitStructure);
	}

	GPIO_InitStructure.GPIO_Pin = g_touch_miso_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_touch_miso_gpio, &GPIO_InitStructure);

	TP_CLK_LOW();
	TP_MOSI_LOW();
	TP_CS_HIGH();
}

int Touch_PenDown(void)
{
	uint8_t v = (g_touch_pen_gpio->IDR & g_touch_pen_pin) ? 1 : 0;
	return g_touch_pen_active_low ? (v == 0) : (v != 0);
}

static void TP_Write_Byte(u8 num);

// Helper to init a pin as IPU
static void TP_Init_IPU(GPIO_TypeDef *GPIOx, uint16_t Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void Touch_Scan_MISO(uint8_t *act_pa6, uint8_t *act_pb4, uint8_t *act_pc2, uint8_t *act_pc1)
{
	u8 i;
	u8 val_pa6, val_pb4, val_pc2, val_pc1;
	u8 last_pa6 = 0, last_pb4 = 0, last_pc2 = 0, last_pc1 = 0;

	*act_pa6 = 0;
	*act_pb4 = 0;
	*act_pc2 = 0;
	*act_pc1 = 0;

	// Enable clocks for PB and PC if not already (PA is enabled)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	// Init Candidates
	TP_Init_IPU(GPIOA, GPIO_Pin_6); // PA6
	TP_Init_IPU(GPIOB, GPIO_Pin_4); // PB4
	TP_Init_IPU(GPIOC, GPIO_Pin_2); // PC2
	TP_Init_IPU(GPIOC, GPIO_Pin_1); // PC1

	TP_CS_LOW();
	TP_Write_Byte(0x90); // Read Y

	// Skip busy/null
	touch_delay_us(10);

	for (i = 0; i < 64; i++) // Read more bits to catch any activity
	{
		TP_CLK_LOW();
		touch_delay_us(5);
		TP_CLK_HIGH();
		touch_delay_us(5);

		val_pa6 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
		val_pb4 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
		val_pc2 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
		val_pc1 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);

		if (i > 0)
		{
			if (val_pa6 != last_pa6)
				*act_pa6 = 1;
			if (val_pb4 != last_pb4)
				*act_pb4 = 1;
			if (val_pc2 != last_pc2)
				*act_pc2 = 1;
			if (val_pc1 != last_pc1)
				*act_pc1 = 1;
		}

		last_pa6 = val_pa6;
		last_pb4 = val_pb4;
		last_pc2 = val_pc2;
		last_pc1 = val_pc1;
	}

	TP_CS_HIGH();
}

void Touch_ReadDiag(uint8_t *miso_ipu, uint8_t *miso_ipd, uint8_t *miso_flt, uint8_t *miso_act,
					uint8_t *cs_odr, uint8_t *cs_idr_low, uint8_t *cs_idr_high,
					uint8_t *clk_idr_low, uint8_t *clk_idr_high)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t last, act = 0;
	int i;

	if (cs_odr)
		*cs_odr = (g_touch_cs_gpio->ODR & g_touch_cs_pin) ? 1 : 0;

	// Start Diagnostic Session
	TP_CS_LOW();
	touch_delay_us(5);

	// Send a dummy command to wake up the XPT2046 MISO driver
	// 0x90 = Read Y, 12-bit, Differential
	TP_Write_Byte(0x90);
	touch_delay_us(5);

	// Now check MISO impedance
	// If XPT2046 is working and CS is low, MISO should be strongly driven (0 or 1)
	// It should NOT follow the internal pull-up/pull-down easily.

	GPIO_InitStructure.GPIO_Pin = g_touch_miso_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// Check with Pull-Up
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_touch_miso_gpio, &GPIO_InitStructure);
	touch_delay_us(10);
	if (miso_ipu)
		*miso_ipu = TP_MISO_READ();

	// Check with Pull-Down
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(g_touch_miso_gpio, &GPIO_InitStructure);
	touch_delay_us(10);
	if (miso_ipd)
		*miso_ipd = TP_MISO_READ();

	// Check with Floating
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(g_touch_miso_gpio, &GPIO_InitStructure);
	touch_delay_us(10);
	if (miso_flt)
		*miso_flt = TP_MISO_READ();

	// Restore to IPU for normal operation
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_touch_miso_gpio, &GPIO_InitStructure);

	// Check CS and CLK Pins (Self-Test of MCU Output)
	if (cs_idr_low)
		*cs_idr_low = (g_touch_cs_gpio->IDR & g_touch_cs_pin) ? 1 : 0;

	TP_CLK_LOW();
	touch_delay_us(2);
	if (clk_idr_low)
		*clk_idr_low = (g_touch_clk_gpio->IDR & g_touch_clk_pin) ? 1 : 0;

	TP_CLK_HIGH();
	touch_delay_us(2);
	if (clk_idr_high)
		*clk_idr_high = (g_touch_clk_gpio->IDR & g_touch_clk_pin) ? 1 : 0;

	// Check MISO Activity (Toggling)
	TP_CLK_LOW();
	touch_delay_us(2);
	last = TP_MISO_READ();
	for (i = 0; i < 32; i++)
	{
		TP_CLK_HIGH();
		touch_delay_us(2);
		TP_CLK_LOW();
		touch_delay_us(2);
		{
			uint8_t cur = TP_MISO_READ();
			if (cur != last)
				act++;
			last = cur;
		}
	}

	TP_CS_HIGH();
	touch_delay_us(2);
	if (cs_idr_high)
		*cs_idr_high = (g_touch_cs_gpio->IDR & g_touch_cs_pin) ? 1 : 0;
	if (miso_act)
		*miso_act = act;
}

// 模拟 SPI 写字节
static void TP_Write_Byte(u8 num)
{
	u8 count = 0;
	for (count = 0; count < 8; count++)
	{
		if (num & 0x80)
			TP_MOSI_HIGH();
		else
			TP_MOSI_LOW();
		num <<= 1;
		TP_CLK_LOW();
		touch_delay_us(5);
		TP_CLK_HIGH();
		touch_delay_us(5);
	}
	// 最后一个时钟周期结束后，CLK 应回到低电平
	TP_CLK_LOW();
	touch_delay_us(5);
}

// 模拟 SPI 读数据 (12位)
// XPT2046 时序：CMD(8) -> BUSY(1, Low) -> DATA(12) -> TAIL(3)
static u16 TP_Read_AD(u8 CMD)
{
	u8 count = 0;
	u16 Num = 0;

	TP_CLK_LOW();
	TP_MOSI_LOW();
	TP_CS_LOW();

	TP_Write_Byte(CMD);

	touch_delay_us(6); // 等待转换和 BUSY

	// Clock 1: Ignore Null Bit
	TP_CLK_LOW();
	touch_delay_us(5);
	TP_CLK_HIGH();
	touch_delay_us(5);

	// Read 12 Data Bits (D11-D0)
	for (count = 0; count < 12; count++)
	{
		Num <<= 1;
		TP_CLK_LOW();
		touch_delay_us(5);
		TP_CLK_HIGH(); // Sample on rising edge
		touch_delay_us(5);
		if (TP_MISO_READ())
			Num++;
	}

	TP_CS_HIGH();
	return (Num);
}

static u16 touch_read_axis_raw(u8 cmd)
{
	return TP_Read_AD(cmd);
}

static u16 touch_raw_to_value(u16 raw)
{
	return raw & 0x0FFF;
}

// spiд
u8 SPI_SndRecv(u8 data)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
		;
	SPI_I2S_SendData(SPI2, data);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
		;
	return (u8)SPI_I2S_ReceiveData(SPI2);
}

u32 SPI_X(void)
{
	u16 i;
	u16 j;
	u16 cnt = 0;
	for (i = 0; i < TOUCH_READ_TIMES; i++)
	{
		x_addata[i] = 0;
	}
	for (i = 0; i < (TOUCH_READ_TIMES * 2); i++)
	{
		u16 raw = touch_read_axis_raw(0xD0);
		x_addata[cnt++] = touch_raw_to_value(raw);
		if (cnt >= TOUCH_READ_TIMES)
			break;
	}
	if (cnt < 2)
		return 0;
	for (i = 0; i < cnt; i++)
	{
		for (j = (u16)(i + 1); j < cnt; j++)
		{
			if (x_addata[j] < x_addata[i])
			{
				u16 t = x_addata[i];
				x_addata[i] = x_addata[j];
				x_addata[j] = t;
			}
		}
	}
	if (cnt & 1)
		return x_addata[cnt / 2];
	Temp = (x_addata[cnt / 2 - 1] + x_addata[cnt / 2]) >> 1;
	return Temp;
}

u32 SPI_Y(void)
{
	u16 i;
	u16 j;
	u16 cnt = 0;
	for (i = 0; i < TOUCH_READ_TIMES; i++)
	{
		y_addata[i] = 0;
	}
	for (i = 0; i < (TOUCH_READ_TIMES * 2); i++)
	{
		u16 raw = touch_read_axis_raw(0x90);
		y_addata[cnt++] = touch_raw_to_value(raw);
		if (cnt >= TOUCH_READ_TIMES)
			break;
	}
	if (cnt < 2)
		return 0;
	for (i = 0; i < cnt; i++)
	{
		for (j = (u16)(i + 1); j < cnt; j++)
		{
			if (y_addata[j] < y_addata[i])
			{
				u16 t = y_addata[i];
				y_addata[i] = y_addata[j];
				y_addata[j] = t;
			}
		}
	}
	if (cnt & 1)
		return y_addata[cnt / 2];
	Temp = (y_addata[cnt / 2 - 1] + y_addata[cnt / 2]) >> 1;
	return Temp;
}

static void TOUCH_INT_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable GPIOB, GPIOC and AFIO clock */
	RCC_APB2PeriphClockCmd(touch_gpio_rcc(g_touch_pen_gpio), ENABLE);
	/* LEDs pins configuration */
	GPIO_InitStructure.GPIO_Pin = g_touch_pen_pin;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_touch_pen_gpio, &GPIO_InitStructure);
}

static void TOUCH_INT_EXIT_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	uint32_t line = touch_exti_line(g_touch_pen_pin);
	uint8_t port_src = touch_gpio_port_source(g_touch_pen_gpio);
	uint8_t pin_src = touch_pin_source(g_touch_pen_pin);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(port_src, pin_src);
	EXTI_InitStructure.EXTI_Line = line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(line);
	EXTI_Init(&EXTI_InitStructure);
}
static void TOUCH_InterruptConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = touch_exti_irq(g_touch_pen_pin);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// ���ݲɼ���������ֵ��Ѱ������������
static void getPos(u16 xScreen, u16 yScreen)
{
	u16 x = xScreen, y = yScreen;
	int i, j;
	int dist = 0;
	for (i = QPSTARTX + QPINTERVAL / 2, dist = 9999; i <= QPSTARTX + QPSIZEX; i += QPINTERVAL)
	{
		if (abs(i - x) <= dist)
		{
			xScreen = i;
			dist = abs(i - x);
		}
	}
	for (j = QPSTARTY + QPINTERVAL / 2, dist = 9999; j <= QPSTARTY + QPSIZEY; j += QPINTERVAL)
	{
		if (abs(j - y) <= dist)
		{
			yScreen = j;
			dist = abs(j - y);
		}
	}
	//		play(xScreen/QPINTERVAL, yScreen/QPINTERVAL);
}

void TouchScreen()
{
	//	static u16 sDataX,sDataY;

	xScreenAD = SPI_X();
	yScreenAD = SPI_Y();
	if (xScreenAD < 50 || yScreenAD < 50 || xScreenAD > 4050 || yScreenAD > 4050)
	{
	} // ��Ч����
	else
	{
		PressFlag++;
		if (PointX_ADmax > PointX_ADmin && PointY_ADmax > PointY_ADmin) // �Ѿ�����У׼��
		{
			xScreen = (float)(xScreenAD - PointX_ADmin) * 239 / (PointX_ADmax - PointX_ADmin);
			yScreen = (float)(yScreenAD - PointY_ADmin) * 319 / (PointY_ADmax - PointY_ADmin);
		}
	}
	//	if((xScreen>1)&&(yScreen>1)&&(xScreen<240-1)&&(yScreen<320-1)) {
	//		if(!(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)) ) {
	//			if((xScreen < QPSIZEX/QPINTERVAL*QPINTERVAL-1)&&(yScreen <= QPSIZEY/QPINTERVAL*QPINTERVAL-1)) getPos(xScreen,yScreen);
	//		}

	//  }
}

void EXTI2_IRQHandler(void)
{
	uint32_t line = touch_exti_line(g_touch_pen_pin);
	if (EXTI_GetITStatus(line) != RESET)
	{
		touch_delay_ms(20);
		TouchScreen();
		EXTI_ClearITPendingBit(line);
	}
}

void Touch_Init()
{
	TOUCH_SCREEN_INIT();
	TOUCH_INT_config();
	// TOUCH_INT_EXIT_Init(); // Disable EXTI to prevent freeze
	// TOUCH_InterruptConfig();

	//	while(1);
}

// ��ʾ�ɼ����ĵ�λ����
char showstr[32];

void Touch_Check()
{
	int status = 0;
	LCD_ShowString(45, 45, 245, 325, 12, "1");

	LCD_ShowString(205, 285, 245, 325, 12, "2");

	LCD_DrawLine(0, 40, 240, 40);
	LCD_DrawLine(0, 280, 240, 280); // �������x=(40 , 200),y=(40,280)
	LCD_ShowString(40 - 2, 40 - 5, 240, 12, 12, "+");
	LCD_DrawLine(40, 0, 40, 320);
	LCD_DrawLine(200, 0, 200, 320);

	while (status == 0)
	{
		LCD_ShowString(45, 0, 240, 12, 12, "Press No.1 Point");
		if (PressFlag > 0) // �Ѿ���������
		{
			PointX_ADmin = xScreenAD;
			PointY_ADmin = yScreenAD;
			sprintf(showstr, "TouchAD, x:%4d y%4d   ", xScreenAD, yScreenAD);
			LCD_ShowString(45, 12, 240, 12, 12, showstr);
			status = 1;
			PressFlag = 0;
		}
	}
	while (status == 1)
	{
		LCD_ShowString(45, 0, 240, 12, 12, "Press No.2 Point");
		if (PressFlag > 0) // �Ѿ���������
		{
			PointX_ADmax = xScreenAD;
			PointY_ADmax = yScreenAD;
			sprintf(showstr, "TouchAD, x:%4d y%4d   ", xScreenAD, yScreenAD);
			LCD_ShowString(45, 24, 240, 12, 12, showstr);
			status = 2;
			PressFlag = 0;
			status = 3;
		}
	}
}

void showTouch()
{

	sprintf(showstr, "Press at:%4d y%4d   ", xScreen, yScreen);
	LCD_ShowString(45, 0, 240, 12, 12, showstr);
	LCD_ShowString(xScreen - 2, yScreen - 5, 240, 12, 12, "+"); //+���ĵ�������ƫ��Ϊ2��5
}
