#ifndef __GAME_H
#define __GAME_H

#include "main.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>

// 游戏常量定义
#define BOARD_N 5

// 单元格状态
#define CELL_EMPTY 0
#define CELL_BLACK 1
#define CELL_WHITE 2

// 游戏模式
#define MODE_PVP 0
#define MODE_PVE 1

// 游戏状态
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2

// 界面状态
#define UI_START 0
#define UI_GAME 1

// 全局变量声明
extern u8 g_board[BOARD_N][BOARD_N];
extern u8 g_turn;
extern u8 g_mode;
extern u8 g_state;
extern u8 g_ui;
extern int g_sel_r;
extern int g_sel_c;
extern u32 g_rng;

// 触摸屏相关变量
extern int xScreen;
extern int yScreen;
extern int PressFlag;
extern int xScreenAD;
extern int yScreenAD;
extern u16 PointX_ADmin;
extern u16 PointY_ADmin;
extern u16 PointX_ADmax;
extern u16 PointY_ADmax;

// 游戏核心函数声明
void game_init(void);
void game_reset_board(void);
int game_try_move(int sr, int sc, int tr, int tc);
void game_apply_captures(u8 board[BOARD_N][BOARD_N], u8 player);
void game_check_game_end(void);
void game_update_status_bar(void);
int game_has_any_move(u8 board[BOARD_N][BOARD_N], u8 player);
void game_count_pieces(u8 board[BOARD_N][BOARD_N], int *black, int *white);

// 菜单界面函数声明
void menu_draw_start_screen(void);
void menu_draw_start_touch_status(void);
void menu_handle_touch(void);
void menu_start_game(void);

// 显示函数声明
void display_init_game_view(void);
void display_draw_board_lines(void);
void display_draw_piece_at(int r, int c, u8 piece);
void display_draw_board_content(void);
void display_draw_button(int x1, int y1, int x2, int y2, const char *label, int active);
void display_highlight_cell(int r, int c);
void display_set_game_over(const char *msg);

// AI函数声明
void ai_turn(void);

// 工具函数声明
int game_in_bounds(int r, int c);
int game_abs_i(int v);
u32 game_rng_next(void);
int game_touch_in_rect(int x, int y, int x1, int y1, int x2, int y2);
int game_touch_to_cell(int x, int y, int *out_r, int *out_c);
int game_cell_size(void);
int game_board_x0(void);
int game_board_y0(void);

#endif