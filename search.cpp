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
} */

/*class HashEntry
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
};*/

int think (int depth)
{
    int best;
    for (int i = 0; i <= depth; i++) best = alphaBeta(-10000, 10000, i);
    return best;
}

int alphaBeta(int alpha, int beta, int depth)
{
    if (alpha > beta) return alpha;
    int list_moves[100][2], from, to, current = 0, c, p, score, best = 0, orig_piece, ep_before, index;
    bool legal;
    //HashMap h1 = new HashMap{};
    legalmoves(sideToMove);

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        if (depth == max_depth)
        {
            printf("%d %d %d- ", list_moves[current][0], list_moves[current][1], points(list_moves[current][1]));
        }
        current++;
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

    if (depth == 0) return capturing(sideToMove) * (1 - 2 * sideToMove);

    while (current >= 0)
    {
        from = list_moves[current][0];
        to = list_moves[current][1];
        c = color[to];
        p = piece[to];
        orig_piece = piece[from];
        ep_before = ep;
        legal = play(from, to);

        if(!legal) {print(); printf("FALSEEEEEEEEEE\n"); square(from); square(to);}
        if (legal)
        {
            score = hasheval[hashing];//analyzed(hashing, depth);
            index = hashing;

            if (score == -1 || hasheval2[hashing2] != score)
            {
                score = -alphaBeta(-beta, -alpha, depth-1);
                //hashes[index] = hashing;
                hasheval[index] = score;
                hasheval2[hashing2] = score;
                hashdepth[index] = depth;
                if (score <= alpha) hashtype[index] = 1;
                else if (score >= beta) hashtype[index] = 3;
                else hashtype[index] = 2;
                //hashcount++;
                //h1.put(hashing, score);
                //printf("%d %d\n", hashing, score);
            }
            else
            {
                switch (hashtype[index])
                {
                    case 1: if (alpha < hasheval[index]) alpha = hasheval[index]; score = -alphaBeta(-beta, -alpha, depth-1); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) beta = hasheval[index]; score = -alphaBeta(-beta, -alpha, depth-1); break;
                }
            }

            /*if (index == -1)
            {
                score = -alphaBeta(-beta, -alpha, depth-1);
                hashes[hashcount] = hashing;
                hasheval[hashcount] = score;
                hashdepth[hashcount] = depth;
                if (score <= alpha) hashtype[hashcount] = 1;
                else if (score >= beta) hashtype[hashcount] = 3;
                else hashtype[hashcount] = 2;
                hashcount++;
                //h1.put(hashing, score);
                printf("%d %d\n", hashing, score);
            }
            else
            {
                switch (hashtype[index])
                {
                    case 1: if (alpha < hasheval[index]) alpha = hasheval[index]; score = -alphaBeta(-beta, -alpha, depth-1); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) beta = hasheval[index]; score = -alphaBeta(-beta, -alpha, depth-1); break;
                }
            }*/

            takeback(from, to, orig_piece, c, p, 0, ep_before);

            if (score >= beta)
            {
                history[100 * from + to] += depth * depth;
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

    if (depth != max_depth) return alpha;
    else return best;
}

int alphaBetaMax(int alpha, int beta, int depth)
{
    //printf("maxiiiiii ALPHA and BETA are %d %d with depth %d\n", alpha, beta, depth);
    if (alpha > beta) printf("IT'S CLOBERRING TIMEEEEEEEEEEEE!");
    int list_moves[100][2], from, to, current = 0, c, p, score, best = 0, orig_piece, ep_before, index;
    bool legal;

    legalmoves(WHITE);

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        if (depth == max_depth)
        {
            printf("%d %d %d- ", list_moves[current][0], list_moves[current][1], points(list_moves[current][1]));
        }
        current++;
    }

    if (current == 0)
    {
        if (depth != max_depth) return -9111;
        else return -1;
    }
    current--;
    best = 100 * list_moves[current][0] + list_moves[current][1];

    if (depth == 0) return position();

    while (current >= 0)
    {
        from = list_moves[current][0];
        to = list_moves[current][1];
        c = color[to];
        p = piece[to];
        orig_piece = piece[from];
        ep_before = ep;
        legal = play(from, to);
        //if (depth == 5) printf("move %d %d\n", from, to);

        if(!legal) {print(); printf("FALSE WHIIIIIIIIIIIIIIITE\n"); square(from); square(to);}
        if (legal)
        {
            /*int cac[64], cap[64];
            for (int i = 0; i < 64; i++)
            {
                cac[i] = color[i];
                cap[i] = piece[i];
            }
            int capt = capturing(BLACK, alpha);
            bool same = true;
            for (int i = 0; i < 64; i++)
            {
                same = same && (cac[i] == color[i]) && (cap[i] == piece[i]);
            }

            if (!same) printf("new position\n");
            //else printf("no changeeeeeeeeeee\n");
            //printf("THE transFormation izzzzzz %d and the capturing is %d\n", same, capt);
            if (capt < alpha)
            {
                color[from] = color[to];
                piece[from] = orig_piece;
                color[to] = c;
                piece[to] = p;
                goto tryagain;
                //if (depth != 5) return alpha;
                //else {printf("THE MOVE IS 34444444444444 %d \n", best); return best;}
            }*/

            /*int capt = capturing(BLACK);
            printf("%d", capt);
            if (capt < alpha)
            {
                takeback(from, to, orig_piece, c, p, 0, 0);
                goto tryagain;
                //if (depth != 5) return alpha;
                //else {printf("THE MOVE IS 34444444444444 %d \n", best); return best;}
            }*/

            //if ((to == 36 && orig_piece == PAWN && c == BLACK) || (from == 36 && orig_piece == KNIGHT)) {printf("%d and %d\, depth %d\n", from, to, depth); print();}
            //if (from == 36 && to == 21) printf("AWEEEEEEEEEEEEEEESOOOOOOOOMEEEEEE\n");

            //printf("where amazing happens %d %d %d\n", from, to, depth);
            //if (from == 38 && to == 30 && depth == 1) print();
            //index = abs(hashing);
            //printf("%d\n", index);
            index = analyzed(hashing, depth);
            if (index == -1)
            {
                score = alphaBetaMin(alpha, beta, depth-1);
                //hashes[hashcount] = hashing;
                hasheval[hashcount] = score;
                if (score <= alpha) hashtype[hashcount] = 1;
                if (score >= beta) hashtype[hashcount] = 3;
                else hashtype[index] = 2;
                hashcount++;
                //printf("%d %d\n", hashing, score);
            }
            else
            {
                switch (hashtype[index])
                {
                    case 1: if (alpha < hasheval[index]) alpha = hasheval[index]; score = alphaBetaMin(alpha, beta, depth-1); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) beta = hasheval[index]; score = alphaBetaMin(alpha, beta, depth-1); break;
                }
            }
            //if (from == 38 && to == 30 && depth == 1) printf("just bring it %d\n", score);
            //if (from == 36 && to == 21) {printf("score is %d and depth is %d\n", score, depth); print();}
            takeback(from, to, orig_piece, c, p, 0, ep_before);
            //printf("KANE for PRESIDENT %d!!!!!\n", depth);

            if (score >= beta)
            {
                //printf("ALPHA grdsgtrv %d %d\n", score, beta);
                if (depth != max_depth) return beta;
                //else return best;
            }

            else if (score > alpha)
            {
                alpha = score;
                if (depth == max_depth)
                {
                    best = 100*from + to;
                    //printf("ALPHA high score %d %d\n", alpha, beta);
                }
            }
        }

        //printf("WE'R HERE %d %d %d\n", depth, from, to);
        //if (depth == 1 && from == 54 && to == 46) print();
        //tryagain:
        current--;
    }

    //printf("time to return now\n");
    if (depth != max_depth) return alpha;
    else return best;
}

