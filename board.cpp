#include "defs.h"
#include "data.h"
#include "protos.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void initialise()
{
	for (int i = 0; i < 64; i++)
    {
		color[i] = start_color[i];
		piece[i] = start_piece[i];
	}

    for (int i = 0; i < 100; i++)
    {
		possible[i][0] = -1;
		possible[i][1] = -1;
	}

	player = WHITE;
	opponent = BLACK;
	castle = 15;
	ep = -1;
	fifty = 0;
	current = 0;
	ply = 0;
	white_long_castle = true;
	white_short_castle = true;
	black_long_castle = true;
	black_short_castle = true;
}

bool checked (int player)
{
    int square = -1;

    for (int i = 0; i < 64; i++)
    {
        if (piece[i] == KING && color[i] == player)
        {
            square = i;
            break;
        }
    }

    for (int i = 0; i < 64; i++)
    {
        if (color[i] == 1 - player && valid(i, square)) return true;
    }

    return false;
}

bool play(int from, int to, int player)
{
    playcount++;
    if ((color[from] != player) || (color[to] == player) || !valid(from, to)) return false;

    int color_to = color[to], piece_to = piece[to], piece_from = piece[from], ep_before = ep;

    color[to] = color[from];
    piece[to] = piece[from];
    color[from] = EMPTY;
    piece[from] = EMPTY;

    if(checked(player))
    {
        takeback(from, to, piece_from, color_to, piece_to, 0, ep_before);

        return false;
    }

    if (((color[to] == BLACK) && (to / 8 == 7) && (piece[to] == PAWN)) ||
        ((color[to] == WHITE) && (to / 8 == 0) && (piece[to] == PAWN))) piece[to] = QUEEN;

    else if (piece[to] == PAWN)
    {
        if (player == WHITE)
        {
            if (from / 8 == 6 && to / 8 == 4 && ((color[to-1] == BLACK && piece[to-1] == PAWN && to % 8 > 0)
                                                 || (color[to+1] == BLACK && piece[to+1] == PAWN && to % 8 < 7))) ep = from - 8;
            else if (from / 8 == 3 && to / 8 == 2 && ((from % 8 == to % 8 + 1) || (from % 8 == to % 8 -1)) && to == ep)
            {
                color[ep + 8] = EMPTY;
                piece[ep + 8] = EMPTY;
            }
        }
        else
        {
            if (from / 8 == 1 && to / 8 == 3 && ((color[to-1] == WHITE && piece[to-1] == PAWN && to % 8 > 0)
                                                 || (color[to+1] == WHITE && piece[to+1] == PAWN && to % 8 < 7))) ep = from + 8;
            else if (from / 8 == 4 && to / 8 == 5 && ((from % 8 == to % 8 + 1) || (from % 8 == to % 8 -1)) && to == ep)
            {
                color[ep - 8] = EMPTY;
                piece[ep - 8] = EMPTY;
            }
        }
    }

    else if (piece[to] == KING)
    {
        if (color[to] == WHITE)
        {
            if (abs(from - to) == 2)
            {
                if (to == 58)
                {
                    color[59] = color[56];
                    piece[59] = piece[56];
                    color[56] = EMPTY;
                    piece[56] = EMPTY;
                }

                else if (to == 62)
                {
                    color[61] = color[63];
                    piece[61] = piece[63];
                    color[63] = EMPTY;
                    piece[63] = EMPTY;
                }
            }

            white_long_castle = false;
            white_short_castle = false;
        }

        else if (color[to] == BLACK)
        {
            if (abs(from - to) == 2)
            {
                if (to == 2)
                {
                    color[3] = color[0];
                    piece[3] = piece[0];
                    color[0] = EMPTY;
                    piece[0] = EMPTY;
                }

                else if (to == 6)
                {
                    color[5] = color[7];
                    piece[5] = piece[7];
                    color[7] = EMPTY;
                    piece[7] = EMPTY;
                }
            }

            black_long_castle = false;
            black_short_castle = false;
        }
    }

    if (from == 56 || to == 56) white_long_castle = false;
    if (from == 63 || to == 63) white_short_castle = false;
    if (from == 0 || to == 0) black_long_castle = false;
    if (from == 7 || to == 7) black_short_castle = false;

    if (ep_before == ep) ep = -1;
    /*if (ep != -1)
    {
        jbl++;
        if (jbl % 100 == 0)
        {
            printf("The ep is %d %d-%d\n", ep, from, to);
            square(ep);
            square(from);
            square(to);
            print();
        }
    }*/
    return true;
}

