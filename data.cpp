#include "defs.h"

int MAX_SIZE = 100000000;
int color[64];      // white, black or empty
int piece[64];      // pawn, knight, bishop, rook, queen, king or empty
int possible[100][3];
int promotion = QUEEN;   // promoted piece
int offset[30];     // possible offsets
int totaloffs = 0;         // count of possible moves
int sideToMove = WHITE;           // white or black
int opponent;         // white or black
int castle;         // castle rights
int ep;             // en passant square
int fifty;          // 50-moves rule
int current;        // current move
int ply = 0;            // depth of calculation
int max_time;
int max_depth;
int start_time;
int stop_time;
int nodes = 0;          // nodes analyzed
int hashing;
bool white_short_castle;
bool white_long_castle;
bool black_short_castle;
bool black_long_castle;
int hash_piece[2][6][64];
int hash_side;
int hash_ep[64];
int playcount = 0;
int legalcount = 0;
int captcount = 0;
int validcount = 0;
int hashcount = 0;
int hasheval[100000000];
int hashes[100000000];
int hashdepth[100000000];
int hashtype[100000000];
int history[4096];
int pv[500];
int last[100];
int dcount[100];
int from_last[1000];
int to_last[1000];
int color_to[1000];
int piece_to[1000];
int piece_from[1000];
int castling_before[1000];
int ep_before[1000];
int fifty_before[1000];
int pvcount = 0;
int hits = 0;
int t1 = 0;
int t2 = 0;
int t3 = 0;
int captdepth = 0;
int distToRoot = 0;
int evaluation;
int path[1000];
int path2[1000];
int reps[1000];
int repcount = 0;
bool threefold = false;
int moves = 0;
bool zeitnot = false;

int start_color[64] = {
	1, 1, 1, 1, 1, 1, 1, 1,//1, 1, 1, 1, 1, 1, 1, 1,6, 1, 6, 6, 1, 6, 1, 6,
	1, 1, 1, 1, 1, 1, 1, 1,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0//0, 0, 0, 0, 0, 0, 0, 0,6, 0, 6, 6, 0, 6, 0, 6
};

int start_piece[64] = {
	3, 1, 2, 4, 5, 2, 1, 3,//3, 1, 2, 4, 5, 2, 1, 3,6, 1, 6, 6, 5, 6, 1, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	3, 1, 2, 4, 5, 2, 1, 3//3, 1, 2, 4, 5, 2, 1, 3,6, 1, 6, 6, 5, 6, 1, 6
};

char piece_letter[6] = {
	'P', 'N', 'B', 'R', 'Q', 'K'
};
