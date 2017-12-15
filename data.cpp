#include "defs.h"

int color[64];      // white, black or empty
int piece[64];      // pawn, knight, bishop, rook, queen, king or empty
int possible[100][2];
int player;           // white or black
int opponent;         // white or black
int castle;         // castle rights
int ep;             // en passant square
int fifty;          // 50-moves rule
int current;        // current move
int ply;            // depth of calculation
int max_time;
int max_depth;
int start_time;
int end_time;
int nodes;          // nodes analyzed
bool white_short_castle;
bool white_long_castle;
bool black_short_castle;
bool black_long_castle;
int playcount = 0;
int legalcount = 0;
int captcount = 0;
int validcount = 0;

int start_color[64] = {
	6, 1, 6, 6, 1, 6, 1, 6,//1, 1, 1, 1, 1, 1, 1, 1,6, 1, 6, 6, 1, 6, 1, 6,
	1, 1, 1, 1, 1, 1, 1, 1,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	6, 0, 6, 6, 0, 6, 0, 6//0, 0, 0, 0, 0, 0, 0, 0,6, 0, 6, 6, 0, 6, 0, 6
};

int start_piece[64] = {
	6, 1, 6, 6, 5, 6, 1, 6,//3, 1, 2, 4, 5, 2, 1, 3,6, 1, 6, 6, 5, 6, 1, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	6, 1, 6, 6, 5, 6, 1, 6//3, 1, 2, 4, 5, 2, 1, 3,6, 1, 6, 6, 5, 6, 1, 6
};

char piece_letter[6] = {
	'P', 'N', 'B', 'R', 'Q', 'K'
};
