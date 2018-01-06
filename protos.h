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
int analyzed (int h);
int rank8 (int square);
int file8 (int square);
int pawnscount (int side, int file);
bool inside (int square);
bool endgame();
void takeback ();
void square(int coo);

/* search.cpp */
void thinkingTime ();
int think (int depth);
int alphaBeta (int alpha, int beta, int depth);
int alphaBetaMax (int alpha, int beta, int depth);
int alphaBetaMin (int alpha, int beta, int depth);

/* evaluation.cpp */
int position();

/* book.cpp */

void open_book();
void close_book();
int book_move();
bool book_match(char *s1, char *s2);

/* main.cpp */
int main();
int parse (char input[64]);
char* notation (int m);
int compmove(int player, int d);
int humanmove (int player);
void print();
