#ifndef HEADER_FILE
#define HEADER_FILE

#include "definitions.h"

#endif

//############# Globals ###############
Node *history;
//the player who starts moving
int turn = 2;
//player number that the computer is going to play as
int computer_player = 1;
bool use_minimax = true;
int minimax_depth = 3;

//############# Functions ##############

//Returns the board as a string
char * returnBoard() {
  int *board = (*history).board_state;
  char *boardCharArray = malloc(34 * sizeof(char *));
  char line[2];
  char start[2] = "[";
  char end[2] = "]";

  strcpy(boardCharArray, start);
  for (size_t i = 0; i < 64; i++) {
    if (board[i] != 9) {
      sprintf(line, "%d", board[i]);
      strcat(boardCharArray, line);
    }
  }
  strcat(boardCharArray, end);
  return boardCharArray;
}

//changes the turn
void _setTurn(int newTurn) {
  turn = newTurn;
}

//sets the minimax depth
void _setMiniMaxDepth(int depth) {
  if (depth < 2){
    depth = 2;
  }
  minimax_depth = depth;
}

int *initializeBoard() {
  int initialBoardState[64] = {
    9,1,9,1,9,1,9,1,
    1,9,1,9,1,9,1,9,
    9,1,9,1,9,1,9,1,
    0,9,0,9,0,9,0,9,
    9,0,9,0,9,0,9,0,
    2,9,2,9,2,9,2,9,
    9,2,9,2,9,2,9,2,
    2,9,2,9,2,9,2,9
  };
  int *board = (int *)malloc(sizeof(int) * 64);
  for (size_t i = 0; i < 64; i++) {
      board[i] = initialBoardState[i];
  }
  return board;
}

void initializeHistory() {
  history = (Node *)malloc(sizeof(Node));
  (*history).prev = (*history).next = NULL;
  (*history).board_state = initializeBoard();
}

void clearHistory() {
  rewriteHistory(history);

  Node *tmp = history;
  while (history != NULL) {
    tmp = (*history).prev;
    free((*history).board_state);
    (*history).board_state = NULL;
    free(history);
    history = NULL;
    history = tmp;
  }
}

//This function makes sure that valid coordinates are passed through
// returns the number in the board on coordinate x,y
int getBoardNumber(int x, int y, int *board) {
  if (x < 1 || x > 8 || y < 1 || y > 8) {
    return 9; //this means that the edge of the board was hit
  }
  // translate x,y coordinates to 1 dimensional array coordinate
  int realPosition = ((y - 1) * 8) + (x - 1);
  return board[realPosition];
}

//sets a number in the board
//in: coordinates (x,y), int number to be set, int the board on which
//    the number is going to be set
//out: false if the number was not set, true if the nubmer was set
bool setBoardNumber(int x, int y, int number, int *board) {
  if (getBoardNumber(x, y, board) == 9) {
    return false; //this is an invalid position
  }
  //check that it is an acceptable next_number
  if (number < 0 || number > 4) {
    return false;
  }
  int realPosition = ((y - 1) * 8) + (x - 1);
  board[realPosition] = number;
  //check that the nmber was correctly set
  if (getBoardNumber(x, y, board) == number) {
    return true;
  }
  return false;
}

