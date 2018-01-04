#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "defs.h"
#include "data.h"
#include "protos.h"

FILE *book_file;

void open_book()
{
	srand(time(NULL));
	book_file = fopen("C:\\Users\\A.Monev\\Desktop\\Chess Project\\book.txt", "r");
	if (!book_file) printf("Opening book missing.\n");
}

void close_book()
{
	if (book_file) fclose(book_file);
	book_file = NULL;
}

int book_move()
{
    legalmoves(sideToMove);
	char line[256];
	char book_line[256];
	int moves[50] = {};
	int counts[50] = {};
	int pos = 0;
	int total_count = 0;

	if (!book_file || ply > 25) return -1;

	line[0] = '\0';
	int j = 0, m;
	for (int i = 0; i < ply; i++) j += sprintf(line + j, "%s ", notation(last[i]));

	fseek(book_file, 0, SEEK_SET);
	while (fgets(book_line, 256, book_file))
    {
		if (book_match(line, book_line))
		{
			m = parse(&book_line[strlen(line)]);
			if (m == 1) m = last[ply-1];
			else m = -1;
			if (m == -1) continue;

			for (j = 0; j < pos; j++)
            {
                if (moves[j] == m)
                {
					counts[j]++;
					break;
				}
            }

			if (j == pos)
            {
				moves[pos] = m;
				counts[pos] = 1;
				pos++;
			}
			total_count++;
			takeback();
		}
	}

	if (pos == 0) return -1;
	j = rand() % total_count;

	for (int i = 0; i < pos; i++)
    {
		j -= counts[i];
		if (j < 0) return moves[i];
	}

	return -1;
}

bool book_match(char *s1, char *s2)
{
	for (int i = 0; i < (signed int)strlen(s1); i++)
    {
        if (s2[i] == '\0' || s2[i] != s1[i]) return false;
    }
	return true;
}
