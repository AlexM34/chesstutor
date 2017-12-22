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

	sideToMove = WHITE;
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

/* init_hash() initializes the random numbers used by set_hash(). */

void init_hash()
{
	int i, j, k;

	srand(0);
	for (i = 0; i < 2; ++i)
    {
		for (j = 0; j < 6; ++j)
        {
            for (k = 0; k < 64; ++k)
            {
                hash_piece[i][j][k] = hash_rand();
				hash_piece2[i][j][k] = hash_rand();
            }
        }
    }
	hash_side = hash_rand();
	hash_side2 = hash_rand();
	for (i = 0; i < 64; ++i)
    {
        hash_ep[i] = hash_rand();
		hash_ep2[i] = hash_rand();
    }

    for (i = 0; i < MAX_SIZE; i++)
    {
        //hashes[i] = -1;
        hasheval[i] = -1;
        hasheval2[i] = -1;
        hashdepth[i] = -1;
        hashtype[i] = -1;
    }

    for (i = 0; i < 4096; i++)
    {
        history[i] = 0;
        fromto[i] = 100 * (i / 64) + (i % 64);
    }
}

void sorthistory ()
{
    int x;
    for (int i = 0; i < 4096; i++)
    {
        for (int j = i + 1; j < 4096; j++)
        {
            if (history[i] < history[j])
            {
                x = history[i];
                history[i] = history[j];
                history[j] = x;
                x = fromto[i];
                fromto[i] = fromto[j];
                fromto[j] = x;
            }
        }
    }
}

/* hash_rand() XORs some shifted random numbers together to make sure
   we have good coverage of all 32 bits. (rand() returns 16-bit numbers
   on some systems.) */

int hash_rand()
{
	long long r = 0;

	for (int i = 0; i < 32; i++)
		r ^= rand() << i;
	//printf("%llu\n", r);
	r = r % 10000000;
	int x = (int) r;
	x = abs(r);
	//printf("%d\n", x);
	return x;
}


/* set_hash() uses the Zobrist method of generating a unique number (hash)
   for the current chess position. Of course, there are many more chess
   positions than there are 32 bit numbers, so the numbers generated are
   not really unique, but they're unique enough for our purposes (to detect
   repetitions of the position).
   The way it works is to XOR random numbers that correspond to features of
   the position, e.g., if there's a BLACK KNIGHT on B8, hash is XORed with
   hash_piece[BLACK][KNIGHT][B8]. All of the pieces are XORed together,
   hash_side is XORed if it's BLACK's move, and the en passant square is
   XORed if there is one. (A chess technicality is that one position can't
   be a repetition of another if the en passant state is different.) */

