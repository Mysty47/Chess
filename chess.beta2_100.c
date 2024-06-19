#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int size = 9;
int turn = 0; // 0 - white 1 - black
int kingX;
int kingY;
int WKX;
int WKY;
int end = 0;
int totalMoves = 0;
int whiteMoves = 0;
int blackMoves = 0;
int whiteKingChecks = 0;
int blackKingChecks = 0;
int depth = 2;
void initializeBoard(char **board);
void clean(char **board);
void printBoard(char **board);
typedef struct {
  int srcX, srcY, destX, destY;
  char piece;
} Move;

Move *moveList;
int moveCount = 0;

void saveGameToFile(const char *filename, char **board, int size) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    perror("Failed to open file for recording game");
    return;
  }

  char **boardCopy = (char **)malloc(size * sizeof(char *));
  for (int i = 0; i < size; i++) {
    boardCopy[i] = (char *)malloc(size * sizeof(char));
    memcpy(boardCopy[i], board[i], size * sizeof(char));
  }
  fprintf(file, "Size of board: %d X %d\n", size-1,size-1);
  for (int i = 0; i < moveCount; i++) {
    char piece = moveList[i].piece;
    int srcX = moveList[i].srcX;
    int srcY = moveList[i].srcY;
    int destX = moveList[i].destX;
    int destY = moveList[i].destY;

    boardCopy[destX][destY] = piece;

    fprintf(file, "Move %d: %c%d -> %c%d (Piece: %c)\n", i + 1, srcY + '`',
            srcX, destY + '`', destX, piece);

    for (int row = 0; row < size; row++) {
      for (int col = 0; col < size; col++) {
        fprintf(file, "%c ", boardCopy[row][col]);
      }
      fprintf(file, "\n");
    }
    fprintf(file, "\n");
  }

  for (int i = 0; i < size; i++) {
    free(boardCopy[i]);
  }
  free(boardCopy);

  fclose(file);
}

int Start(int *c) {
  while (1) {
    printf("\nType the number to choose!\n");
    printf("1: New Game\n");
    printf("2: Change size\n");
    printf("3: Watch replay\n");
    printf("4: Exit\nEnter Value: ");

    scanf("%d", c);

    switch (*c) {
    case 1:
      return *c;

    case 2: {
      int sc;
      printf("\nChoose size:\n");
      printf("1: 6x6\n");
      printf("2: 8x8\n");
      printf("3: 9x9\n");

      scanf("%d", &sc);

      switch (sc) {
      case 1:
        size = 7;
        break;
      case 2:
        size = 9;
        break;
      case 3:
        size = 10;
        break;
      default:
        printf("\nInvalid size choice.\n");
        continue;
      }
      *c = 5;
      return *c;
    }

    case 3:
    case 4:
      return *c;

    default:
      printf("\nInvalid number\n");
    }
  }
}

int isKingInCheck(char **board, int kX, int kY) {
  for (int i = kX + 1; i < size; i++) {
    if (board[i][kY] == 'T')
      return 1;
    if (board[i][kY] != '.')
      break;
  }
  for (int i = kX - 1; i >= 0; i--) {
    if (board[i][kY] == 'T')
      return 1;
    if (board[i][kY] != '.')
      break;
  }
  for (int i = kY + 1; i < size; i++) {
    if (board[kX][i] == 'T')
      return 1;
    if (board[kX][i] != '.')
      break;
  }
  for (int i = kY - 1; i >= 0; i--) {
    if (board[kX][i] == 'T')
      return 1;
    if (board[kX][i] != '.')
      break;
  }
  return 0;
}
void clean(char **board) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      board[i][j] = '.';
    }
  }
  board[0][0] = '#';
  for (int j = 1; j < size; j++) {
    board[0][j] = 'a' + j - 1;
    board[j][0] = '1' + j - 1;
  }
}

