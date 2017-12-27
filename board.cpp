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

	for (int i = 0; i < 500; i++) pv[i] = -1;
	for (int i = 0; i < 100; i++) last[i] = -1;
	for (int i = 0; i < 1000; i++)
    {
        dcount[i] = 0;
        from_last[i] = -1;
        to_last[i] = -1;
        color_to[i] = -1;
        piece_to[i] = -1;
        piece_from[i] = -1;
        castling_before[i] = -1;
        ep_before[i] = -1;
    }

	sideToMove = WHITE;
	opponent = BLACK;
	castle = 15;
	ep = -1;
	fifty = 0;
	current = 0;
	ply = 0;
	if (color[56] == WHITE) white_long_castle = true;
	if (color[63] == WHITE) white_short_castle = true;
	if (color[0] == BLACK) black_long_castle = true;
	if (color[7] == BLACK) black_short_castle = true;
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

    for (i = 0; i < 4096; i++) history[i] = 0;
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
    if (from < 0 || to < 0 || from > 63 || to > 63) return false;
    playcount++;
    if ((color[from] != sideToMove) || (color[to] == sideToMove) || !valid(from, to)) return false;

    from_last[ply] = from;
    to_last[ply] = to;
    color_to[ply] = color[to];
    piece_to[ply] = piece[to];
    piece_from[ply] = piece[from];
    ep_before[ply] = ep;
    castling_before[ply] = 1000 * white_long_castle + 100 * white_short_castle + 10 * black_long_castle + black_short_castle;
    fifty_before[ply] = fifty;
    if (color[to] == 1 - sideToMove) fifty = ply;

    color[to] = color[from];
    piece[to] = piece[from];
    color[from] = EMPTY;
    piece[from] = EMPTY;
    ply++;

    if(checked(sideToMove))
    {
        sideToMove = 1 - sideToMove;
        takeback();

        return false;
    }

    set_hash(sideToMove);

    if (piece[to] == PAWN)
    {
        fifty = ply;
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

    if (ep_before[ply-1] == ep) ep = -1;
    sideToMove = 1 - sideToMove;
    return true;
}

/*bool free (int from, int to)
{
    if (color[from] == color[to]) return false;
    if (rank8(from) == rank8(to))
    {
        for (int i = 0; i < 8; i++)
        {
            if ((file8(from) - i) * (file8(to) - i) < 0 && color[rank8(from) * 8 + i] != EMPTY) return false;
        }

        return true;
    }

    if (file8(from) == file8(to))
    {
        for (int i = 0; i < 8; i++)
        {
            if ((rank8(from) - i) * (rank8(to) - i) < 0 && color[file8(from) + 8 * i] != EMPTY) return false;
        }

        return true;
    }

    if (rank8(from) + file8(from) == rank8(to) + file8(to))
    {
        for (int i = 0; i < 8; i++)
        {

        }
    }
}*/

