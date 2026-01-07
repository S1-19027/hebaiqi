#include "game.h"

// 全局变量定义
u8 g_board[BOARD_N][BOARD_N];
u8 g_turn;
u8 g_mode;
u8 g_state;
u8 g_ui;
int g_sel_r;
int g_sel_c;
u32 g_rng;

// 工具函数
int game_abs_i(int v) { return v < 0 ? -v : v; }

int game_in_bounds(int r, int c) { return r >= 0 && r < BOARD_N && c >= 0 && c < BOARD_N; }

u32 game_rng_next(void) { g_rng = g_rng * 1664525u + 1013904223u; return g_rng; }

int game_touch_in_rect(int x, int y, int x1, int y1, int x2, int y2)
{
	if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
	if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }
	return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

int game_touch_to_cell(int x, int y, int *out_r, int *out_c)
{
	int bx = game_board_x0();
	int by = game_board_y0();
	int cs = game_cell_size();
	int size = cs * BOARD_N;
	if (!game_touch_in_rect(x, y, bx, by, bx + size - 1, by + size - 1)) return 0;
	*out_c = (x - bx) / cs;
	*out_r = (y - by) / cs;
	return game_in_bounds(*out_r, *out_c);
}

int game_cell_size(void) { return 240 / BOARD_N; }
int game_board_x0(void) { return 0; }
int game_board_y0(void) { return 40; }

// 游戏初始化
void game_init(void)
{
	g_rng = 0x12345678u;
	g_mode = MODE_PVP;
	g_state = STATE_MENU;
	g_sel_r = -1;
	g_sel_c = -1;
}

// 重置棋盘
void game_reset_board(void)
{
	int r, c;
	for (r = 0; r < BOARD_N; r++) for (c = 0; c < BOARD_N; c++) g_board[r][c] = CELL_EMPTY;

	for (c = 0; c < BOARD_N; c++) g_board[0][c] = CELL_BLACK;
	for (c = 0; c < BOARD_N; c++) g_board[BOARD_N - 1][c] = CELL_WHITE;

	g_turn = CELL_BLACK;
	g_state = STATE_PLAYING;
	g_sel_r = -1;
	g_sel_c = -1;
}

// 统计行/列占用数
static int row_occupied_count(u8 board[BOARD_N][BOARD_N], int r)
{
	int c, n = 0;
	for (c = 0; c < BOARD_N; c++) if (board[r][c] != CELL_EMPTY) n++;
	return n;
}

static int col_occupied_count(u8 board[BOARD_N][BOARD_N], int c)
{
	int r, n = 0;
	for (r = 0; r < BOARD_N; r++) if (board[r][c] != CELL_EMPTY) n++;
	return n;
}

// 应用吃子规则
void game_apply_captures(u8 board[BOARD_N][BOARD_N], u8 player)
{
	u8 enemy = (player == CELL_BLACK) ? CELL_WHITE : CELL_BLACK;
	int r, c;
	int is_live = (board == g_board);

	// 检查行
	for (r = 0; r < BOARD_N; r++) {
		if (row_occupied_count(board, r) >= 4) continue;
		for (c = 0; c <= BOARD_N - 3; c++) {
			// 检查2对1: P P E -> 移除E
			if (board[r][c] == player && board[r][c + 1] == player && board[r][c + 2] == enemy) {
				board[r][c + 2] = CELL_EMPTY;
				if (is_live) display_draw_piece_at(r, c + 2, CELL_EMPTY);
			}
			// 检查2对1: E P P -> 移除E
			if (board[r][c] == enemy && board[r][c + 1] == player && board[r][c + 2] == player) {
				board[r][c] = CELL_EMPTY;
				if (is_live) display_draw_piece_at(r, c, CELL_EMPTY);
			}
		}
	}

	// 检查列
	for (c = 0; c < BOARD_N; c++) {
		if (col_occupied_count(board, c) >= 4) continue;
		for (r = 0; r <= BOARD_N - 3; r++) {
			// 检查2对1: P P E -> 移除E
			if (board[r][c] == player && board[r + 1][c] == player && board[r + 2][c] == enemy) {
				board[r + 2][c] = CELL_EMPTY;
				if (is_live) display_draw_piece_at(r + 2, c, CELL_EMPTY);
			}
			// 检查2对1: E P P -> 移除E
			if (board[r][c] == enemy && board[r + 1][c] == player && board[r + 2][c] == player) {
				board[r][c] = CELL_EMPTY;
				if (is_live) display_draw_piece_at(r, c, CELL_EMPTY);
			}
		}
	}
}

// 统计棋子数量
void game_count_pieces(u8 board[BOARD_N][BOARD_N], int *black, int *white)
{
	int r, c;
	*black = 0;
	*white = 0;
	for (r = 0; r < BOARD_N; r++) {
		for (c = 0; c < BOARD_N; c++) {
			if (board[r][c] == CELL_BLACK) (*black)++;
			else if (board[r][c] == CELL_WHITE) (*white)++;
		}
	}
}

// 检查是否有可移动的棋子
int game_has_any_move(u8 board[BOARD_N][BOARD_N], u8 player)
{
	static const int dr[4] = { -1, 1, 0, 0 };
	static const int dc[4] = { 0, 0, -1, 1 };
	int r, c, k;
	for (r = 0; r < BOARD_N; r++) {
		for (c = 0; c < BOARD_N; c++) {
			if (board[r][c] != player) continue;
			for (k = 0; k < 4; k++) {
				int nr = r + dr[k];
				int nc = c + dc[k];
				if (game_in_bounds(nr, nc) && board[nr][nc] == CELL_EMPTY) return 1;
			}
		}
	}
	return 0;
}

// 尝试移动棋子
int game_try_move(int sr, int sc, int tr, int tc)
{
	u8 player = g_turn;
	if (!game_in_bounds(sr, sc) || !game_in_bounds(tr, tc)) return 0;
	if (g_board[sr][sc] != player) return 0;
	if (g_board[tr][tc] != CELL_EMPTY) return 0;
	if (game_abs_i(tr - sr) + game_abs_i(tc - sc) != 1) return 0;

	g_board[tr][tc] = player;
	display_draw_piece_at(tr, tc, player);

	g_board[sr][sc] = CELL_EMPTY;
	display_draw_piece_at(sr, sc, CELL_EMPTY);

	game_apply_captures(g_board, player);
	g_turn = (player == CELL_BLACK) ? CELL_WHITE : CELL_BLACK;
	
	game_update_status_bar();
	game_check_game_end();
	
	return 1;
}

// 检查游戏结束条件
void game_check_game_end(void)
{
	int black, white;
	game_count_pieces(g_board, &black, &white);

	if (black <= 1) { display_set_game_over("WHITE WIN"); return; }
	if (white <= 1) { display_set_game_over("BLACK WIN"); return; }

	if (!game_has_any_move(g_board, g_turn)) {
		if (g_turn == CELL_BLACK) display_set_game_over("BLACK NO MOVE");
		else display_set_game_over("WHITE NO MOVE");
	}
}