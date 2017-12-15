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
    int computer = WHITE;
    char input[64];
    initialise();
    print();

    while (true)
    {
        max_depth = 9;
        int square = alphaBetaMax(-1000, 1000, max_depth);
        printf("square %d\n", square);

        if (square == -1)
        {
            finish(square);
            return 0;
        }

        int from = square / 100;
        int to = square % 100;

        bool legal = play(from, to, WHITE);
        char comp_move[4];
        comp_move[0] = (char) (from % 8 + 'a');
        comp_move[1] = (char) ((8 - from / 8) + '0');
        comp_move[2] = (char) (to % 8 + 'a');
        comp_move[3] = (char) ((8 - to / 8) + '0');

        printf("Computer's move: %c%c%c%c %d\n", comp_move[0], comp_move[1], comp_move[2], comp_move[3], position());
        printf("play %d, valid %d, legal %d, capt %d\n", playcount, validcount, legalcount, captcount);
        playcount = 0, validcount = 0, legalcount = 0, captcount = 0;
        print();

        int m = -1;
        while (m != 1)
        {
            printf("Your move: ");
            scanf("%64s", input);
            m = parse(input);
            if (m == -2) printf("The evaluation is %d\n", position());
            if (m == -3) printf("The capturing is %d\n", capturing(BLACK));
        }

        print();
    }

    return 0;
}

int parse(char input[64])
{
    if (input[0] == 'p') return -2;
    if (input[0] == 'x') return -3;

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

    if (!play(from, to, BLACK)) return -1;
    else if (piece_from != piece[to])
    {
        if (input[4] == 'n') piece[to] = KNIGHT;
        else if (input[4] == 'b') piece[to] = BISHOP;
        else if (input[4] == 'r') piece[to] = ROOK;
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