int alphaBetaMin(int alpha, int beta, int depth)
{
    //printf("minnnni ALPHA and BETA are %d %d with depth %d\n", alpha, beta, depth);
    //if (depth == 2) print();
    if (alpha > beta) printf("JOHN CENA SUUUUUUUUCKSSSS!");
    int list_moves[100][2], from, to, current = 0, c, p, score, orig_piece, ep_before, best = 0, index;
    bool legal;
    legalmoves(BLACK);

    while (possible[current][0] != -1)
    {
        list_moves[current][0] = possible[current][0];
        list_moves[current][1] = possible[current][1];
        current++;
    }

    if (current == 0)
    {
        if (depth != max_depth) return 9010;
        return 1;
    }
    current--;

    if (depth == 0) return position();
            //if (color[to] == WHITE) printf("let's evaluate now");
            //return capturing(BLACK);}

    while (current >= 0)
    {
        //printf("Cena sucks\n");
        from = list_moves[current][0];
        to = list_moves[current][1];
        c = color[to];
        p = piece[to];
        orig_piece = piece[from];
        ep_before = ep;
        legal = play(from, to);

        //printf("%d\n",legal);
        if(!legal) {print(); printf("FALSEEEEEEEEEEEEEEEEEEE BLACK\n"); square(from); square(to);}
        if (legal)
        {
            /*int capt = capturing(WHITE);
            if (capt > beta)
            {
                takeback(from, to, orig_piece, c, p, 0, 0);
                goto tryagain;
                //if (depth != 5) return alpha;
                //else {printf("THE MOVE IS 34444444444444 %d \n", best); return best;}
            }*/
            //if (ep != -1) {printf("%d %d \n", from, to); print();}
            index = analyzed(hashing, depth);
            if (index == -1)
            {
                score = alphaBetaMax(alpha, beta, depth-1);
                //hashes[hashcount] = hashing;
                hasheval[hashcount] = score;
                if (score <= alpha) hashtype[hashcount] = 1;
                if (score >= beta) hashtype[hashcount] = 3;
                else hashtype[hashcount] = 2;
                hashcount++;
                //printf("%d %d\n", hashing, score);
            }
            else
            {
                switch (hashtype[index])
                {
                    case 1: if (alpha < hasheval[index]) alpha = hasheval[index]; score = alphaBetaMin(alpha, beta, depth-1); break;
                    case 2: score = hasheval[index]; break;
                    case 3: if (beta > hasheval[index]) beta = hasheval[index]; score = alphaBetaMin(alpha, beta, depth-1); break;
                }
            }

            takeback(from, to, orig_piece, c, p, 0, ep_before);

            if (score <= alpha) return alpha;
            else if (score < beta)
            {
                beta = score;
                if (depth == max_depth)
                {
                    best = 100*from + to;
                    //printf("ALPHA high score %d %d\n", alpha, beta);
                }
                //if (depth == 4) printf("The new value of beta is %d %d \n", beta, alpha);
            }
        }

        //tryagain:
        current--;
    }

    //printf("Wooooooooooooooow\n");
    if (depth != max_depth) return beta;
    else return best;
}

/* int greedy(int depth, int side)
{
    int best_from, best_to, eval =  (2 * side - 1)* 1000, trials = 100, from, to, current, ccolor[64], cpiece[64];
    bool possible = false;
    if (depth == 0) return position();

    while (trials > 0)
    {
        for (int i = 0; i < 64; i++)
        {
            ccolor[i] = color[i];
            cpiece[i] = piece[i];
        }

        while (!possible)
        {
               from = rand() % 64;
               to = rand() % 64;
               possible = play(from, to, side);
        }

        current = greedy(depth - 1, 1 - side);
        if (side == WHITE && current > eval)
        {
            eval = current;
            best_from = from;
            best_to = to;
        }

        if (side == BLACK && current < eval)
        {
            eval = current;
            best_from = from;
            best_to = to;
        }

        for (int i = 0; i < 64; i++)
        {
            color[i] = ccolor[i];
            piece[i] = cpiece[i];
        }

        trials--;
    }

    if (depth != MAX_PLY)
    {
        return eval;
    }

    else return 100 * best_from + best_to;
} */
