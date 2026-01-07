#include "game.h"

// 触摸屏相关变量定义
extern int xScreen;
extern int yScreen;
extern int PressFlag;
extern int xScreenAD;
extern int yScreenAD;
extern u16 PointX_ADmin;
extern u16 PointY_ADmin;
extern u16 PointX_ADmax;
extern u16 PointY_ADmax;

// 外部函数声明
extern void Touch_ReadDiag(uint8_t *miso_ipu, uint8_t *miso_ipd, uint8_t *miso_flt, uint8_t *miso_act,
                           uint8_t *cs_odr, uint8_t *cs_idr_low, uint8_t *cs_idr_high,
                           uint8_t *clk_idr_low, uint8_t *clk_idr_high);
extern void Touch_Scan_MISO(uint8_t *act_pa6, uint8_t *act_pb4, uint8_t *act_pc2, uint8_t *act_pc1);
extern int Touch_PenDown(void);

// 绘制开始屏幕
void menu_draw_start_screen(void)
{
    LCD_Clear(WHITE);
    POINT_COLOR = BLACK;
    BACK_COLOR = WHITE;
    // LCD_ShowString(60, 6, 240, 20, 16, (u8 *)"TOUCH TEST");
    display_draw_button(40, 28, 200, 68, "START GAME", 1);
    LCD_ShowString(20, 78, 240, 12, 12, (u8 *)"Tap START GAME to enter game screen");
    menu_draw_start_touch_status();
}

// 绘制开始屏幕触摸状态
void menu_draw_start_touch_status(void)
{
    char buf[64];
    int pen = Touch_PenDown();
    u8 miu = 0, mid = 0, mif = 0, act = 0;
    u8 cs_odr = 0, cs_idr_low = 0, cs_idr_high = 0;
    u8 clk_idr_low = 0, clk_idr_high = 0;
    u8 a6 = 0, b4 = 0, c2 = 0, c1 = 0;

    Touch_ReadDiag(&miu, &mid, &mif, &act, &cs_odr, &cs_idr_low, &cs_idr_high, &clk_idr_low, &clk_idr_high);
    Touch_Scan_MISO(&a6, &b4, &c2, &c1);

    LCD_Fill(0, 96, 239, 96 + 16 * 4 - 1, WHITE); // 增加高度显示4行
    POINT_COLOR = BLUE;
    BACK_COLOR = WHITE;

    // sprintf(buf, "PEN:%d AD:%4d,%4d", pen, xScreenAD, yScreenAD);
    // LCD_ShowString(6, 96, 240, 16, 16, (u8 *)buf);

    // sprintf(buf, "M:%d%d%d CS:%d%d%d CK:%d%d", miu, mid, mif, cs_odr,cs_idr_low, cs_idr_high, clk_idr_low, clk_idr_high);
    // LCD_ShowString(6, 112, 240, 16, 16, (u8 *)buf);

    // sprintf(buf, "SCAN A6:%d B4:%d C2:%d C1:%d", a6, b4, c2, c1);
    // LCD_ShowString(6, 128, 240, 16, 16, (u8 *)buf);
}

// 开始游戏
void menu_start_game(void)
{
    game_reset_board();
    game_check_game_end();
    display_init_game_view();
}

// 处理触摸事件
void menu_handle_touch(void)
{
    int r, c;
    int cell_r, cell_c;

    if (game_touch_in_rect(xScreen, yScreen, 10, 285, 115, 315))
    {
        menu_start_game();
        return;
    }

    if (game_touch_in_rect(xScreen, yScreen, 125, 285, 230, 315))
    {
        g_mode = (g_mode == MODE_PVE) ? MODE_PVP : MODE_PVE;
        display_draw_button(125, 285, 230, 315, g_mode == MODE_PVE ? "MODE:PVE" : "MODE:PVP", 1);
        game_update_status_bar();
        if (g_state == STATE_PLAYING && g_mode == MODE_PVE && g_turn == CELL_WHITE)
        {
            ai_turn();
        }
        return;
    }

    if (g_state != STATE_PLAYING)
        return;

    if (!game_touch_to_cell(xScreen, yScreen, &cell_r, &cell_c))
        return;

    if (g_mode == MODE_PVE && g_turn == CELL_WHITE)
        return;

    if (g_sel_r < 0 || g_sel_c < 0)
    {
        if (g_board[cell_r][cell_c] == g_turn)
        {
            g_sel_r = cell_r;
            g_sel_c = cell_c;
            display_highlight_cell(g_sel_r, g_sel_c);
        }
        return;
    }

    r = g_sel_r;
    c = g_sel_c;

    if (cell_r == r && cell_c == c)
    {
        display_draw_piece_at(g_sel_r, g_sel_c, g_board[g_sel_r][g_sel_c]);
        g_sel_r = -1;
        g_sel_c = -1;
        return;
    }

    if (g_board[cell_r][cell_c] == g_turn)
    {
        display_draw_piece_at(g_sel_r, g_sel_c, g_board[g_sel_r][g_sel_c]);
        g_sel_r = cell_r;
        g_sel_c = cell_c;
        display_highlight_cell(g_sel_r, g_sel_c);
        return;
    }

    if (game_try_move(r, c, cell_r, cell_c))
    {
        g_sel_r = -1;
        g_sel_c = -1;
        if (g_state == STATE_PLAYING && g_mode == MODE_PVE && g_turn == CELL_WHITE)
        {
            delay_ms(500);
            ai_turn();
        }
        return;
    }
}