bool valid (int from, int to)
{
    validcount++;
    int j = from - to;

    if (piece[from] == PAWN)
    {
        if (color[from] == WHITE)
        {
            if (j == 8 && color[to] == EMPTY) return true;
            else if (from / 8 == 6 && j == 16 && color[to + 8] == EMPTY && color[to] == EMPTY) return true;
            else if (color[to] == BLACK && from / 8 == to / 8 + 1 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1)) return true;
            else if (from / 8 == 3 && to / 8 == 2 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1) && ep == to)return true;
        }

        else if (color[from] == BLACK)
        {
            if (j == -8 && color[to] == EMPTY) return true;
            else if (from / 8 == 1 && j == -16 && color[to - 8] == EMPTY && color[to] == EMPTY) return true;
            else if (color[to] == WHITE && from / 8 == to / 8 - 1 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1)) return true;
            else if (from / 8 == 4 && to / 8 == 5 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1) && ep == to) return true;
        }
    }

    else if (piece[from] == KNIGHT)
    {
        if (((abs(from / 8 - to / 8) == 1) && (abs(from % 8 - to % 8) == 2)) ||
            ((abs(from / 8 - to / 8) == 2) && (abs(from % 8 - to % 8) == 1))) return true;
    }

    else if (piece[from] == BISHOP)
    {
        if ((from / 8 + from % 8) == (to / 8 + to % 8))
        {
            for (int i = from + 7; i < to; i += 7)
            {
                if (color[i] != EMPTY) return false;
            }

            for (int i = to + 7; i < from; i += 7)
            {
                if (color[i] != EMPTY) return false;
            }

            return true;
        }

        if ((from / 8 - from % 8) == (to / 8 - to % 8))
        {
            for (int i = from + 9; i < to; i += 9)
            {
                if (color[i] != EMPTY) return false;
            }

            for (int i = to + 9; i < from; i += 9)
            {
                if (color[i] != EMPTY) return false;
            }

            return true;
        }
    }

    else if (piece[from] == ROOK)
    {
        if (j % 8 == 0)
        {
            for (int i = from + 8; i < to; i += 8)
            {
                if (color[i] != EMPTY) return false;
            }

            for (int i = to + 8; i < from; i += 8)
            {
                if (color[i] != EMPTY) return false;
            }

            return true;
        }

        else if (from / 8 == to / 8)
        {
            for (int i = from + 1; i < to; i++)
            {
                if (color[i] != EMPTY) return false;
            }

            for (int i = to + 1; i < from; i++)
            {
                if (color[i] != EMPTY) return false;
            }

            return true;
        }
    }

    else if (piece[from] == QUEEN)
    {
        bool check;
        if ((from / 8 + from % 8) == (to / 8 + to % 8))
        {
            check = true;
            for (int i = from + 7; i < to; i += 7)
            {
                if (color[i] != EMPTY) check = false;
            }

            for (int i = to + 7; i < from; i += 7)
            {
                if (color[i] != EMPTY) check = false;
            }

            if (check) return true;
        }

        if ((from / 8 - from % 8) == (to / 8 - to % 8))
        {
            check = true;
            for (int i = from + 9; i < to; i += 9)
            {
                if (color[i] != EMPTY) check = false;
            }

            for (int i = to + 9; i < from; i += 9)
            {
                if (color[i] != EMPTY) check = false;
            }

            if (check) return true;
        }

        if (j % 8 == 0)
        {
            check = true;
            for (int i = from + 8; i < to; i += 8)
            {
                if (color[i] != EMPTY) check = false;
            }

            for (int i = to + 8; i < from; i += 8)
            {
                if (color[i] != EMPTY) check = false;
            }

            if (check) return true;
        }

        else if (from / 8 == to / 8)
        {
            check = true;
            for (int i = from + 1; i < to; i++)
            {
                if (color[i] != EMPTY) check = false;
            }

            for (int i = to + 1; i < from; i++)
            {
                if (color[i] != EMPTY) check = false;
            }

            if (check) return true;
        }
    }

    else if (piece[from] == KING)
    {
        if (((from / 8 == to / 8) || (from / 8 == to / 8 + 1) || (from / 8 == to / 8 - 1)) &&
            ((from % 8 == to % 8) || (from % 8 == to % 8 + 1) || (from % 8 == to % 8 - 1))) return true;

        else if (color[from] == WHITE)
        {
            if (from == 60 && to == 58 && white_long_castle && color[57] == EMPTY && color[58] == EMPTY && color[59] == EMPTY)
            {
                bool legal = false;
                color[59] = color[60];
                piece[59] = piece[60];
                color[60] = EMPTY;
                piece[60] = EMPTY;
                legal = legal || checked(WHITE);
                color[58] = color[59];
                piece[58] = piece[59];
                color[59] = EMPTY;
                piece[59] = EMPTY;
                legal = legal || checked(WHITE);
                color[60] = color[58];
                piece[60] = piece[58];
                color[58] = EMPTY;
                piece[58] = EMPTY;

                return !legal;
            }

            else if (from == 60 && to == 62 && white_short_castle && color[61] == EMPTY && color[62] == EMPTY)
            {
                bool legal = false;
                color[61] = color[60];
                piece[61] = piece[60];
                color[60] = EMPTY;
                piece[60] = EMPTY;
                legal = legal || checked(WHITE);
                color[62] = color[61];
                piece[62] = piece[61];
                color[61] = EMPTY;
                piece[61] = EMPTY;
                legal = legal || checked(WHITE);
                color[60] = color[62];
                piece[60] = piece[62];
                color[62] = EMPTY;
                piece[62] = EMPTY;

                return !legal;
            }
        }

        else if (color[from] == BLACK)
        {
            if (from == 4 && to == 2 && black_long_castle && color[1] == EMPTY && color[2] == EMPTY && color[3] == EMPTY)
            {
                bool legal = false;
                color[3] = color[4];
                piece[3] = piece[4];
                color[4] = EMPTY;
                piece[4] = EMPTY;
                legal = legal || checked(BLACK);
                color[2] = color[3];
                piece[2] = piece[3];
                color[3] = EMPTY;
                piece[3] = EMPTY;
                legal = legal || checked(BLACK);
                color[4] = color[2];
                piece[4] = piece[2];
                color[2] = EMPTY;
                piece[2] = EMPTY;

                return !legal;
            }

            else if (from == 4 && to == 6 && black_short_castle && color[5] == EMPTY && color[6] == EMPTY)
            {
                bool legal = false;
                color[5] = color[4];
                piece[5] = piece[4];
                color[4] = EMPTY;
                piece[4] = EMPTY;
                legal = legal || checked(BLACK);
                color[6] = color[5];
                piece[6] = piece[5];
                color[5] = EMPTY;
                piece[5] = EMPTY;
                legal = legal || checked(BLACK);
                color[4] = color[6];
                piece[4] = piece[6];
                color[6] = EMPTY;
                piece[6] = EMPTY;

                return !legal;
            }
        }
    }

    return false;
}

