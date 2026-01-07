/**
 ******************************************************************************
 * @file InputCaptureMode/main.c
 * @author   MCD Application Team
 * @version  V3.0.0
 * @date     04/27/2009
 * @brief    Main program body
 ******************************************************************************
 * @copy
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
 */

/** @addtogroup InputCaptureMode
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
extern u16 FrameFlag;
extern int PressFlag;
extern int xScreen;
extern int yScreen;
extern int PressFlag;
extern int xScreenAD;
extern int yScreenAD;
extern u16 PointX_ADmin;
extern u16 PointY_ADmin;
extern u16 PointX_ADmax;
extern u16 PointY_ADmax;
void TouchScreen(void);
void RS232_Configuration(void);
void NVIC_Configuration(void);
void systick_init(void);

int main(void)
{
	FrameFlag = 0;
	SystemInit(); // ��ʼ��ϵͳʱ��Ϊ72M
	RS232_Configuration();
	NVIC_Configuration();
	systick_init();
	Touch_Init();
	LCD_Configuration();
	LCD_Init();
	game_init();
	// 触摸屏校准
//	Touch_Check();
	if (PointX_ADmax <= PointX_ADmin || PointY_ADmax <= PointY_ADmin)
	{
		PointX_ADmin = 200;
		PointX_ADmax = 3800;
		PointY_ADmin = 200;
		PointY_ADmax = 3800;
	}

	g_ui = UI_START;
	menu_draw_start_screen(); // 初始界面就是 开始菜单界面，显示"TOUCH TEST"和"START GAME"按钮。

	while (1)
	{
		static int was_down = 0;
		static u8 diag_div = 0;
		int down;
		int pen;

		pen = Touch_PenDown(); // 检测是否有触摸

		if (pen)
		{
			TouchScreen(); // 读取触摸坐标
		}

		if (g_ui == UI_START)
		{
			if (++diag_div >= 20)
			{
				diag_div = 0;
				menu_draw_start_touch_status();
			}
		}
		else
		{
			diag_div = 0;
		}

		down = pen && (xScreenAD >= 50 && yScreenAD >= 50 && xScreenAD <= 4050 && yScreenAD <= 4050);
		if (down && !was_down)
		{
			PressFlag = 0;
			if (g_ui == UI_START)
			{
				if (game_touch_in_rect(xScreen, yScreen, 40, 28, 200, 68))
				{
					g_ui = UI_GAME;			  // 切换到游戏界面
					display_init_game_view(); // 切换到游戏界面
				}
			}
			else
			{
				menu_handle_touch(); // 处理游戏中的触摸事件
			}
		}
		if (!down && was_down)
		{
			PressFlag = 0;
		}
		was_down = down;
		delay_ms(10);
	}
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