bool valid (int from, int to)
{
    validcount++;
    int j = from - to;

    if (piece[from] == PAWN)
    {
        if (color[from] == WHITE)
        {
            if (j == 8 && color[to] == EMPTY) return true;
            else if (rank8(from) == 6 && j == 16 && color[to + 8] == EMPTY && color[to] == EMPTY) return true;
            else if (color[to] == BLACK && rank8(from) == rank8(to) + 1 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1)) return true;
            else if (rank8(from) == 3 && rank8(to) == 2 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1) && ep == to)return true;
        }

        else if (color[from] == BLACK)
        {
            if (j == -8 && color[to] == EMPTY) return true;
            else if (rank8(from) == 1 && j == -16 && color[to - 8] == EMPTY && color[to] == EMPTY) return true;
            else if (color[to] == WHITE && rank8(from) == rank8(to) - 1 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1)) return true;
            else if (rank8(from) == 4 && rank8(to) == 5 && (from % 8 == to % 8 + 1 || from % 8 == to % 8 - 1) && ep == to) return true;
        }
    }

    else if (piece[from] == KNIGHT)
    {
        if (((abs(rank8(from) - rank8(to)) == 1) && (abs(from % 8 - to % 8) == 2)) ||
            ((abs(rank8(from) - rank8(to)) == 2) && (abs(from % 8 - to % 8) == 1))) return true;
    }

    else if (piece[from] == BISHOP)
    {
        if ((rank8(from) + from % 8) == (rank8(to) + to % 8))
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

        if ((rank8(from) - from % 8) == (rank8(to) - to % 8))
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
    int current = 0, p1, p2, x, j, index;
    for (int i = 0; i < 64; i++)
    {
        if (color[i] == side)
        {
            genpossible(i);
            for (int k = 0; k < totaloffs; k++)
            {
                j = i + offset[k];
                if (color[j] != side)
                {
                    bool legal = play(i, j);
                    if (legal)
                    {
                        takeback();

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

    if (current > 0 && pv[ply] != -1)
    {
        p1 = pv[ply] / 100;
        p2 = pv[ply] % 100;
        //printf("\n%d", ply); square(p1); square(p2);
        //print();
        index = -1;
        for (int i = 0; i < current; i++)
        {
            if (possible[i][0] == p1 && possible[i][1] == p2) {index = i; break;}
        }
        if (index != -1 && play(p1, p2))
        {
            pvcount++;
            possible[index][0] = possible[current-1][0];
            possible[index][1] = possible[current-1][1];
            possible[current-1][0] = p1;
            possible[current-1][1] = p2;
            current--;
            takeback();
        }
        /*else
        {
            printf("\n%d %d ", ply, current); square(p1); square(p2);
            print();
            for (int i = 0; i < current; i++)
            {
                square(possible[i][0]);
                square(possible[i][1]);
                printf(" ");
            }
        }*/
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

    int captures = 0, from, to, p3, p4;

    for (int i = 0; i < current; i++)
    {
        p1 = possible[i][1];
        if (points(p1) > 0) captures++;
    }

    current -= captures;

    for (int i = 0; i < current; i++)
    {
        for (int j = i + 1; j < current; j++)
        {
            p1 = possible[i][0];
            p2 = possible[i][1];
            p3 = possible[j][0];
            p4 = possible[j][1];

            if (history[64 * p1 + p2] > history[64 * p3 + p4])
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
    if (captdepth > 4) return position();
    captcount++;
    int list_moves[100][2], current = 0, mult = 1 - 2 * side, res = position() * mult, r, from, to, val, j;     // mult = -1 if WHITE and 1 if BLACK

    for (int i = 0; i < 64; i++)
    {
        if (color[i] == side)
        {
            genpossible(i);
            for (int k = 0; k < totaloffs; k++)
            {
                j = i + offset[k];
                if (color[j] == 1 - side)
                {
                    bool legal = play(i, j);
                    if (legal)
                    {
                        takeback();

                        list_moves[current][0] = i;
                        list_moves[current][1] = j;
                        current++;
                    }
                }
            }
        }
    }

    if (current == 0) return res;
    current--;
    to = list_moves[current][1];

    while (current >= 0)
    {
        to = list_moves[current][1];
        if (color[to] == (1-side))
        {
                from = list_moves[current][0];
                bool legal = play(from, to);
                if(legal)
                {
                    captdepth++;
                    r = -capturing(1-side);
                    captdepth--;
                    takeback();
                }

            if (r > res) res = r;
        }

        current--;
    }

    return res;
}

void takeback ()
{
    ply--;
    if (ply < 0 || ply > 500) printf("WHAAAAAAAAAAAAAAAAAAAAAT");
    int from = from_last[ply], to = to_last[ply];
    color[from] = color[to];
    piece[from] = piece_from[ply];
    color[to] = color_to[ply];
    piece[to] = piece_to[ply];
    ep = ep_before[ply];
    fifty = fifty_before[ply];
    white_long_castle = castling_before[ply] / 1000;
    white_short_castle = (castling_before[ply] / 100) % 10;
    black_long_castle = (castling_before[ply] / 10) % 10;
    black_short_castle = castling_before[ply] % 10;
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

    if (piece[from] == KING)
    {
        if (color[from] == WHITE && from == 60)
        {
            if (to == 62)
            {
                color[63] = WHITE;
                piece[63] = ROOK;
                color[61] = EMPTY;
                piece[61] = EMPTY;
            }
            else if (to == 58)
            {
                color[56] = WHITE;
                piece[56] = ROOK;
                color[59] = EMPTY;
                piece[59] = EMPTY;
            }
        }
        else if (color[from] == BLACK && from == 4)
        {
            if (to == 6)
            {
                color[7] = BLACK;
                piece[7] = ROOK;
                color[5] = EMPTY;
                piece[5] = EMPTY;
            }
            else if (to == 2)
            {
                color[0] = BLACK;
                piece[0] = ROOK;
                color[3] = EMPTY;
                piece[3] = EMPTY;
            }
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
            if (color[x] == 1 - color[coo] || x == ep)
            {
                offset[totaloffs] = x - coo;
                totaloffs++;
            }
            x = coo + 8 * side - 1;
            if (color[x] == 1 - color[coo] || x == ep)
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
                if (i != 0)
                {
                    offset[totaloffs] = i;
                    totaloffs++;
                }
            }
            for (int i = -rank8(coo); i < 8 - rank8(coo); i++)
            {
                if (i != 0)
                {
                    offset[totaloffs] = 8 * i;
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
                if (i != 0)
                {
                    offset[totaloffs] = i;
                    totaloffs++;
                }
            }
            for (int i = -rank8(coo); i < 8 - rank8(coo); i++)
            {
                if (i != 0)
                {
                    offset[totaloffs] = 8 * i;
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
            if (color[coo] == WHITE && coo == 60)
            {
                offset[totaloffs] = -2;
                offset[totaloffs + 1] = 2;
                totaloffs += 2;
            }
            if (color[coo] == BLACK && coo == 4)
            {
                offset[totaloffs] = -2;
                offset[totaloffs + 1] = 2;
                totaloffs += 2;
            }
            break;

        default: totaloffs = 0; break;
    }
}

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
    printf("%c%d ", ('a' + coo % 8), 8 - coo / 8);
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
    else if (result == 0) printf("Draw!");
    else if (result == -1) printf("White is checkmated!");
}
