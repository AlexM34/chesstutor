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
    max_depth = 5;
    max_time = 200000;

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
    open_book();
    print();

    set_hash(sideToMove);
    path[0] = hashing;
    reps[0] = 1;
    repcount = 1;

    if (computer == WHITE)
    {
        m = compmove(computer, max_depth);
        moves++;
        print();
    }

    while (true)
    {
        for (int i = 0; i < 20; i++) printf("%d ", pv[i]);
        printf("\n");

        if (m == 0) break;

        m = humanmove(1 - computer);
        moves++;
        print();

        //for (int i = 0; i < 20; i++) printf("%d ", pv[i]);
        //printf("\n");

        if (m == 0) break;
        else if (m == 2) goto beginning;

        m = compmove(computer, max_depth);
        moves++;
        print();
    }

    close_book();
    return 0;
}

int parse(char input[64])
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

    last[ply-1] = 100 * from + to;

	return 1;
}

char* notation (int m)
{
    char* str = new char[6];
    char c;

	if (false) {
		switch (m) {
			case KNIGHT:
				c = 'n';
				break;
			case BISHOP:
				c = 'b';
				break;
			case ROOK:
				c = 'r';
				break;
			default:
				c = 'q';
				break;
		}
		sprintf(str, "%c%d%c%d%c",
				file8(m / 100) + 'a',
				8 - rank8(m / 100),
				file8(m % 100) + 'a',
				8 - rank8(m % 100),
				c);
	}
	else
    {
        sprintf(str, "%c%c%c%c",
				file8(m / 100) + 'a',
				8 - rank8(m / 100) + '0',
				file8(m % 100) + 'a',
				8 - rank8(m % 100) + '0');
    }
	return str;
}

int compmove(int player, int d)
{
    zeitnot = false;
    distToRoot = 0;
    for (int i = 0; i < 100; i++) dcount[i] = 0;
    start_time = clock();
    stop_time = start_time + max_time / 20;
    int square = think(d);
    int elapsed = clock() - start_time;
    max_time -= elapsed;

    if (max_time < 0)
    {
        finish(4 * sideToMove - 2);
        return 0;
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
    last[ply] = square;

    bool legal = play(from, to);
    if (!legal) printf("FALSEEEEEE\n");
    char comp_move[4];
    comp_move[0] = (char) (from % 8 + 'a');
    comp_move[1] = (char) ((8 - from / 8) + '0');
    comp_move[2] = (char) (to % 8 + 'a');
    comp_move[3] = (char) ((8 - to / 8) + '0');

    printf("play %d, valid %d, legal %d, capt %d\n", playcount, validcount, legalcount, captcount);
    playcount = 0, validcount = 0, legalcount = 0, captcount = 0;

    for (int i = 0; i < 10; i++) printf("%d. %d\n", i, dcount[i]);
    printf("\nPVcount: %d\n", pvcount);
    printf("hits: %d\n 1. %d 2. %d 3. %d\n", hits, t1, t2, t3);
    printf("The evaluation is %d\n", evaluation);
    pvcount = 0;
    hits = 0;
    hashcount = 0;

    printf("ELAPSED: %d, REMAINING: %d\n", elapsed, max_time);
    printf("Computer's move: %d. %c%c%c%c %d %d\n", ply, comp_move[0], comp_move[1], comp_move[2], comp_move[3], (1 - 2 * sideToMove) * capturing(sideToMove), hashcount);
    return 1;
}

int humanmove (int player)
{
    legalmoves(player);
    int m = -1;
    char input[64];
    if (possible[0][1] == -1)
    {
        if (checked(player)) finish(2 * player - 1);
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
        m = parse(input);
        if (m == -2) printf("The evaluation is %d\n", position());
        if (m == -3) printf("The capturing is %d\n", capturing(player * (1 - 2 * sideToMove)));
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
