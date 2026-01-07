#include "game.h"
#include <math.h>

// 更新状态栏
void game_update_status_bar(void)
{
    char buf[64];
    int black, white;
    game_count_pieces(g_board, &black, &white);

    POINT_COLOR = BLACK;
    BACK_COLOR = WHITE;
    LCD_Fill(0, 0, 240, 35, WHITE);

    sprintf(buf, "MODE:%s  TURN:%s", g_mode == MODE_PVE ? "PVE" : "PVP", g_turn == CELL_BLACK ? "BLACK" : "WHITE");
    LCD_ShowString(6, 0, 240, 8, 16, (u8 *)buf);
    //  sprintf(buf, "B:%d W:%d AD:%4d,%4d", black, white, xScreenAD, yScreenAD);
    //  LCD_ShowString(6, 18, 240, 16, 16, (u8 *)buf);
}

// 初始化游戏视图
// 初始化游戏视图
void display_init_game_view(void)
{
    LCD_Clear(WHITE);
    POINT_COLOR = BLACK;
    BACK_COLOR = WHITE;

    display_draw_board_lines();
    display_draw_board_content();
    game_update_status_bar();

    display_draw_button(10, 285, 115, 315, g_state == STATE_PLAYING ? "RESTART" : "START", 1);
    // display_draw_button(125, 285, 230, 315, g_mode == MODE_PVE ? "MODE:PVE" : "MODE:PVP", 1);
    //  显示当前模式的退出按钮（返回主菜单）
    display_draw_button(125, 285, 230, 315, "BACK", 1);
}

// 绘制棋盘线
void display_draw_board_lines(void)
{
    int i;
    int bx = game_board_x0();
    int by = game_board_y0();
    int cs = game_cell_size();
    int size = cs * (BOARD_N - 1);

    POINT_COLOR = BLACK;
    BACK_COLOR = WHITE;
    LCD_Fill(bx, by, bx + size - 1, by + size - 1, LGRAY);

    for (i = 0; i < BOARD_N; i++)
    {
        int x = bx + i * cs;
        int y = by + i * cs;
        LCD_DrawLine(x, by, x, by + size);
        LCD_DrawLine(bx, y, bx + size, y);
    }
}
// 在指定位置绘制棋子（圆形，在交叉点）
void display_draw_piece_at(int r, int c, u8 piece)
{
    int cs = game_cell_size();
    int x_center = game_board_x0() + c * cs;
    int y_center = game_board_y0() + r * cs;
    int radius = cs / 3;

    // 先清除该位置的棋子和高亮（用棋盘背景色填充）

    LCD_Fill(x_center - radius, y_center - radius, x_center + radius, y_center + radius, LGRAY);

    // 重新绘制交叉点（黑色小点）
    POINT_COLOR = BLACK;
    LCD_DrawLine(x_center - radius, y_center, x_center + radius, y_center);
    LCD_DrawLine(x_center, y_center - radius, x_center, y_center + radius);

    if (piece == CELL_BLACK)
    {
        // POINT_COLOR = BLACK;
        // 绘制黑色圆形棋子
        // LCD_Draw_Circle(x_center, y_center, radius);
        // 填充圆形
        for (int i = -radius; i <= radius; i++)
        {
            int width = (int)sqrt(radius * radius - i * i);
            LCD_Fill(x_center - width, y_center + i, x_center + width, y_center + i, BLACK);
        }
        POINT_COLOR = WHITE;
        LCD_Draw_Circle(x_center, y_center, radius);
    }
    else if (piece == CELL_WHITE)
    {
        // POINT_COLOR = WHITE;
        //  绘制白色圆形棋子
        // LCD_Draw_Circle(x_center, y_center, radius);
        //  填充圆形
        for (int i = -radius; i <= radius; i++)
        {
            int width = (int)sqrt(radius * radius - i * i);
            LCD_Fill(x_center - width, y_center + i, x_center + width, y_center + i, WHITE);
        }
        POINT_COLOR = BLACK;
        LCD_Draw_Circle(x_center, y_center, radius);
    }
}

// 绘制棋盘内容
void display_draw_board_content(void)
{
    int r, c;
    for (r = 0; r < BOARD_N; r++)
    {
        for (c = 0; c < BOARD_N; c++)
        {
            display_draw_piece_at(r, c, g_board[r][c]);
        }
    }
}

// 绘制按钮
void display_draw_button(int x1, int y1, int x2, int y2, const char *label, int active)
{
    u16 fill = active ? YELLOW : GRAY;
    u16 text = active ? BLACK : WHITE;
    int w = x2 - x1 + 1;
    int h = y2 - y1 + 1;
    int tx = x1 + 6;
    int ty = y1 + (h - 16) / 2;
    POINT_COLOR = BLACK;
    LCD_Fill(x1, y1, x2, y2, fill);
    LCD_DrawRectangle(x1, y1, x2, y2);
    POINT_COLOR = text;
    LCD_ShowString(tx, ty, w, h, 16, (u8 *)label);
}

// 高亮单元格
void display_highlight_cell(int r, int c)
{
    int x, y;
    int cs = game_cell_size();
    int radius = cs / 2;

    game_cross_xy(r, c, &x, &y);

    // 先清除该位置的高亮（用棋盘背景色填充）
    POINT_COLOR = LGRAY;
    BACK_COLOR = LGRAY;
    LCD_Fill(x - radius, y - radius, x + radius, y + radius, LGRAY);

    // 重新绘制交叉点
    POINT_COLOR = BLACK;
    LCD_DrawPoint(x, y);

    // 绘制红色高亮圆圈
    POINT_COLOR = RED;
    LCD_Draw_Circle(x, y, radius);
}

// 设置游戏结束信息
void display_set_game_over(const char *msg)
{
    g_state = STATE_GAME_OVER;
    POINT_COLOR = RED;
    LCD_Fill(0, 0, 239, 35, WHITE);
    LCD_ShowString(6, 8, 240, 20, 16, (u8 *)msg);
}
void display_clear_highlight(int r, int c)
{
    int x, y;
    int cs = game_cell_size();
    int radius = cs / 2;

    game_cross_xy(r, c, &x, &y);

    // 恢复棋盘背景
    LCD_Fill(x - radius, y - radius, x + radius, y + radius, LGRAY);

    // 重画棋盘线
    POINT_COLOR = BLACK;
    LCD_DrawLine(x - radius, y, x + radius, y);
    LCD_DrawLine(x, y - radius, x, y + radius);

    // 重画该点的棋子（如果有）
    display_draw_piece_at(r, c, g_board[r][c]);
}