//This function determines if a move is valid or not
int *pathFinder(
  int x, //current coordinate x
  int y, //current coordinate y
  int final[2], //final coordinate x,y (target coordinates)
  int *board, //current board state
  int past_x, //where the piece jumped from
  int past_y, //where the piece jumped from
  int starting_piece, //type of piece that is jumping
  bool has_to_eat, //if the player is forced to eat
  EatenPieces *eaten
) {
  int current_board_num = getBoardNumber(x, y, board);
  int past_board_num = getBoardNumber(past_x, past_y, board);
  int team = (starting_piece > 2)? starting_piece - 2: starting_piece;
  int opponent = (team == 1)? 2: 1;

  //if it's a spot where a piece was already eaten
  EatenPieces *tmp = eaten;
  while (tmp != NULL) {
    if ((*tmp).x == x && (*tmp).y == y) {
      return NULL;
    }
    tmp = (*tmp).prev;
  }
  //if it's an invalid space
  if (current_board_num == 9) {
    return NULL;
  }
  //if moving to or jumping to a piece of the same team
  if ((current_board_num == team || current_board_num == team + 2) && !(x == past_x && y == past_y)) {
    return NULL;
  }
  //if it jumped two opponents pieces in a row
  if ((current_board_num == opponent || current_board_num == opponent + 2) && (past_board_num == opponent || past_board_num == opponent + 2)) {
    return NULL;
  }
  //if it jumped from a 0 to another 0
  if (current_board_num == 0 && past_board_num == 0) {
    return NULL;
  }
  //if the final position is not a free space or starting positions
  //is the same as ending position
  if (x == final[0] && y == final[1] && current_board_num != 0) {
    return NULL;
  }
  //if the player has to eat, and in this move no pieces are eaten
  if (has_to_eat && past_board_num == starting_piece && current_board_num == 0) {
    return NULL;
  }
  //if a man is trying to move backwards
  if (team == 1 && past_board_num == team && past_y > y) {
    return NULL;
  }
  if (team == 2 && past_board_num == team && past_y < y) {
    return NULL;
  }
  //if the final coordinates are reached
  if (x == final[0] && y == final[1]) {
    int *final = (int *)malloc(sizeof(int) * 64);
    for (size_t i = 0; i < 64; i++) {
      final[i] = board[i];
    }
    return final;
  }
  //if it's not a king and it reached the other side, it can't move any more
  if (team == 1 && starting_piece == team && y == 8) {
    return NULL;
  }
  if (team == 2 && starting_piece == team && y == 1) {
    return NULL;
  }
  //if it's moving from the starting position and this position is
  //no the target position
  if (current_board_num == 0 && past_board_num == starting_piece) {
    return NULL;
  }


  //+++ Move to the next location +++
  int *path = NULL;
  //if the piece has to jump
  if ((current_board_num == opponent || current_board_num == opponent + 2) && (past_board_num == starting_piece || past_board_num == 0)) {
    EatenPieces *add_eaten = (EatenPieces *)malloc(sizeof(EatenPieces));
    (*add_eaten).prev = eaten;
    (*add_eaten).x = x;
    (*add_eaten).y = y;
    path = pathFinder(x + (x - past_x), y + (y - past_y), final, board, x, y, starting_piece, has_to_eat, add_eaten);
    free(add_eaten);
  }
  else {
    if (!(past_x == x + 1 && past_y == y + 1)) {
      path = pathFinder(x + 1, y + 1, final, board, x, y, starting_piece, has_to_eat, eaten);
    }
    if (path == NULL && !(past_x == x - 1 && past_y == y + 1)) {
      path = pathFinder(x - 1, y + 1, final, board, x, y, starting_piece, has_to_eat, eaten);
    }
    if (path == NULL && !(past_x == x - 1 && past_y == y - 1)) {
      path = pathFinder(x - 1, y - 1, final, board, x, y, starting_piece, has_to_eat, eaten);
    }
    if (path == NULL && !(past_x == x + 1 && past_y == y - 1)) {
      path = pathFinder(x + 1, y - 1, final, board, x, y, starting_piece, has_to_eat, eaten);
    }
  }

  //if path is found, remove eaten pieces
  if (path != NULL) {
    setBoardNumber(x, y, 0, path);
  }
  return path;
}

//Intializes the function pathFinder
//out: final state if move is valid, else NULL
int *makeMove(int initial[], int final[], int *board, int team) {
  int starting_piece = getBoardNumber(initial[0], initial[1], board);
  bool has_to_eat = hasToEat(board, team);

  int *result = pathFinder(initial[0], initial[1], final, board, initial[0], initial[1], starting_piece, has_to_eat, NULL);
  if (result != NULL) {
    //turn piece into a king when it reaches the other side
    if ((starting_piece == 1 && final[1] == 8) || (starting_piece == 2 && final[1] == 1)) {
      starting_piece += 2;
    }
    setBoardNumber(final[0], final[1], starting_piece, result);
  }
  return result;
}

