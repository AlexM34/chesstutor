#include <string.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int pawns[64] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-30,-30, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

int knights[64] =
{
	-50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

int bishops[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

int rooks[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0
};

int queens[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

int kings[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

int kings_end[64] =
{
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

int flip[64] =
{
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
};

int position()
{
    int eval = 25 * (1 - 2 * sideToMove), x;
    for (int i = 0; i < 64; i++)
    {
        if (color[i] != EMPTY)
        {
            int side = 1 - 2 * color[i];
            bool isolated = true, backwards = true, passed = true;
            switch( piece[i] )
            {
                case PAWN: eval += side * 100;
                           if (color[i] == WHITE) eval += pawns[i];
                           else eval -= pawns[flip[i]];
                           if (file8(i) > 0 && pawnscount(color[i], file8(i)-1) > 0) isolated = false;
                           if (file8(i) < 7 && pawnscount(color[i], file8(i)+1) > 0) isolated = false;
                           if (isolated) eval -= side * 15;
                           x = pawnscount(color[i], file8(i));
                           if (x > 1) eval -= side * (x - 1) * 10;
                           if (color[i] == WHITE)
                           {
                               for (int j = rank8(i) + 1; j < 7; j++)
                               {
                                   x = 8 * j + file8(i) - 1;
                                   if (file8(i) > 0 && piece[x] == PAWN && color[x] == WHITE) backwards = false;
                                   x = 8 * j + file8(i) + 1;
                                   if (file8(i) < 7 && piece[x] == PAWN && color[x] == WHITE) backwards = false;
                               }
                               for (int j = rank8(i) - 1; j > 0; j--)
                               {
                                   x = 8 * j + file8(i) - 1;
                                   if (file8(i) > 0 && piece[x] == PAWN && color[x] == BLACK) passed = false;
                                   x = 8 * j + file8(i) + 1;
                                   if (file8(i) < 7 && piece[x] == PAWN && color[x] == BLACK) passed = false;
                                   x = 8 * j + file8(i);
                                   if (piece[x] == PAWN && color[x] == BLACK) passed = false;
                               }

                               if (backwards)
                               {
                                   backwards = false;
                                   if (file8(i) > 0 && inside(i-17) && piece[i-17] == PAWN && color[i-17] == BLACK) backwards = true;
                                   if (file8(i) < 7 && inside(i-15) && piece[i-15] == PAWN && color[i-15] == BLACK) backwards = true;
                                   if (backwards) eval -= 8;
                               }
                               if (passed) eval += 15;
                           }
                           else
                           {
                               for (int j = rank8(i) - 1; j > 0; j--)
                               {
                                   x = 8 * j + file8(i) - 1;
                                   if (file8(i) > 0 && piece[x] == PAWN && color[x] == BLACK) backwards = false;
                                   x = 8 * j + file8(i) + 1;
                                   if (file8(i) < 7 && piece[x] == PAWN && color[x] == BLACK) backwards = false;
                               }
                               for (int j = rank8(i) + 1; j < 7; j++)
                               {
                                   x = 8 * j + file8(i) - 1;
                                   if (file8(i) > 0 && piece[x] == PAWN && color[x] == WHITE) passed = false;
                                   x = 8 * j + file8(i) + 1;
                                   if (file8(i) < 7 && piece[x] == PAWN && color[x] == WHITE) passed = false;
                                   x = 8 * j + file8(i);
                                   if (piece[x] == PAWN && piece[x] == WHITE) passed = false;
                               }

                               if (backwards)
                               {
                                   backwards = false;
                                   if (file8(i) > 0 && inside(i+15) && piece[i+15] == PAWN && color[i+15] == WHITE) backwards = true;
                                   if (file8(i) < 7 && inside(i+17) && piece[i+17] == PAWN && color[i+17] == WHITE) backwards = true;
                                   if (backwards) eval += 8;
                               }
                               if (passed) eval -= 15;
                           }

                           break;

                case KNIGHT: eval += side * 325;
                             if (color[i] == WHITE) eval += knights[i];
                             else eval -= knights[flip[i]];
                             break;

                case BISHOP: eval += side * 350;
                             if (color[i] == WHITE) eval += bishops[i];
                             else eval -= bishops[flip[i]];
                             break;

                case ROOK:   eval += side * 525;
                             if (color[i] == WHITE) eval += rooks[i];
                             else eval -= rooks[flip[i]];
                             if (pawnscount(color[i], file8(i)) == 0)
                             {
                                 eval += side * 15;
                                 if (pawnscount(1 - color[i], file8(i)) == 0) eval += side * 10;
                             }
                             break;
                case QUEEN:  eval += side * 950;
                             if (color[i] == WHITE) eval += queens[i];
                             else eval -= queens[flip[i]];
                             break;
                case KING: if (endgame())
                           {
                               if (color[i] == WHITE) eval += kings_end[i];
                               else eval -= kings_end[flip[i]];
                           }
                           else
                           {
                               if (color[i] == WHITE) eval += kings[i];
                               else eval -= kings[flip[i]];
                           }
                           break;
            }
        }
    }

    return eval;
}
