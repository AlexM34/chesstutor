#include <stdio.h>
#include <string.h>
#include "defs.h"
#include <stdlib.h>
#include <time.h>
#include "data.h"
#include "protos.h"

int random()
{
    srand(time(NULL));
    int from, to, trials = 0, ep_before = ep;
    bool legal = false;

    while (!legal && trials <= 10000)
    {
        from = rand() % 64;
        to = rand() % 64;
        legal = play(from, to);
        trials++;
    }

    if (ep_before == ep) ep = -1;
    if (!legal) return -1;
    else return from * 100 + to;
}

/*int greedy(int depth, int side)
{
    if (depth == 0) return position();

    if (depth == 5) alpha = (2 * side - 1) * 1000;

    int best_from, best_to, eval = (2 * side - 1) * 1000, from, to, current = 0, ccolor[64], cpiece[64], value;
    bool legal;
    legalmoves(side);

    int list_moves[100][2];

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        current++;
    }

    if (current == 0) return (2 * side - 1) * 1000;
    current--;

    while (current >= 0)
    {
        for (int i = 0; i < 64; i++)
        {
            ccolor[i] = color[i];
            cpiece[i] = piece[i];
        }

        from = list_moves[current][0];
        to = list_moves[current][1];
        legal = play(from, to, side);

        if (legal)
        {
            value = greedy(depth - 1, 1 - side);

            if (value == -999) break;

            if (side == WHITE && value > eval)
            {
                eval = value;
                best_from = from;
                best_to = to;
            }

            if (side == BLACK && value < eval)
            {
                eval = value;
                best_from = from;
                best_to = to;
            }

            for (int i = 0; i < 64; i++)
            {
                color[i] = ccolor[i];
                piece[i] = cpiece[i];
            }
        }

        if (depth == 0)
        {
            int x = position();
            if (alpha < x) alpha = x;
            return x;
        }
        else
        {
            if (alpha > eval && depth % 2 == 0) return -999;
        }

        current--;
    }

    if (depth != 5)
    {
        return eval;
    }

    else
    {
        return 100 * best_from + best_to;
    }
}*/

int think (int depth)
{
    int best;
    for (int i = 1; i <= depth; i++)
    {
        //for (int i = 0; i < 100; i++) dcount[i] = 0;

        nodes = 0;
        best = alphaBeta(-10000, 10000, i);
        printf("nodes for %d are %d\n", i, nodes);
    }

    return best;
}

int alphaBeta(int alpha, int beta, int depth)
{
    dcount[depth]++;
    if (alpha > beta) return alpha;
    int list_moves[100][2], from, to, current = 0, score, best = 0, index;
    bool legal;
    legalmoves(sideToMove);

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        if (depth == max_depth)
        {
            square(list_moves[current][0]);
            square(list_moves[current][1]);
            printf("%d- ", history[64 * list_moves[current][0] + list_moves[current][1]]);
        }
        current++;
        //printf("\n");
    }
    if (depth == max_depth)
    {
        //for (int i = 0; i <= ply; i++) printf("%d. %d ", i, pv[i]);
    }
    if (current == 0)
    {
        if (depth != max_depth) return -9101;
        else
        {
            if (checked(sideToMove)) return 2 * sideToMove - 1;
            else return 0;
        }
    }
    current--;
    best = 100 * list_moves[current][0] + list_moves[current][1];

    if (depth == 0) return capturing(sideToMove);

    while (current >= 0 && alpha < beta)
    {
        from = list_moves[current][0];
        to = list_moves[current][1];
        last[ply] = 100 * from + to;
        legal = play(from, to);
        nodes++;

        if (!legal) {print(); printf("FALSEEEEEEEEEE\n"); square(from); square(to);}
        if (legal)
        {
            score = hasheval[hashing];
            index = hashing;

            if (score == -1 || hasheval2[hashing2] != score || hashdepth[index] < depth)
            {
                score = -alphaBeta(-beta, -alpha, depth-1);
                hasheval[index] = score;
                hasheval2[hashing2] = score;
                hashdepth[index] = depth;
                if (score <= alpha)
                    {hashtype[index] = 1;
                    t1++;}
                else if (score >= beta)
                    {hashtype[index] = 3;
                    t3++;}
                else
                    {hashtype[index] = 2;
                    t2++;}
            }
            else
            {
                hits++;
                switch (hashtype[index])
                {
                    case 1: if (alpha < hasheval[index]) {alpha = hasheval[index];} score = -alphaBeta(-beta, -alpha, depth-1); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) {beta = hasheval[index];} score = -alphaBeta(-beta, -alpha, depth-1); break;
                }
            }
            if (depth == max_depth)
            {
                square(from);
                square(to);
                printf("%d\n", (2 * sideToMove - 1) * score);
            }

            takeback();

            if (score > beta)
            {
                if (color[to] == EMPTY) history[64 * from + to] += depth * depth;
                if (depth != max_depth) return beta;
            }
            else if (score > alpha)
            {
                alpha = score;
                best = 100*from + to;
            }
        }
        current--;
    }

    //if (ply == 0 || depth > 8 || pv[ply-1] == last[depth+1] || last[depth+1] == -1 || pv[ply] == -1)
    for (int i = 0; i < ply; i++) pv[i] = last[i];
    pv[ply] = best;
    //printf("%d. %d\n", ply, best);

    if (depth != max_depth) return alpha;
    else return best;
}
