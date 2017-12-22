/* board.cpp */
void initialise();
void init_hash();
int hash_rand();
void set_hash(int player);
void sorthistory ();
bool checked (int player);
bool play (int from, int to);
bool valid (int from, int to);
void legalmoves (int side);
int capturing (int side);
void finish (int result);
void swap_values (int x, int y);
void genpossible (int coo);
int points(int piece);
int analyzed (int h, int d);
int rank8 (int square);
int file8 (int square);
bool inside (int square);
bool endgame();
void takeback (int from, int to, int from_piece, int to_color, int to_piece, int castling, int ep);
void square(int coo);

/* search.cpp */
int random();
int greedy (int depth, int color);
int think (int depth);
int alphaBeta (int alpha, int beta, int depth);
int alphaBetaMax (int alpha, int beta, int depth);
int alphaBetaMin (int alpha, int beta, int depth);

/* evaluation.cpp */
int position();

/* main.cpp */
int main();
int parse (char input[64]);
int compmove(int player, int d);
int humanmove (int player);
void print();