void initializeBoard(char **board) {
  srand(time(NULL));

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      board[i][j] = '.';
    }
  }

  board[0][0] = '#';
  for (int j = 1; j < size; j++) {
    board[0][j] = 'a' + j - 1;
    board[j][0] = '1' + j - 1;
  }

  int wKingPlaced = 0, bKingPlaced = 0, towersPlaced = 0;
  while (!wKingPlaced || !bKingPlaced || towersPlaced < 2) {
    int randX = rand() % (size - 1) + 1;
    int randY = rand() % (size - 1) + 1;

    if (!wKingPlaced && board[randX][randY] == '.') {
      board[randX][randY] = 'K';
      WKX = randX;
      WKY = randY;
      wKingPlaced = 1;
    }

    if (!bKingPlaced && board[randX][randY] == '.') {
      board[randX][randY] = 'k';
      kingX = randX;
      kingY = randY;
      bKingPlaced = 1;
    }

    if (towersPlaced < 2 && board[randX][randY] == '.') {
      board[randX][randY] = 'T';
      towersPlaced++;
    }

    if (wKingPlaced && bKingPlaced) {
      if (abs(WKX - kingX) <= 1 && abs(WKY - kingY) <= 1) {
        board[WKX][WKY] = '.';
        wKingPlaced = 0;
        board[kingX][kingY] = '.';
        bKingPlaced = 0;
      }
    }

    if (wKingPlaced && isKingInCheck(board, WKX, WKY)) {
      board[WKX][WKY] = '.';
      wKingPlaced = 0;
    }
    if (bKingPlaced && isKingInCheck(board, kingX, kingY)) {
      board[kingX][kingY] = '.';
      bKingPlaced = 0;
    }
  }
}

void printBoard(char **board) {

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%c ", board[i][j]);
    }
    printf("\n");
  }
}

int will_kings_touch(char **board, int dX, int dY, int sX, int sY) {
  if (turn == 0) {
    if (board[sX][sY] == 'K') {
      if (board[dX - 1][dY - 1] == 'k') {
        return 1;
      }
      if (board[dX - 1][dY] == 'k') {
        return 1;
      }
      if (board[dX - 1][dY + 1] == 'k') {
        return 1;
      }
      if (board[dX][dY - 1] == 'k') {
        return 1;
      }
      if (board[dX][dY + 1] == 'k') {
        return 1;
      }
      if (board[dX + 1][dY - 1] == 'k') {
        return 1;
      }
      if (board[dX + 1][dY] == 'k') {
        return 1;
      }
      if (board[dX + 1][dY + 1] == 'k') {
        return 1;
      }
    }
  }
  if (turn == 1) {
    if (board[sX][sY] == 'k') {
      if (board[dX - 1][dY - 1] == 'K') {
        return 1;
      }
      if (board[dX - 1][dY] == 'K') {
        return 1;
      }
      if (board[dX - 1][dY + 1] == 'K') {
        return 1;
      }
      if (board[dX][dY - 1] == 'K') {
        return 1;
      }
      if (board[dX][dY + 1] == 'K') {
        return 1;
      }
      if (board[dX + 1][dY - 1] == 'K') {
        return 1;
      }
      if (board[dX + 1][dY] == 'K') {
        return 1;
      }
      if (board[dX + 1][dY + 1] == 'K') {
        return 1;
      }
    }
  }
  return 0;
}

