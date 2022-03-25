#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

//############ Structs ###############
typedef struct neighbors {
  //there is a total of four neighbors
  int neighbor[4]; //what type of piece the neighbor is
  int relative_position[4][2]; //saves the position relative to another piece
} Neighbors;

typedef struct mctsEval {
  int total_score;
  int num_of_visits;
} MCTSEval;

typedef struct node {
  int *board_state;
  struct node *prev, *next;
} Node;

typedef struct multiMinimax_args {
  int *board;
  int player;
} MinimaxArgs;

typedef struct eatenPieces {
  int x;
  int y;
  struct eatenPieces *prev;
} EatenPieces;

//############# Functions ##############
char * returnBoard();
void _setTurn(int newTurn);
void _setMiniMaxDepth(int depth);
void initializeHistory();
void clearHistory();
int playerMove(int initial[2], int final[2]);
int noPieces(int *board);
int cantMovePieces(int *state, int player_moving);
int gameOver(int *state, int player_moving);
bool hasToEat(int *board, int team);
bool goBackInHistory();
bool goForwardInHistory();
void rewriteHistory();
int computerMove() ;
float evaluatePosition(int *board, int team, int winner);
Node *getChildNodes(int *board, int player);
float max(float prev_min, float eval);
float min(float prev_min, float eval);
float minimax(int *board, int depth, float alpha, float beta, bool maximizing_player, int maxi_player_num);
int *useMinimax(int *board, int player);
void* multiMinimax(void* _args);
void rewriteHistory();