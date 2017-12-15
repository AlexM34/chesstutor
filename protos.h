/* board.cpp */
void initialise();
bool checked (int player);
bool play (int from, int to, int player);
bool valid (int from, int to);
void legalmoves (int side);
int capturing (int side);
void finish (int result);
void swap_values (int x, int y);
int points(int piece);
void takeback (int from, int to, int from_piece, int to_color, int to_piece, int castling, int ep);
void square(int coo);

/* search.cpp */
int random();
int greedy (int depth, int color);
int alphaBetaMax (int alpha, int beta, int depth);
int alphaBetaMin (int alpha, int beta, int depth);

/* evaluation.cpp */
int position();

/* main.cpp */
int main();
int parse (char input[64]);
void print();