int willKingBeInCheck(char **board, int kX, int kY, int destX, int destY) {
  for (int i = destX + 1; i < size; i++) {
    board[kingX][kingY] = '.';
    if (board[i][destY] == 'T') {
      board[kingX][kingY] = 'k';
      return 0;
    }
    if (board[i][destY] != '.') {
      board[kingX][kingY] = 'k';
      break;
    }
    board[kingX][kingY] = 'k';
  }
  for (int i = destX - 1; i >= 0; i--) {
    board[kingX][kingY] = '.';
    if (board[i][destY] == 'T') {
      board[kingX][kingY] = 'k';
      return 0;
    }
    if (board[i][destY] != '.') {
      board[kingX][kingY] = 'k';
      break;
    }
    board[kingX][kingY] = 'k';
  }
  for (int i = destY + 1; i < size; i++) {
    board[kingX][kingY] = '.';
    if (board[destX][i] == 'T') {
      board[kingX][kingY] = 'k';
      return 0;
    }
    if (board[destX][i] != '.') {
      board[kingX][kingY] = 'k';
      break;
    }
    board[kingX][kingY] = 'k';
  }
  for (int i = destY - 1; i >= 0; i--) {
    board[kingX][kingY] = '.';
    if (board[destX][i] == 'T') {
      board[kingX][kingY] = 'k';
      return 0;
    }
    if (board[destX][i] != '.') {
      board[kingX][kingY] = 'k';
      break;
    }
    board[kingX][kingY] = 'k';
  }
  return 1;
}

int isValidMove(char **board, int srcX, int srcY, int destX, int destY,
                int turn, int size) {

  if (srcX < 0 || srcX >= size || srcY < 0 || srcY >= size || destX < 0 ||
      destX >= size || destY < 0 || destY >= size) {
    return 0;
  }

  if (strchr("abcdefgh#123456789", board[srcX][srcY]) ||
      strchr("abcdefgh#123456789", board[destX][destY])) {
    return 0;
  }

  if (board[srcX][srcY] == '.' || (turn == 0 && board[srcX][srcY] == 'k') ||
      (turn == 1 && board[srcX][srcY] == 'K')) {
    return 0;
  }

  if (board[srcX][srcY] == 'K' || board[srcX][srcY] == 'k') {

    if (destX > srcX + 1 || destX < srcX - 1) {
      return 0;
    }

    if (destY > srcY + 1 || destY < srcY - 1) {
      return 0;
    }

    if (board[srcX][srcY] == 'K') {
      if (board[destX][destY] == 'T') {
        return 0;
      }
      if (turn == 1) {
        return 0;
      }
    }
    if (board[destX][destY] == '.' || board[destX][destY] == 'T') {
      if (will_kings_touch(board, destX, destY, srcX, srcY)) {
        return 0;
      }
    }
  }

  if (board[srcX][srcY] == 'k' || board[srcX][srcY] == 'K') {
    if (board[destX][destY] == 'a' || board[destX][destY] == 'b' ||
        board[destX][destY] == 'c' || board[destX][destY] == 'd' ||
        board[destX][destY] == 'e' || board[destX][destY] == 'f' ||
        board[destX][destY] == 'g' || board[destX][destY] == 'h' ||
        board[destX][destY] == '1' || board[destX][destY] == '2' ||
        board[destX][destY] == '3' || board[destX][destY] == '4' ||
        board[destX][destY] == '5' || board[destX][destY] == '6' ||
        board[destX][destY] == '7' || board[destX][destY] == '8') {
      return 0;
    }
  }

  if (board[srcX][srcY] == 'k') {
    if (turn == 0) {
      return 0;
    }
    return willKingBeInCheck(board, kingX, kingY, destX, destY);
  }

  if (board[srcX][srcY] == 'T') {
    if (destX != srcX && destY != srcY) {
      return 0;
    }
    if (board[destX][destY] == 'k' || board[destX][destY] == 'T' ||
        board[destX][destY] == 'K' || board[destX][destY] == 'a' ||
        board[destX][destY] == 'b' || board[destX][destY] == 'c' ||
        board[destX][destY] == 'd' || board[destX][destY] == 'e' ||
        board[destX][destY] == 'f' || board[destX][destY] == 'g' ||
        board[destX][destY] == 'h' || board[destX][destY] == '1' ||
        board[destX][destY] == '2' || board[destX][destY] == '3' ||
        board[destX][destY] == '4' || board[destX][destY] == '5' ||
        board[destX][destY] == '6' || board[destX][destY] == '7' ||
        board[destX][destY] == '8') {
      return 0;
    }
    if (turn == 1) {
      return 0;
    }
    if (srcY == destY) {
      int temp = (srcX > destX) ? -1 : 1;
      for (int i = srcX + temp; i != destX; i += temp) {
        if (board[i][srcY] != '.') {
          return 0;
        }
      }
    } else if (srcX == destX) {
      int temp = (srcY > destY) ? -1 : 1;
      for (int i = srcY + temp; i != destY; i += temp) {
        if (board[srcX][i] != '.') {
          return 0;
        }
      }
    }
  }

  char temp = board[destX][destY];
  board[destX][destY] = board[srcX][srcY];
  board[srcX][srcY] = '.';

  int kingX = -1, kingY = -1;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if ((turn == 0 && board[i][j] == 'K') ||
          (turn == 1 && board[i][j] == 'k')) {
        kingX = i;
        kingY = j;
        break;
      }
    }
    if (kingX != -1)
      break;
  }

  board[srcX][srcY] = board[destX][destY];
  board[destX][destY] = temp;

  return 1;
}