void set_hash(int player)
{
    int i;
	hashing = 0;
	hashing2 = 0;
	for (i = 0; i < 64; ++i)
    {
		if (color[i] != EMPTY)
        {
			hashing ^= hash_piece[color[i]][piece[i]][i];
			hashing2 ^= hash_piece2[color[i]][piece[i]][i];
        }
    }
	if (player == BLACK)
    {
        hashing ^= hash_side;
		hashing2 ^= hash_side2;
    }
	if (ep != -1)
    {
		hashing ^= hash_ep[ep];
		hashing2 ^= hash_ep2[ep];
    }
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

bool play(int from, int to)
{
    playcount++;
    if ((color[from] != sideToMove) || (color[to] == sideToMove) || !valid(from, to)) return false;

    int color_to = color[to], piece_to = piece[to], piece_from = piece[from], ep_before = ep;

    color[to] = color[from];
    piece[to] = piece[from];
    color[from] = EMPTY;
    piece[from] = EMPTY;
    ply++;

    if(checked(sideToMove))
    {
        sideToMove = 1 - sideToMove;
        takeback(from, to, piece_from, color_to, piece_to, 0, ep_before);

        return false;
    }

    set_hash(sideToMove);

    if (((color[to] == WHITE) && (to / 8 == 0) && (piece[to] == PAWN)) ||
        ((color[to] == BLACK) && (to / 8 == 7) && (piece[to] == PAWN))) piece[to] = QUEEN;

    if (piece[to] == PAWN)
    {
        if (to / 8 == sideToMove * 7) piece[to] = QUEEN;

        else if (sideToMove == WHITE)
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
    sideToMove = 1 - sideToMove;
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
    int current = 0, c, p, p1, p2, x, piece_from, ep_before, j;
    for (int i = 0; i < 64; i++)
    {
        if (color[i] == side)
        {
            genpossible(i);
            //printf("%d %d\n", i, totaloffs);
            for (int k = 0; k < totaloffs; k++)
            {
                j = i + offset[k];
                //printf("%d\n", j);
                if (color[j] != side)
                {
                    c = color[j];
                    p = piece[j];
                    piece_from = piece[i];
                    ep_before = ep;

                    bool legal = play(i, j);
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

    /*for (int i = 0; i < current; i++)
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
    }*/

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

    int captures = 0, from, to, p3, p4;

    for (int i = 0; i < current; i++)
    {
        p1 = possible[i][1];
        if (points(p1) > 0) captures++;
    }

    for (int i = captures; i < current; i++)
    {
        for (int j = i + 1; j < current; j++)
        {
            p1 = possible[i][0];
            p2 = possible[i][1];
            p3 = possible[j][0];
            p4 = possible[j][1];

            if (history[100 * p1 + p2] < history[100 * p3 + p4])
            {
                possible[i][0] = p3;
                possible[j][0] = p1;
                possible[i][1] = p4;
                possible[j][1] = p2;
            }
        }
    }
}

int capturing (int side)
{
    captcount++;
    int list_moves[100][2], current = 0, mult = 2 * side - 1, res = position(), r, from, to, c, p, val, orig_piece, ep_before;     // mult = -1 if WHITE and 1 if BLACK
    //legalmoves(side);

    int piece_from, j;
    for (int i = 0; i < 64; i++)
    {
        if (color[i] == side)
        {
            genpossible(i);
            //printf("%d %d\n", i, totaloffs);
            for (int k = 0; k < totaloffs; k++)
            {
                j = i + offset[k];
                //printf("%d\n", j);
                if (color[j] == 1 - side)
                {
                    c = color[j];
                    p = piece[j];
                    piece_from = piece[i];
                    ep_before = ep;

                    bool legal = play(i, j);
                    if (legal)
                    {
                        takeback(i, j, piece_from, c, p, 0, ep_before);

                        list_moves[current][0] = i;
                        list_moves[current][1] = j;
                        current++;
                        //printf("%d\n", current);
                    }
                }
            }
        }
    }
    /*while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        current++;
    }*/

    if (current == 0) return position();
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
                bool legal = play(from, to);
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
    ply--;
    sideToMove = 1 - sideToMove;

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

void genpossible (int coo)
{
    totaloffs = 0;
    int side = 2 * color[coo] - 1, x;
    for (int i = 0; i < 30; i++) offset[i] = 0;
    switch (piece[coo])
    {
        case PAWN:
            offset[totaloffs] = 8 * side;
            totaloffs++;
            if (rank8(coo) == 1 || rank8(coo) == 6)
            {
                offset[totaloffs] = 16 * side;
                totaloffs++;
            }
            x = coo + 8 * side + 1;
            if (color[x] == 1 - color[coo])
            {
                offset[totaloffs] = x - coo;
                totaloffs++;
            }
            x = coo + 8 * side - 1;
            if (color[x] == 1 - color[coo])
            {
                offset[totaloffs] = x - coo;
                totaloffs++;
            }
            break;

        case KNIGHT:
            for (int i = -1; i < 2; i += 2)
            {
                for (int j = 1; j < 3; j++)
                {
                    for (int k = -1; k < 2; k += 2)
                    {
                        x = i * (8 * j + (3-j) * k);
                        if (inside(coo + x))
                        {
                            offset[totaloffs] = x;
                            totaloffs++;
                        }
                    }
                }
            }
            break;

        case BISHOP:
            for (int i = 1; i < 8; i++)
            {
                x = 7 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }
            for (int i = 1; i < 8; i++)
            {
                x = 9 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }

            for (int i = 1; i < 8; i++)
            {
                x = -7 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }
            for (int i = 1; i < 8; i++)
            {
                x = -9 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }
            break;

        case ROOK:
            for (int i = -file8(coo); i < 8 - file8(coo); i++)
            {
                x = coo + i;
                if (i != 0)
                {
                    offset[totaloffs] = i;
                    totaloffs++;
                }
            }
            for (int i = -rank8(coo); i < 8 - rank8(coo); i++)
            {
                x = coo + 8 * i;
                if (i != 0)
                {
                    offset[totaloffs] = i;
                    totaloffs++;
                }
            }
            break;

        case QUEEN:

            for (int i = 1; i < 8; i++)
            {
                x = 7 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }
            for (int i = 1; i < 8; i++)
            {
                x = 9 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }

            for (int i = 1; i < 8; i++)
            {
                x = -7 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }
            for (int i = 1; i < 8; i++)
            {
                x = -9 * i;
                if (inside(coo + x))
                {
                    offset[totaloffs] = x;
                    totaloffs++;
                }
                else break;
            }
            for (int i = -file8(coo); i < 8 - file8(coo); i++)
            {
                x = coo + i;
                if (i != 0)
                {
                    offset[totaloffs] = i;
                    totaloffs++;
                }
            }
            for (int i = -rank8(coo); i < 8 - rank8(coo); i++)
            {
                x = coo + 8 * i;
                if (i != 0)
                {
                    offset[totaloffs] = i;
                    totaloffs++;
                }
            }
            break;

        case KING:
            for (int i = -1; i < 2; i++)
            {
                for (int j = -1; j < 2; j++)
                {
                    x = 8 * i + j;
                    if (x != 0 && inside(coo+x))
                    {
                        offset[totaloffs] = x;
                        totaloffs++;
                    }
                }
            }
            break;

        default: totaloffs = 0; break;
    }
}

class HashEntry
{
    private:
          int key;
          int value;

    public:
          HashEntry(int key, int value)
          {
                this->key = key;
                this->value = value;
          }

          int getKey()
          {
              return key;
          }
          int getValue()
          {
              return value;
          }
};

const int TABLE_SIZE = MAX_SIZE;

class HashMap
{
    private: HashEntry **table;

    public: HashMap()
    {
        table = new HashEntry*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = NULL;
    }

     int get(int key)
      {
            int h = (key % TABLE_SIZE);

            while (table[h] != NULL && table[h]->getKey() != key) h = (h + 1) % TABLE_SIZE;
            if (table[h] == NULL) return -1;
            else return table[h]->getValue();
      }

      void put(int key, int value)
      {
            int h = (key % TABLE_SIZE);
            while (table[h] != NULL && table[h]->getKey() != key) h = (h + 1) % TABLE_SIZE;

            if (table[h] != NULL) delete table[h];
            table[h] = new HashEntry(key, value);
      }

      ~HashMap()
      {
            for (int i = 0; i < TABLE_SIZE; i++)
            {
                if (table[i] != NULL) delete table[i];
            }

            delete[] table;
      }
};

int analyzed (int h, int d)
{
    for (int i = 0; i < hashcount; i++)
    {
        //if (h == hashes[i])
        {
            if (d <= hashdepth[i]) return i;
            else return -1;
        }
    }

    return -1;
}

int points(int square)
{
    switch(piece[square])
    {
        case PAWN: return 100;
        case KNIGHT: return 325;
        case BISHOP: return 350;
        case ROOK: return 525;
        case QUEEN: return 950;
        default: return 0;
    }
}

int rank8 (int square)
{
    return square / 8;
}

int file8 (int square)
{
    return square % 8;
}

bool inside (int square)
{
    return square >=0 && square < 64;
}

void square(int coo)
{
    printf("%c%d\n", ('a' + coo % 8), 8 - coo / 8);
}

bool endgame()
{
    int all = 0;
    for(int i = 0; i < 64; i++) all += points(i);
    return all < 2500;
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