void legalmoves(int side)
{
    legalcount++;
    srand(time(NULL));
    int current = 0, c, p, p1, p2, x, piece_from, ep_before;
    for (int i = 0; i < 64; i++)
    {
        if (color[i] == side)
        {
            for (int j = 0; j < 64; j++)
            {
                if ((color[j] != side) && valid(i, j))
                {
                    c = color[j];
                    p = piece[j];
                    piece_from = piece[i];
                    ep_before = ep;

                    bool legal = play(i, j, side);
                    if (legal)
                    {
                        takeback(i, j, piece_from, c, p, 0, ep_before);

                        possible[current][0] = i;
                        possible[current][1] = j;
                        current++;
                    }
                }
            }
        }
    }

    for (int i = current; i < 100; i++)
    {
        possible[i][0] = -1;
        possible[i][1] = -1;
    }

    for (int i = 0; i < current; i++)
    {
        for (int j = i+1; j < current; j++)
        {
            x = rand() % 2;
            if (x == 0)
            {
                x = possible[i][0];
                possible[i][0] = possible[j][0];
                possible[j][0] = x;
                x = possible[i][1];
                possible[i][1] = possible[j][1];
                possible[j][1] = x;
            }
        }
    }

    for (int i = 0; i < current; i++)
    {
        for (int j = i+1; j < current; j++)
        {
            p1 = possible[i][1];
            p2 = possible[j][1];

            if (points(p1) > points(p2))
            {
                x = possible[i][0];
                possible[i][0] = possible[j][0];
                possible[j][0] = x;
                x = possible[i][1];
                possible[i][1] = possible[j][1];
                possible[j][1] = x;
            }
        }
    }
}