int checkForCheckMate(char **board, int kX, int kY, int deX, int deY, int sX, int sY) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (board[i][j] == 'k') {
        kX = i;
        kY = j;
        break;
      }
    }
  }

  if (isKingInCheck(board, kX, kY) == 1) {
    if (board[kX - 1][kY - 1] == '.' || board[kX - 1][kY - 1] == 'T') {
      int dX = kX - 1;
      int dY = kY - 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX - 1][kY] == '.' || board[kX - 1][kY] == 'T') {
      int dX = kX - 1;
      int dY = kY;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX - 1][kY + 1] == '.' || board[kX - 1][kY + 1] == 'T') {
      int dX = kX - 1;
      int dY = kY + 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX][kY - 1] == '.' || board[kX][kY - 1] == 'T') {
      int dX = kX;
      int dY = kY - 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX][kY + 1] == '.' || board[kX][kY + 1] == 'T') {
      int dX = kX;
      int dY = kY + 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX + 1][kY - 1] == '.' || board[kX + 1][kY - 1] == 'T') {
      int dX = kX + 1;
      int dY = kY - 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX + 1][kY] == '.' || board[kX + 1][kY] == 'T') {
      int dX = kX + 1;
      int dY = kY;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX + 1][kY + 1] == '.' || board[kX + 1][kY + 1] == 'T') {
      int dX = kX + 1;
      int dY = kY + 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
  }
  if (isKingInCheck(board, kX, kY) == 0) {
    if (board[kX - 1][kY - 1] == '.' || board[kX - 1][kY - 1] == 'T') {
      int dX = kX - 1;
      int dY = kY - 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX - 1][kY] == '.' || board[kX - 1][kY] == 'T') {
      int dX = kX - 1;
      int dY = kY;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX - 1][kY + 1] == '.' || board[kX - 1][kY + 1] == 'T') {
      int dX = kX - 1;
      int dY = kY + 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX][kY - 1] == '.' || board[kX][kY - 1] == 'T') {
      int dX = kX;
      int dY = kY - 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX][kY + 1] == '.' || board[kX][kY + 1] == 'T') {
      int dX = kX;
      int dY = kY + 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX + 1][kY - 1] == '.' || board[kX + 1][kY - 1] == 'T') {
      int dX = kX + 1;
      int dY = kY - 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX + 1][kY] == '.' || board[kX + 1][kY] == 'T') {
      int dX = kX + 1;
      int dY = kY;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
    if (board[kX + 1][kY + 1] == '.' || board[kX + 1][kY + 1] == 'T') {
      int dX = kX + 1;
      int dY = kY + 1;
      if (willKingBeInCheck(board, kX, kY, dX, dY) == 1 &&
          !will_kings_touch(board, dX, dY, kX, kY))
        return 1;
    }
  }

  if (isKingInCheck(board, kX, kY) == 1) {
    return 0;
  }

  if (isKingInCheck(board, kX, kY) == 0) {
    return 2;
  }
}

