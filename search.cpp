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
        best = alphaBeta(-100000, 100000, i);
        printf("nodes for %d are %d\n", i, nodes);
    }

    return best;
}

int alphaBeta(int alpha, int beta, int depth)
{
    if (threefold) return 0;
    if (fifty - ply >= 100) return 0;
    dcount[depth]++;
    distToRoot++;
    if (alpha > beta) return alpha;
    int list_moves[100][3], from, to, current = 0, score, best = 0, index, iprom = 0, pr, next = depth - 1;
    bool legal, incheck = false;
    legalmoves(sideToMove);

    if (checked(sideToMove))
    {
        next++;
        checks++;
        incheck = true;
    }

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        list_moves[current][2] = QUEEN;
        if (rank8(possible[current][1]) == 7 * sideToMove && piece[possible[current][0]] == PAWN)
        {
            switch (iprom % 4)
            {
                case 0: list_moves[current][2] = QUEEN; break;
                case 1: list_moves[current][2] = ROOK; break;
                case 2: list_moves[current][2] = BISHOP; break;
                case 3: list_moves[current][2] = KNIGHT; break;
            }
            iprom++;
        }
        if (depth - checks == max_depth)
        {
            square(list_moves[current][0]);
            square(list_moves[current][1]);
            printf("%d %d- ", list_moves[current][2], history[64 * list_moves[current][0] + list_moves[current][1]]);
        }
        current++;
        //printf("\n");
    }
    if (current == 0)
    {
        distToRoot--;
        printf("%d %d %d\n", incheck, checks, depth);
        if (!incheck) return 0;
        if (depth == max_depth && (checks == 0 || (checks == 1 && incheck))) return 12345 * (2 * sideToMove - 1);
        else return -8101-100*depth;
    }
    current--;
    best = 100 * list_moves[current][0] + list_moves[current][1];

    if (next == -1) {distToRoot--; return capturing(sideToMove);}

    while (current >= 0 && alpha < beta)
    {
        from = list_moves[current][0];
        to = list_moves[current][1];
        last[ply] = 100 * from + to;
        promotion = list_moves[current][2];
        legal = play(from, to);
        nodes++;

        if (!legal) {print(); printf("FALSEEEEEEEEEE\n"); square(from); square(to);}
        if (legal)
        {
            score = hasheval[hashing];
            index = hashing;

            if (score == -1 || hasheval2[hashing2] != score || hashdepth[index] < depth)
            {
                score = -alphaBeta(-beta, -alpha, next);
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
                    case 1: if (alpha < hasheval[index]) {alpha = hasheval[index];} score = -alphaBeta(-beta, -alpha, next); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) {beta = hasheval[index];} score = -alphaBeta(-beta, -alpha, next); break;
                }
            }
            if (distToRoot == 1 && depth == max_depth)
            {
                square(from);
                square(to);
                printf("%d\n", (2 * sideToMove - 1) * score);
            }

            takeback();

            if (score > beta)
            {
                if (color[to] == EMPTY) history[64 * from + to] += depth * depth;
                if (depth != max_depth || (checks == 1 && !incheck) || checks > 1) {distToRoot--; return beta;}
            }
            else if (score > alpha)
            {
                alpha = score;
                best = 100*from + to;
                pr = list_moves[current][2];
            }
        }
        current--;
    }

    //if (ply == 0 || depth > 8 || pv[ply-1] == last[depth+1] || last[depth+1] == -1 || pv[ply] == -1)
    for (int i = 0; i < ply; i++) pv[i] = last[i];
    pv[ply] = best;
    if (incheck) checks--;
    distToRoot--;
    //printf("%d. %d\n", ply, checks);

    if (distToRoot > 0) return alpha;
    else
    {
        promotion = pr;
        evaluation = alpha;
        return best;
    }
}