//handles moves made by the players
//in: initial and final position coordinates
//out: 0 if move was made, -1 if not, 1 if player1 won, 2 if player 2 won
//     3 if it's a draw
int playerMove(int initial[2], int final[2]) {
  rewriteHistory();
  int *result = makeMove(initial , final, (*history).board_state, turn);
  if (result != NULL) {
    (*history).next = (Node *)malloc(sizeof(Node));
    (*(*history).next).board_state = result;
    (*(*history).next).next = NULL;
    (*(*history).next).prev = history;
    history = (*history).next;
  }
  else {
    return -1;
  }
  int player_moving = turn;
  turn = (turn == 2)? 1: 2;
  return gameOver((*history).board_state, player_moving);
}

//checks if one of the player's pieces can eat
//in: int* a state of the board, int team that is moving
//out: bool true if they can eat, false if they can't
bool hasToEat(int *board, int team) {
  int opponent = (team == 1)? 2: 1;
  int number, next, next_next;

  //goes through all the pieces
  for (size_t y = 1; y < 9; y++) {
    size_t x = 1 + (y % 2); //starts from a black square
    for (; x < 9; x++) {
      number = getBoardNumber(x, y, board);
      if(number == team || number == team + 2) {
        if (number != 2) {
          next = getBoardNumber(x + 1, y + 1, board);
          next_next = getBoardNumber(x + 2, y + 2, board);
          if ((next == opponent || next == opponent + 2) && next_next == 0) {
            return true;
          }
          next = getBoardNumber(x - 1, y + 1, board);
          next_next = getBoardNumber(x - 2, y + 2, board);
          if ((next == opponent || next == opponent + 2) && next_next == 0) {
            return true;
          }
        }
        if (number != 1) {
          next = getBoardNumber(x - 1, y - 1, board);
          next_next = getBoardNumber(x - 2, y - 2, board);
          if ((next == opponent || next == opponent + 2) && next_next == 0) {
            return true;
          }
          next = getBoardNumber(x + 1, y - 1, board);
          next_next = getBoardNumber(x + 2, y - 2, board);
          if ((next == opponent || next == opponent + 2) && next_next == 0) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

//checks if players have pieces left over
//returns: 1 if player 1 has no pieces left over, 2 if player 2 has no
//         pieces, and 0 if both players have pieces
int noPieces(int *board) {
  int team1_count = 0;
  int team2_count = 0;
  size_t i, j;

  for (i = 1, j = 63; i <= j; i ++, j--) {
    if (board[i] == 1 || board[i] == 3 || board[j] == 1 || board[j] == 3) {
      team1_count += 1;
    }
    if (board[i] == 2 || board[i] == 4 || board[j] == 2 || board[j] == 4) {
      team2_count += 1;
    }
    if (team1_count > 0 && team2_count > 0) {
      break;
    }
  }
  if (team1_count == 0) {
    return 1;
  }
  if (team2_count == 0)  {
    return 2;
  }
  return 0;
}

//checks if one or both of the players can't move their pieces
//in: a board state, the player who is moving (1 or 2)
//returns: 1 if player 1 can't move, 2 if player 2 can't move, 0 if both
//         players can move, 3 if both playes can't move
int cantMovePieces(int *board, int player_moving) {
  bool player1_can_move = false;
  bool player2_can_move = false;
  int number, next, next_next, opponent, team;

  //goes through all the pieces
  for (size_t y = 1; y < 9; y++) {
    size_t x = 1 + (y % 2); //starts from a black square
    for (; x < 9; x += 2) {
      number = getBoardNumber(x, y, board);

      //skip empty or invalid places
      if (number == 0 || number == 9) {
        continue;
      }
      if (
        ((number == 1 || number == 3) && !player1_can_move) ||
        ((number == 2 || number == 4) && !player2_can_move) 
      ) {
        //get who is opponent
        team = (number > 2)? number - 2: number;
        opponent = (team == 1)? 2: 1;

        //check move forward
        if (number != 2) {
          next = getBoardNumber(x + 1, y + 1, board);
          next_next = getBoardNumber(x + 2, y + 2, board);
          //if it can move or jump
          if (((next == opponent || next == opponent + 2) && next_next == 0) || next == 0) {
            if (team == 1) {
              player1_can_move = true;
            }
            else {
              player2_can_move = true;
            }
            continue;
          }
          next = getBoardNumber(x - 1, y + 1, board);
          next_next = getBoardNumber(x - 2, y + 2, board);
          //if it can move or jump
          if (((next == opponent || next == opponent + 2) && next_next == 0) || next == 0) {
            if (team == 1) {
              player1_can_move = true;
            }
            else {
              player2_can_move = true;
            }
            continue;
          }
        }
        //check move backward
        if (number != 1) {
          next = getBoardNumber(x - 1, y - 1, board);
          next_next = getBoardNumber(x - 2, y - 2, board);
          //if it can move or jump
          if (((next == opponent || next == opponent + 2) && next_next == 0) || next == 0) {
            if (team == 1) {
              player1_can_move = true;
            }
            else {
              player2_can_move = true;
            }
            continue;
          }
          next = getBoardNumber(x + 1, y - 1, board);
          next_next = getBoardNumber(x + 2, y - 2, board);
          //if it can move or jump
          if (((next == opponent || next == opponent + 2) && next_next == 0) || next == 0) {
            if (team == 1) {
              player1_can_move = true;
            }
            else {
              player2_can_move = true;
            }
            continue;
          }
        }
      }
    }
  }
  if (!player1_can_move && player2_can_move && player_moving == 2) {
    return 1;
  }
  if (!player2_can_move && player1_can_move && player_moving == 1) {
    return 2;
  }
  if (!player1_can_move && !player2_can_move) {
    return 3;
  }
  return 0;
}

//checks if one of the players won
//in: a board state, the player who is moving (1 or 2)
//returns: 0 if no one has won, 1 if player 1 won, 2 if player 2 won,
//         and 3 if it's a draw
int gameOver(int *state, int player_moving) {
  int no_pieces = noPieces(state);
  if (no_pieces != 0) {
    return (no_pieces == 1)? 2 : 1;
  }
  int cant_move = cantMovePieces(state, player_moving);
  if (cant_move != 0) {
    switch (cant_move) {
      case 1:
        return 2;
      case 2:
        return 1;
      case 3:
        return 3;
    }
  }
  return 0;
}

//Undoes previous plays
//returns true if plays where undone, false if not
bool goBackInHistory() {
  if (history == NULL) {
    return false;
  }
  if ((*history).prev == NULL) {
    return false;
  }
  history = (*history).prev;
  turn = (turn == 2)? 1: 2;
  return true;
}

//Redoes plays in History
//returns true if plays where redone, false if not
bool goForwardInHistory() {
  if (history == NULL) {
    return false;
  }
  //if you can not go forward in history
  if ((*history).next == NULL) {
    return false;
  }
  history = (*history).next;
  turn = (turn == 2)? 1: 2;
  return true;
}

//makes the current position of the history pointer the last position,
//by deleting the rest of the history
void rewriteHistory() {
  if ((*history).next != NULL) {
    Node *next;
    Node *temp = (*history).next;
    do {
      next = (*temp).next;
      free(temp);
      temp = next;
    } while (temp != NULL);
  }
}

//handles moves made by the players
//in: initial and final position coordinates
//out: 0 if move was made, -1 if not, 1 if player1 won, 2 if player 2 won
//     3 if it's a draw
int computerMove() {
  int *result = NULL;
  rewriteHistory();

  //choose which method to use
  if (use_minimax) {
    result = useMinimax((*history).board_state, computer_player);
  }

  //save move to history
  (*history).next = (Node *)malloc(sizeof(Node));
  (*(*history).next).prev = history;
  history = (*history).next;
  (*history).next = NULL;
  (*history).board_state = result;

  //see if anyone has won
  int player_moving = turn;
  turn = (turn == 2)? 1: 2;
  return gameOver(result, player_moving);
}

//initializes the minimax function with multithreading
//out: int* board with the next play
int *useMinimax(int *board, int player) {
  size_t num_of_children = 0;
  int ret;
  int *result = NULL;
  float max = -INFINITY;
  float eval;
  float *output;
  Node *child, *tmp;
  pthread_t *thread;

  //get the children
  child = getChildNodes(board, player);
  //count how the number of children
  tmp = child;
  while (child != NULL) {
    num_of_children++;
    child = (*child).next;
  }

  if (num_of_children == 1) {
    result = (*tmp).board_state;
    free(tmp);
    tmp = NULL;
    return result;
  }

  child = tmp;
  thread = malloc(sizeof(pthread_t) * num_of_children);
  MinimaxArgs *args[num_of_children];

  //initialize a thread for each child
  for (size_t i = 0; i < num_of_children; i++) {
    //prepare arguments to pass
    args[i] = (MinimaxArgs *)malloc(sizeof(MinimaxArgs));
    (*args[i]).player = player;
    (*args[i]).board = (*child).board_state;
    ret = pthread_create(&thread[i], NULL, &multiMinimax, (void *) args[i]);

    if (ret != 0) {
      printf("Multithreading creation error!\n" );
      exit(1);
    }
    child = (*child).next;
  }

  //get the result from each thread
  child = tmp;
  for (size_t i = 0; i < num_of_children; i++) {
    pthread_join(thread[i], (void **) &output);
    eval = *output;
    //free allocated memory
    free(output);
    free(args[i]);
    args[i] = NULL;
    //compare the children
    tmp = (*child).next;
    if (eval > max) {
      max = eval;
      result = (*child).board_state;
    }
    else {
      free((*child).board_state);
      (*child).board_state = NULL;
    }
    free(child);
    child = NULL;
    //examine the next child
    child = tmp;
  }
  free(thread);

  return result;
}

//initializes the minimax function for each thread
//out: float* evaluation of the board passed to it
void* multiMinimax(void* _args) {
  float alpha = -INFINITY;
  float beta = INFINITY;
  int depth = minimax_depth - 1;
  MinimaxArgs *args = (MinimaxArgs *) _args;
  float *return_eval;

  float result = minimax((*args).board, depth, alpha, beta, false, (*args).player);

  return_eval = malloc(sizeof(float));
  *return_eval = result;

  return return_eval;
}

//Evaluates a game state to determine how good it is
//in: board state, the player who wants to evaluate the state
float evaluatePosition(int *board, int team, int winner) {
  float evaluation = 0;
  int number;
  float opponent = (team == 1)? 2: 1;
  //weighting for each piece
  float team_piece = 30;
  float team_king = 154;
  float opponent_piece = -45;
  float opponent_king = -210;
  //advantage of having pieces on the center of the board
  float center_advantage = 2;
  float center_advantage_vertical = 2;
  //keep the pieces on home row as long as possible
  float home_row = 5;
  float opponent_home_row = -3;

  if (winner == team) {
    evaluation += 30000;
  }
  else if (winner == 0 || winner == 3) {
    evaluation += 0;
  }
  else {
    evaluation -= 30000;
  }
  for (size_t y = 1; y < 9; y++) {
    size_t x = 1 + (y % 2); //starts from a black square
    for (; x < 9; x += 2) {
        number = getBoardNumber(x, y, board);
        if (number == 0 || number == 9) {
          continue;
        }
        evaluation = (number == team)? evaluation + team_piece: evaluation;
        evaluation = (number == team + 2)? evaluation + team_king: evaluation;
        evaluation = (number == opponent)? evaluation + opponent_piece: evaluation;
        evaluation = (number == opponent + 2)? evaluation + opponent_king: evaluation;
        if (x > 2 && x < 7 && (number == team || number == team + 2)) {
          evaluation += center_advantage;
        }
        if (y > 2 && y < 7 && (number == team || number == team + 2)) {
          evaluation += center_advantage_vertical;
        }
        if (number == 1 && y == 1) {
          evaluation += home_row;
        }
        if ((number == 2 || number == 4) && y == 8) {
          evaluation += opponent_home_row;
        }
    }
  }
  return evaluation;
}

//Gets all of the states possible plays from an initial board layout
//in: int* board, int player currently moving
//out: struct Node linked list with all of the possible plays
Node *getChildNodes(int *board, int player) {
  Node *child = NULL;
  Node *first = NULL;
  int *result;
  int board_number, final[2], initial[2];
  size_t y1, x1, x, y;

  for (y1 = 1; y1 < 9; y1++) {
    x1 = 1 + (y1 % 2); //starts from a black square
    for (; x1 < 9; x1 += 2) {
      //get the piece in this position
      board_number = getBoardNumber(x1, y1, board);
      //if the piece is a player's piece
      if (board_number == player || board_number == player + 2) {
        initial[0] = x1;
        initial[1] = y1;
        //get all the possible places where it could move
        for (y = 1; y < 9; y++) {
          x = 1 + (y % 2); //starts from a black square
          for (; x < 9; x += 2) {
            board_number = getBoardNumber(x, y, board);
            if (board_number == 0) {
              final[0] = x;
              final[1] = y;
              result = makeMove(initial , final, board, player);
              //if the piece can move to this position, add the node as a child
              if (result != NULL) {
                if (child == NULL) {
                  first = (Node *)malloc(sizeof(Node));
                  (*first).prev = (*first).next = NULL;
                  child = first;
                }
                else {
                  (*child).next = (Node *)malloc(sizeof(Node));
                  (*(*child).next).prev = child;
                  child = (*child).next;
                  (*child).next = NULL;
                }
                (*child).board_state = result;
              }
            }
          }
        }
      }
    }
  }
  return first;
}

//returns the highest number
float max(float prev_max, float eval) {
  if (prev_max >= eval) {
    return prev_max;
  }
  return eval;
}

//returns the lowest number
float min(float prev_min, float eval) {
  if (prev_min <= eval) {
    return prev_min;
  }
  return eval;
}

// alpha and beta keep trak of best score either side can achieve
// aplha worst possible score for maximizingPlayer
// beta worst possible score for minimizingPlayer
//maxi_player_num the team number of the mazimizing player (1 or 2)
float minimax(int *board, int depth, float alpha, float beta, bool maximizing_player, int maxi_player_num) {
  Node *child = NULL;
  float eval;

  //get the last player who moved
  int opposite_player = (maxi_player_num == 1)? 2: 1;
  int player = (maximizing_player)? opposite_player: maxi_player_num;
  //see if the game is over
  int winner = gameOver(board, player);
  //if you reached max depth or game is over
  if (depth == 0 || winner != 0) {
    return evaluatePosition(board, maxi_player_num, winner);
  }

  //if it's the turn of the maximizing player
  if (maximizing_player) {
    Node *tmp = NULL;
    float maxEval = -INFINITY;
    // loop through all the positions that can be reached
    // in a single move
    child = getChildNodes(board, maxi_player_num);
    while (child !=  NULL) {
      //find the evaluation of each child
      eval = minimax((*child).board_state, depth - 1, alpha, beta, false, maxi_player_num);
      //take the node with the highes evaluation
      maxEval = max(maxEval, eval);
      //prune values if possible
      alpha = max(alpha, eval);
      if (beta <= alpha) {
        //free the rest of the unused nodes
        while (child != NULL) {
          tmp = (*child).next;
          free((*child).board_state);
          free(child);
          child = tmp;
        }
        break;
      }
      //free un-used memory
      tmp = (*child).next;
      free((*child).board_state);
      free(child);
      child = tmp;
    }
    return maxEval;
  }

  //if it's the turn of the minimizing player
  else {
    Node *tmp = NULL;
    float minEval = INFINITY;
    //get child node for the opponent's move
    // loop through all the positions that can be reached in a single move
    child = getChildNodes(board, opposite_player);
    while (child !=  NULL) {
      //find the evaluation of each child
      eval = minimax((*child).board_state, depth - 1, alpha, beta, true, maxi_player_num);
      //take the node with the lowest evaluation
      minEval = min(minEval, eval);
      //prune values if possible
      beta = min(beta, eval);
      if (beta <= alpha) {
        //free the rest of the unused nodes
        while (child != NULL) {
          tmp = (*child).next;
          free((*child).board_state);
          free(child);
          child = tmp;
        }
        break;
      }
      //free un-used memory
      tmp = (*child).next;
      free((*child).board_state);
      free(child);
      child = tmp;
    }
    return minEval;
  }
}