void locateWhiteKing(char **board, int KingX, int KingY) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (board[i][j] == 'K') {
        WKX = i;
        WKY = j;
        break;
      }
    }
  }
}

void locateBlackKing(char **board, int KingX, int KingY) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (board[i][j] == 'k') {
        kingX = i;
        kingY = j;
        break;
      }
    }
  }
}

int checkForDraw(char **board) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (board[i][j] == 'T')
        return 0;
    }
  }
  return 1;
}

int checkInput(char **board, int sX, int sY, int dX, int dY) {
  if (size == 9) {
    if (strchr(".#abcdefgh123456789", board[sX][sY])) {
      printf("\nInvalid input\n");
      return 0;
    }
    if (sX < 1 || sX > 8) {
      printf("\nInvalid input\n");
      return 0;
    }
  }
  if (size == 7) {
    if (strchr(".#abcdef123456", board[sX][sY])) {
      printf("\nInvalid input\n");
      return 0;
    }
    if (sX < 1 || sX > 6) {
      printf("\nInvalid input\n");
      return 0;
    }
  }
  if (size == 10) {
    if (strchr(".#abcdefghi123456789", board[sX][sY])) {
      printf("\nInvalid input\n");
      return 0;
    }
    if (sX < 1 || sX > 9) {
      printf("\nInvalid input\n");
      return 0;
    }
  }
  return 1;
}

void recordMove(int srcX, int srcY, int destX, int destY, char piece) {
  moveList = realloc(moveList, (moveCount + 1) * sizeof(Move));
  moveList[moveCount].srcX = srcX;
  moveList[moveCount].srcY = srcY;
  moveList[moveCount].destX = destX;
  moveList[moveCount].destY = destY;
  moveList[moveCount].piece = piece;
  moveCount++;
  totalMoves++;
}

void makeMove(char **board, Move move) {
  char piece = board[move.srcX][move.srcY];
  board[move.destX][move.destY] = piece;
  board[move.srcX][move.srcY] = '.';
  if (turn == 0) {
    whiteMoves++;
    turn = 1;
  } else {
    blackMoves++;
    turn = 0;
  }
  recordMove(move.srcX, move.srcY, move.destX, move.destY, piece);
}

void printMoveList() {
  printf("\nMove List:\n");
  for (int i = 0; i < moveCount; i++) {
    printf("%c%d -> %c%d (Piece: %c)\n", moveList[i].srcY + 'a' - 1,
           moveList[i].srcX, moveList[i].destY + 'a' - 1, moveList[i].destX,
           moveList[i].piece);
  }
  printf("\nTotal Moves: %d\n", totalMoves);
  printf("White Moves: %d\n", whiteMoves);
  printf("Black Moves: %d\n", blackMoves);
  printf("Black King in Check: %d times\n", blackKingChecks++);
}

int evaluate(char **board, int size) {
  int score = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (board[i][j] == 'T') {
        score += 5;
      }
    }
  }
  return score;
}

Move *generateMoves(char **board, int size, int turn, int *numMoves) {
  int maxMoves = size * size;
  Move *moves = malloc(maxMoves * sizeof(Move));
  *numMoves = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if ((turn == 0 && board[i][j] == 'K') ||
          (turn == 1 && board[i][j] == 'k')) {
        for (int x = -1; x <= 1; x++) {
          for (int y = -1; y <= 1; y++) {
            int destX = i + x;
            int destY = j + y;
            if (i == destX && j == destY) {

              continue;
            }
            if (isValidMove(board, i, j, destX, destY, turn, size)) {
              moves[*numMoves].srcX = i;
              moves[*numMoves].srcY = j;
              moves[*numMoves].destX = destX;
              moves[*numMoves].destY = destY;
              (*numMoves)++;
            }
          }
        }
      }
    }
  }

  return moves;
}

