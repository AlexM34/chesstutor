#include <stdio.h>
#include <string.h>
#include "defs.h"
#include <stdlib.h>
#include <time.h>
#include "data.h"
#include "protos.h"

void thinkingTime ()
{
    double m = ply / 2, total = max_time, spent, percentage;
    switch ((int) m / 5)
    {
            case 0: percentage = 1 + m / 5; break;
            case 1: percentage = 2 + (m - 5) / 5; break;
            case 2: percentage = 3 + (m - 10) / 1.5; break;
            case 3: percentage = 6 + (m - 15) / 1.2; break;
            case 4: percentage = 10 + (m - 20) / 0.8; break;
            case 5: percentage = 16 + (m - 25) / 0.5; break;
            case 6: percentage = 28 - (m - 30) / 0.8; break;
            case 7: percentage = 22 - (m - 35) / 1.2; break;
            case 8: percentage = 18 - (m - 40) / 1.5; break;
            default: percentage = 15; break;
    }

    percentage += increment / max_time;
    spent = total * percentage / 100;
    if (m > 20 && spent < increment && max_time > spent + increment) spent += increment;
    start_time = clock();
    stop_time = start_time + spent;
    //printf("%d. %fs or %f percent spent from %fs remaining\n", (int) m + 1, spent / 1000, 100 * spent / (600000 + 60 * increment), (max_time - spent + increment) / 1000);
}

int think (int depth)
{
    int best = book_move();
    if (best != -1) return best;
    for (int i = 1; i <= depth; i++)
    {
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
    if (alpha > beta) return alpha;
    if (clock() > stop_time)
    {
        distToRoot--;
        zeitnot = true;
        if (distToRoot > 0) return -11111;
        else return pv[ply];
    }
    distToRoot++;
    int list_moves[100][3], from, to, current = 0, score, best = 0, index, iprom = 0, pr, next = depth - 1, caps = 0;
    bool legal, incheck = false;
    legalmoves(sideToMove);

    if (checked(sideToMove) && ply - moves < 10)
    {
        next++;
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

        current++;
    }
    if (current == 0)
    {
        distToRoot--;
        if (!checked(sideToMove)) return 0;
        if (depth == max_depth && distToRoot == 0) return 12345 * (2 * sideToMove - 1);
        else return -8101-100*depth;
    }
    current--;
    best = 100 * list_moves[current][0] + list_moves[current][1];

    if (next == -1) {distToRoot--; return capturing(sideToMove);}

    while (current >= 0 && alpha < beta)
    {
        caps = 0;
        from = list_moves[current][0];
        to = list_moves[current][1];
        last[ply] = 100 * from + to;
        promotion = list_moves[current][2];
        //if (color[to] == 1 - sideToMove && next < depth && ply - moves < 10) caps++;
        legal = play(from, to);
        nodes++;

        if (!legal) {print(); printf("FALSEEEEEEEEEE\n"); square(from); square(to);}
        if (legal)
        {
            index = analyzed(hashing);

            if (index == -1 || hashdepth[index] < depth)
            {
                score = -alphaBeta(-beta, -alpha, next+caps);
                if (!zeitnot)
                {
                    if (index == -1)
                    {
                        index = hashcount;
                        hashcount++;
                    }
                    hashes[index] = hashing;
                    hasheval[index] = score * (2 * sideToMove - 1);
                    hashdepth[index] = depth;
                    if (score <= alpha) {hashtype[index] = 1; t1++;}
                    else if (score >= beta) {hashtype[index] = 3; t3++;}
                    else {hashtype[index] = 2; t2++;}
                }
            }
            else
            {
                hits++;
                switch (hashtype[index])
                {
                    case 1: if (alpha < hasheval[index]) alpha = hasheval[index];
                            score = -alphaBeta(-beta, -alpha, next+caps); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) beta = hasheval[index];
                            score = -alphaBeta(-beta, -alpha, next+caps); break;
                }
            }
            if (distToRoot == 1 && depth == max_depth)
            {
                square(from);
                square(to);
                printf("%d\n", (2 * sideToMove - 1) * score);
            }

            takeback();
            if (zeitnot) break;

            if (score > beta)
            {
                if (color[to] == EMPTY) history[64 * from + to] += depth * depth;
                if (depth != max_depth || distToRoot > 1) {distToRoot--; return beta;}
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

    for (int i = 0; i < ply; i++) pv[i] = last[i];
    pv[ply] = best;
    distToRoot--;

    if (distToRoot > 0) return alpha;
    else
    {
        promotion = pr;
        evaluation = (1 - 2 * sideToMove) * alpha;
        printf("%d IS BEST for depth %d\n", best, depth);
        return best;
    }
}