int capturing (int side)
{
    captcount++;
    int list_moves[100][2], current = 0, mult = 2 * side - 1, res = position(), r, from, to, c, p, val, orig_piece, ep_before;     // mult = -1 if WHITE and 1 if BLACK
    legalmoves(side);

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        current++;
    }

    if (current == 0) return 1000 * mult;
    current--;
    to = list_moves[current][1];
    //printf("current can't fight the friction, soooooo... is %d %d %d\n", current, to, color[to]);
    if (color[to] != (1-side)) return res;

    while (current >= 0)
    {
        to = list_moves[current][1];
        //printf("current is %d %d %d %d %d\n", current, list_moves[current][0], list_moves[current][1], 1-side, color[to]);
        if (color[to] == (1-side))
        {
            val = points(to);
            /*if (val * mult >= res * mult)
            {
                //printf("%d\n", res);
                return res;
            }

            else
            {*/
                //printf("Jordiiiiiiiiiii\n");
                /*int cac[64], cap[64];
                for (int i = 0; i < 64; i++)
                {
                    cac[i] = color[i];
                    cap[i] = piece[i];
                }*/
                from = list_moves[current][0];
                c = color[to];
                p = piece[to];
                orig_piece = piece[from];
                ep_before = ep;
                bool legal = play(from, to, side);
                if(legal)
                {
                    //printf("Lacazeeeeeeeeeeettte\n");
                    r = capturing(1-side);
                    takeback(from, to, orig_piece, c, p, 0, ep_before);
                }
                else printf("illegaaaaaaaaaaaaaal\n");

                /*bool same = true;
                for (int i = 0; i < 64; i++)
                {
                    same = same && (cac[i] == color[i]) && (cap[i] == piece[i]);
                }
                if (!same) printf("BUGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");*/

            if (r * mult < res * mult) res = r;
        }

        else
        {
            //printf("%d\n", res);
            return res;
        }

        current--;
    }

    return res;
}

void takeback (int from, int to, int from_piece, int to_color, int to_piece, int castling, int ep_now)
{
    color[from] = color[to];
    piece[from] = from_piece;
    color[to] = to_color;
    piece[to] = to_piece;
    ep = ep_now;

    if (ep == to && piece[from] == PAWN && from % 8 != to % 8 && color[to] == EMPTY)
    {
        if (color[from] == WHITE)
        {
            color[ep + 8] = BLACK;
            piece[ep + 8] = PAWN;
        }
        else
        {
            color[ep - 8] = WHITE;
            piece[ep - 8] = PAWN;
        }
    }
}

int points(int square)
{
    switch(piece[square])
    {
        case PAWN: return 1;
        case KNIGHT:
        case BISHOP: return 3;
        case ROOK: return 5;
        case QUEEN: return 9;
        default: return 0;
    }
}

void square(int coo)
{
    printf("%c%d\n", ('a' + coo % 8), 8 - coo / 8);
}

void swap_values(int x, int y)
{
    int i = x;
    x = y;
    y = i;
}

void finish (int result)
{
    if (result == 1) printf("Black is checkmated!");
    else if (result == 0) printf("Stalemate!");
    else if (result == -1) printf("White is checkmated!");
}