char **applyMove(char **board, Move move, int size) {

  char **newBoard = (char **)malloc(size * sizeof(char *));
  for (int i = 0; i < size; i++) {
    newBoard[i] = (char *)malloc(size * sizeof(char));

    memcpy(newBoard[i], board[i], size * sizeof(char));
  }

  newBoard[move.destX][move.destY] = board[move.srcX][move.srcY];
  newBoard[move.srcX][move.srcY] = '.';
  return newBoard;
}

void freeBoard(char **board, int size) {
  for (int i = 0; i < size; ++i) {
    free(board[i]);
  }
  free(board);
}

int minimax(char **board, int size, int depth, int alpha, int beta,
            int maximizingPlayer, int turn) {
  if (depth == 0 || checkForDraw(board)) {
    return evaluate(board, size);
  }

  int numMoves;
  Move *moves = generateMoves(board, size, turn, &numMoves);

  if (maximizingPlayer) {
    int maxEval = INT_MIN;
    for (int i = 0; i < numMoves; i++) {
      char **newBoard = applyMove(board, moves[i], size);
      int eval = minimax(newBoard, size, depth - 1, alpha, beta, 0, 1 - turn);
      maxEval = (eval > maxEval) ? eval : maxEval;
      alpha = (alpha > eval) ? alpha : eval;
      freeBoard(newBoard, size);

      if (beta <= alpha) {
        break;
      }
    }
    free(moves);
    return maxEval;
  } else {
    int minEval = INT_MAX;
    for (int i = 0; i < numMoves; i++) {
      char **newBoard = applyMove(board, moves[i], size);
      int eval = minimax(newBoard, size, depth - 1, alpha, beta, 1, 1 - turn);
      minEval = (eval < minEval) ? eval : minEval;
      beta = (beta < eval) ? beta : eval;
      freeBoard(newBoard, size);

      if (beta <= alpha) {
        break;
      }
    }
    free(moves);
    return minEval;
  }
}

Move findBestMove(char **board, int size, int depth, int turn) {

  int numMoves;
  Move *moves = generateMoves(board, size, turn, &numMoves);

  int bestEvalBlack = INT_MAX;
  Move bestMove;
  for (int i = 0; i < numMoves; i++) {

    char **newBoard = applyMove(board, moves[i], size);

    int eval = minimax(newBoard, size, depth, INT_MIN, INT_MAX, 0, 1 - turn);

    if (eval < bestEvalBlack) {
      bestEvalBlack = eval;
      bestMove = moves[i];
    }

    for (int j = 0; j < size; j++) {
      free(newBoard[j]);
    }
    free(newBoard);
  }

  free(moves);

  printf("Best Move for Black: From (%d, %d) to (%d, %d)\n", bestMove.srcX,
         bestMove.srcY, bestMove.destX, bestMove.destY);
  makeMove(board, bestMove);
  printf("Best Evaluation for Black: %d\n", bestEvalBlack);
  printf("AI move: %c%d -> %c%d\n", bestMove.srcY + '`', bestMove.srcX,
         bestMove.destY + '`', bestMove.destX);
  return bestMove;
}

