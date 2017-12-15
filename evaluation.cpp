#include <string.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define DOUBLED_PAWN_PENALTY		10
#define ISOLATED_PAWN_PENALTY		20
#define BACKWARDS_PAWN_PENALTY		8
#define PASSED_PAWN_BONUS			20
#define ROOK_SEMI_OPEN_FILE_BONUS	10
#define ROOK_OPEN_FILE_BONUS		15
#define ROOK_ON_SEVENTH_BONUS		20

int position()
{
    int eval = 0;
    for (int i = 0; i < 64; i++)
    {
        if (color[i] != EMPTY)
        {
             switch( piece[i] )
            {
                case PAWN: eval += ((-2) * color[i] + 1); break;
                case KNIGHT:
                case BISHOP: eval += ((-2) * color[i] + 1) * 3; break;
                case ROOK: eval += ((-2) * color[i] + 1) * 5; break;
                case QUEEN: eval += ((-2) * color[i] + 1) * 9; break;
            }
        }
    }

    return eval;
}
