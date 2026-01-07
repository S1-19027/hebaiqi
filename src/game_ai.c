#include "game.h"

// AI回合
void ai_turn(void)
{
	typedef struct { u8 sr, sc, tr, tc; int score; } Move;
	Move best;
	int found = 0;
	int sr, sc, k;
	static const int dr[4] = { -1, 1, 0, 0 };
	static const int dc[4] = { 0, 0, -1, 1 };
	u8 player = CELL_WHITE;
	u8 enemy = CELL_BLACK;

	for (sr = 0; sr < BOARD_N; sr++) {
		for (sc = 0; sc < BOARD_N; sc++) {
			if (g_board[sr][sc] != player) continue;
			for (k = 0; k < 4; k++) {
				int tr = sr + dr[k];
				int tc = sc + dc[k];
				u8 tmp[BOARD_N][BOARD_N];
				int r, c;
				int b, w;
				int score;
				if (!game_in_bounds(tr, tc) || g_board[tr][tc] != CELL_EMPTY) continue;
				for (r = 0; r < BOARD_N; r++) for (c = 0; c < BOARD_N; c++) tmp[r][c] = g_board[r][c];
				tmp[tr][tc] = player;
				tmp[sr][sc] = CELL_EMPTY;
				game_apply_captures(tmp, player);
				game_count_pieces(tmp, &b, &w);
				score = (w - b) * 100;
				score += game_has_any_move(tmp, enemy) ? 0 : 1000;
				score += (int)(game_rng_next() & 0x1F);
				if (!found || score > best.score) {
					found = 1;
					best.sr = (u8)sr;
					best.sc = (u8)sc;
					best.tr = (u8)tr;
					best.tc = (u8)tc;
					best.score = score;
				}
			}
		}
	}

	if (!found) {
		game_check_game_end();
		return;
	}

	g_turn = player;
	game_try_move(best.sr, best.sc, best.tr, best.tc);
}