void Playing(char **board) {
  int turn = 0;
  int end = 0;

  while (!end) {
    printBoard(board);

    if (turn == 1) {

      printf("\nAI is thinking...\n");
      Move aiMove = findBestMove(board, size, depth, turn);

      int kingX, kingY;

      for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
          if (board[i][j] == 'k') {
            kingX = i;
            kingY = j;
            break;
          }
        }
      }
      locateBlackKing(board, kingX, kingY);
      // Check for checkmate
      if (isKingInCheck(board, kingX, kingY) == 1) {
        if (checkForCheckMate(board, kingX, kingY, aiMove.destX, aiMove.destY, aiMove.srcX, aiMove.srcY) == 0) {
          printf("\nCheckmate! AI wins.\n");
          printBoard(board);
          end = 1;
        } else {

          printf("\nCheck!\n");
        }
      }
      if (isKingInCheck(board, kingX, kingY) == 0) {
        if (checkForCheckMate(board, kingX, kingY, aiMove.destX, aiMove.destY,
                              aiMove.srcX, aiMove.srcY) == 2) {
          printf("\nStalemate!\n");
          end = 1;
        }
      }

    } else {

      int srcX, srcY, destX, destY;
      char userX, userdestX;

      printf("\nEnter the coordinates of the piece you want to move (example "
             "-> a1): ");
      scanf(" %c%d", &userX, &srcY);
      srcX = srcY;
      srcY = userX - 'a' + 1;

      printf("Enter the destination (example -> a2): ");
      scanf(" %c%d", &userdestX, &destY);
      destX = destY;
      destY = userdestX - 'a' + 1;

      if (isValidMove(board, srcX, srcY, destX, destY, turn, size)) {
        makeMove(board, (Move){srcX, srcY, destX, destY});
        printf("You have made your move.\n");

        int kingX, kingY;

        for (int i = 0; i < size; i++) {
          for (int j = 0; j < size; j++) {
            if (board[i][j] == 'k') {
              kingX = i;
              kingY = j;
              break;
            }
          }
        }
        printBoard(board);

        if (isKingInCheck(board, kingX, kingY) == 1) {
          if (checkForCheckMate(board, kingX, kingY, destX, destY, srcX,
                                srcY) == 0) {
            blackKingChecks++;
            printf("\nCheckmate! You win.\n");
            printBoard(board);
            {
              printf("\nMove List:\n");
              for (int i = 0; i < moveCount; i++) {
                printf("%c%d -> %c%d (Piece: %c)\n", moveList[i].srcY + 'a' - 1,
                       moveList[i].srcX, moveList[i].destY + 'a' - 1,
                       moveList[i].destX, moveList[i].piece);
              }
              printf("\nTotal Moves: %d\n", totalMoves);
              printf("White Moves: %d\n", whiteMoves);
              printf("Black Moves: %d\n", blackMoves);
              printf("Black King in Check: %d times\n", blackKingChecks);
            }
            end = 1;
          } else {
            printf("\nCheck!\n");
            blackKingChecks++;
          }
        }

      } else {
        printf("\nInvalid move. Try again.\n");
        continue;
      }
    }

    if (checkForDraw(board) == 1) {
      printf("\nDraw\n");
      printBoard(board);
      printMoveList();
      end = 1;
    }

    turn = 1 - turn;
  }
}

int main(void) {
  int check;
  char **board;
  moveList = NULL;
  moveCount = 0;

  while (1) {
    check = Start(&check);

    if (check == 4) {
      free(moveList);
      for (int i = 0; i < size; i++) {
        free(board[i]);
      }
      free(board);
      return 0;
    }

    if (check == 5) {
      continue;
    }

    if (check == 1) {
      end = 0;
      turn = 0;
      totalMoves = 0;
      whiteMoves = 0;
      blackMoves = 0;
      whiteKingChecks = 0;
      blackKingChecks = 0;
      moveCount = 0;
      free(moveList);
      moveList = NULL;

      board = malloc(size * sizeof(char *));
      for (int i = 0; i < size; i++) {
        board[i] = malloc(size * sizeof(char));
      }

      initializeBoard(board);
      Playing(board);

    } else if (check == 3) {
      printf("\nLoading replay...\n");

      board = malloc(size * sizeof(char *));
      for (int i = 0; i < size; i++) {
        board[i] = malloc(size * sizeof(char));
      }
      clean(board);
      saveGameToFile("replay.txt", board, size);

      for (int i = 0; i < size; i++) {
        free(board[i]);
      }
      free(board);
    } else {
      printf("\nInvalid number\n");
      continue;
    }
  }

  return 0;
}
