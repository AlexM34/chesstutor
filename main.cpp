#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <windows.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include <stdio.h>
#include <unistd.h>
#include <dos.h>

int main()
{
    beginning:
    int computer, m = 0;
    char input;
    max_depth = 4;

    while (m == 0)
    {
        m = 1;
        printf("Choose your side - White (w) or Black (b): ");
        scanf(" %c", &input);
        if (input == 'w') computer = BLACK;
        else if (input == 'b') computer = WHITE;
        else m = 0;
    }

    initialise();
    init_hash();
    print();

    set_hash(sideToMove);
    path[0] = hashing;
    reps[0] = 1;
    repcount = 1;

    if (computer == WHITE)
    {
        m = compmove(computer, max_depth);
        print();
    }

    while (true)
    {
        for (int i = 0; i < 20; i++) printf("%d ", pv[i]);
        printf("\n");

        if (m == 0) break;

        m = humanmove(1 - computer);
        print();

        //for (int i = 0; i < 20; i++) printf("%d ", pv[i]);
        //printf("\n");

        if (m == 0) break;
        else if (m == 2) goto beginning;

        m = compmove(computer, max_depth);
        print();
    }

    return 0;
}

int parse(char input[64], int player)
{
    if (input[0] == 'p') return -2;
    if (input[0] == 'x') return -3;
    if (input[0] == 'n') return -4;

	if (input[0] < 'a' || input[0] > 'h' || input[1] < '0' || input[1] > '9' ||
			input[2] < 'a' || input[2] > 'h' || input[3] < '0' || input[3] > '9')
    {
	    return -1;
    }

	int from = input[0] - 'a';
	from += 8 * (8 - (input[1] - '0'));
	int to = input[2] - 'a';
	to += 8 * (8 - (input[3] - '0'));
	int piece_from = piece[from];

    if (!play(from, to)) return -1;
    else if (piece_from != piece[to])
    {
        if (input[4] == 'n') piece[to] = KNIGHT;
        else if (input[4] == 'b') piece[to] = BISHOP;
        else if (input[4] == 'r') piece[to] = ROOK;
    }

	return 1;
}

int compmove(int player, int d)
{
    checks = 0;
    distToRoot = 0;
    for (int i = 0; i < 100; i++) dcount[i] = 0;
    int square = think(d);
    printf("square %d\n", square);

    for (int i = 0; i < repcount; i++)
    {
        printf("%d. %d %d\n", i, path[i], reps[i]);
    }

    if (square == -12345 || square == 12345 || square == 0)
    {
        finish(square / 12345);
        return 0;
    }

    if (ply - fifty >= 100)
    {
        finish(0);
        return 0;
    }

    int from = square / 100;
    int to = square % 100;

    bool legal = play(from, to);
    if (!legal) printf("FALSEEEEEE\n");
    char comp_move[4];
    comp_move[0] = (char) (from % 8 + 'a');
    comp_move[1] = (char) ((8 - from / 8) + '0');
    comp_move[2] = (char) (to % 8 + 'a');
    comp_move[3] = (char) ((8 - to / 8) + '0');

    printf("Computer's move: %d. %c%c%c%c %d %d\n", ply, comp_move[0], comp_move[1], comp_move[2], comp_move[3], position(), hashcount);
    printf("play %d, valid %d, legal %d, capt %d\n", playcount, validcount, legalcount, captcount);
    playcount = 0, validcount = 0, legalcount = 0, captcount = 0;

    for (int i = 0; i < 10; i++) printf("%d. %d\n", i, dcount[i]);
    for (int i = 0; i < 20; i++)
    {
        printf("%d ", last[i]);
        //printf("%d. %d %d %d %d %d %d %d\n", i, from_last[i], to_last[i], color_to[i], piece_to[i], piece_from[i], castling_before[i], ep_before[i]);
    }
    printf("\nPVcount: %d\n", pvcount);
    printf("hits: %d\n 1. %d 2. %d 3. %d\n", hits, t1, t2, t3);
    printf("The evaluation is %d\n", evaluation);
    pvcount = 0;
    hits = 0;
    return 1;
}

int humanmove (int player)
{
    legalmoves(player);
    int m = -1;
    char input[64];
    if (possible[0][1] == -1)
    {
        if (checked(player)) finish(1);
        else finish(0);
        return 0;
    }

    if (ply - fifty >= 100)
    {
        finish(0);
        return 0;
    }

    while (m != 1)
    {
        printf("Your move: ");
        scanf("%64s", input);
        m = parse(input, player);
        if (m == -2) printf("The evaluation is %d\n", position());
        if (m == -3) printf("The capturing is %d\n", capturing(player));
        if (m == -4) return 2;
    }

    return 1;
}

void print()
{
	printf("\n8 ");
	for (int i = 0; i < 64; i++)
    {
		if (color[i] == EMPTY) printf(" .");

		else if (color[i] == WHITE) printf(" %c", piece_letter[piece[i]]);

		else printf(" %c", piece_letter[piece[i]] + ('a' - 'A'));

		if ((i + 1) % 8 == 0 && i != 63) printf("\n%d ", 7 - i / 8);
	}
	printf("\n\n   a b c d e f g h\n\n");
}
