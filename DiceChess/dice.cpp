#include "./dice.h"

#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "./chess.h"

bool badDiceCheck(const std::string &input, char mustBeNextPiece, bool chooseFrozen, char &frozenPiece,
                  bool chooseSquare, int &frozenRow, int &frozenFile, int turn, int &tmp) {
  if (mustBeNextPiece != 'x' && input[0] != mustBeNextPiece) {
    std::cout << "You have to move with " << mustBeNextPiece << std::endl;
    return true;
  }

  if (chooseFrozen && frozenPiece == 'z') {
    if (input.length() == 1 && (turn == 0 ? !isWhite(input[0]) : isWhite(input[0]))) {
      frozenPiece = input[0];
      std::cout << "Continue with your own move: ";
      return true;
    } else {
      std::cout << "4: Choose a " << (turn == 0 ? "black" : "white") << " piece that can't be moved the next round."
                << std::endl;
      return true;
    }
  } else if (chooseFrozen && frozenPiece != 'z' && (turn == (isWhite(frozenPiece) ? 0 : 1)) &&
             input[0] == frozenPiece) {
    std::cout << "This piece is frozen this round. Choose another piece to move: ";
    return true;
  }

  if (chooseSquare && frozenRow == 10) {
    if (input.length() == 2) {
      frozenRow = 7 - (input[1] - '1');
      frozenFile = input[0] - 'a';
      std::cout << "Continue with your own move: ";
      tmp = 1 - turn;
      return true;
    } else {
      std::cout << "5: Choose a square your opponent can't move to in the next round." << std::endl;
      return true;
    }
  }

  return false;  // Falls alle Prüfungen bestanden sind
}

bool isMoveInCorrectDirection(const std::string &mustBeNextDirection, int startingRow, int startingFile, int targetRow,
                              int targetFile) {
  if (mustBeNextDirection != "y") {
    int dr = targetRow - startingRow;
    int dc = targetFile - startingFile;
    if ((mustBeNextDirection == "left" && dc >= 0) || (mustBeNextDirection == "right" && dc <= 0) ||
        (mustBeNextDirection == "up" && dr >= 0) || (mustBeNextDirection == "down" && dr <= 0)) {
      std::cout << "You have to move in direction " << mustBeNextDirection << std::endl;
      return false;
    }
  }
  return true;
}

bool canPawnMoveLikeKnight(int turn, char piece, char board[8][8], int targetRow, int targetFile) {
  if ((turn == 0 && piece == 'P') || (turn == 1 && piece == 'p')) {
    if (board[targetRow][targetFile] == 'K' || board[targetRow][targetFile] == 'k') {
      std::cout << "Don't take the king!" << std::endl;
      return false;
    }
    return true;
  }
  return false;
}

bool isSquareFrozen(bool chooseSquare, int targetRow, int targetFile, int frozenRow, int frozenFile, int turn,
                    int tmp) {
  if (chooseSquare && targetRow == frozenRow && targetFile == frozenFile && turn == tmp) {
    std::cout << "This square is frozen this round. Choose another square to "
                 "move to: ";
    return true;
  }
  return false;
}

void undoChanges(char &mustBeNextPiece, std::string &mustBeNextDirection, bool &chooseFrozen, char &frozenPiece,
                 bool &chooseSquare, int &frozenRow, int &frozenFile, int turn, int tmp, bool &PawnsLikeKnights,
                 bool &PawnLikeKnight) {
  // Rückgängig machen der Änderungen für die Schachfigur
  mustBeNextPiece = 'x';

  // Rückgängig machen der Richtungsänderung
  mustBeNextDirection = "y";

  // Rückgängig machen der Wahl der eingefrorenen Schachfigur
  if (chooseFrozen && frozenPiece != 'z' && (turn == (isWhite(frozenPiece) ? 1 : 0))) {
    chooseFrozen = false;
    frozenPiece = 'z';
  }

  // Rückgängig machen der Wahl des eingefrorenen Feldes
  if (chooseSquare && turn == 1 - tmp) {
    chooseSquare = false;
    frozenFile = 10;
    frozenRow = 10;
  }
  PawnsLikeKnights = false;
  PawnLikeKnight = false;
}

// Hilfsfunktion zur Überprüfung eines möglichen Zugs (für movePiece)
bool checkMove(int row, int File, int newRow, int newFile, char mustBeNextPiece, char board[8][8], int turn,
               const std::map<char, Piece *> &pieceMap, int &stop) {
  if (newRow >= 0 && newRow < 8 && newFile >= 0 && newFile < 8) {  // prüfen, ob das Ziel auf dem Brett liegt
    char targetPiece = board[newRow][newFile];
    if ((turn == 0 && !std::isupper(targetPiece)) ||
        (turn == 1 && !std::islower(targetPiece))) {  // prüfen, ob der Zug erlaubt ist
      Move move = {row, File, newRow, newFile, targetPiece, mustBeNextPiece, (targetPiece != ' ' ? true : false)};
      if (canMove_notCheck(board, turn, pieceMap, move)) {
        stop = 1;
        std::cout << "You have to move with " << mustBeNextPiece << std::endl;
        return true;  // Gültiger Zug gefunden
      }
    }
  }
  return false;
}

void handleDiceRoll(int randomNumber, std::vector<char> &White, std::vector<char> &Black,
                    std::vector<std::pair<int, int>> &WhitePositions, std::vector<std::pair<int, int>> &BlackPositions,
                    char &mustBeNextPiece, std::string &mustBeNextDirection, bool &chooseFrozen, char &frozenPiece,
                    bool &chooseSquare, int &frozenRow, int &frozenFile, int &turn, int &tmp, char board[8][8],
                    bool &PawnsLikeKnights, bool &PawnLikeKnight, const std::map<char, Piece *> &pieceMap) {
  if (randomNumber == 1) {
    movePiece(randomNumber, White, Black, WhitePositions, BlackPositions, mustBeNextPiece, tmp, turn, board, pieceMap);
  } else if (randomNumber == 2) {
    moveDirection(randomNumber, White, Black, WhitePositions, BlackPositions, mustBeNextDirection, chooseFrozen,
                  frozenPiece, chooseSquare, frozenRow, frozenFile, tmp, turn, board, pieceMap);
  } else if (randomNumber == 3) {
    switchTurn(randomNumber, turn);
  } else if (randomNumber == 4 && frozenPiece == 'z') {
    chooseFrozenPiece(randomNumber, chooseFrozen, frozenPiece, turn);
  } else if (randomNumber == 5 || randomNumber == 4) {
    chooseSquareForOpponent(randomNumber, chooseSquare, frozenRow, frozenFile, turn, PawnsLikeKnights);
  } else if (randomNumber == 6) {
    activatePawnsLikeKnights(randomNumber, PawnsLikeKnights);
  }
}

void movePiece(int randomNumber, std::vector<char> &White, std::vector<char> &Black,
               std::vector<std::pair<int, int>> &WhitePositions, std::vector<std::pair<int, int>> &BlackPositions,
               char &mustBeNextPiece, int &stop, int turn, char board[8][8], const std::map<char, Piece *> &pieceMap) {
  std::cout << "1: ";
  stop = 0;
  while (true) {
    int randomIndex = std::rand() % (turn == 0 ? White.size() : Black.size());
    mustBeNextPiece = (turn == 0 ? White[randomIndex] : Black[randomIndex]);
    auto [row, File] = (turn == 0 ? WhitePositions[randomIndex] : BlackPositions[randomIndex]);

    // Bewegungsüberprüfung
    for (int dr = -1; dr <= 1; ++dr) {
      for (int dc = -1; dc <= 1; ++dc) {
        if (dr == 0 && dc == 0) continue;

        int newRow = row + dr;
        int newFile = File + dc;
        if (checkMove(row, File, newRow, newFile, mustBeNextPiece, board, turn, pieceMap, stop)) {
          break;
        }
      }
      if (stop == 1) break;
    }
    if (stop == 1) break;
  }
  if (stop == 0) {
    std::cout << "Continue!" << std::endl;
  }
}

void moveDirection(int randomNumber, std::vector<char> &White, std::vector<char> &Black,
                   std::vector<std::pair<int, int>> &WhitePositions, std::vector<std::pair<int, int>> &BlackPositions,
                   std::string &mustBeNextDirection, bool &chooseFrozen, char &frozenPiece, bool &chooseSquare,
                   int &frozenRow, int &frozenFile, int &stop, int turn, char board[8][8],
                   const std::map<char, Piece *> &pieceMap) {
  std::cout << "2: ";
  std::array<std::string, 4> directions = {"left", "right", "up", "down"};
  std::vector<std::array<int, 6>> moves = {
      {-1, -1, 0, -1, 1, -1}, {-1, 1, 0, 1, 1, 1}, {-1, -1, -1, 0, -1, 1}, {1, -1, 1, 0, 1, 1}};
  stop = 0;

  while (true) {
    int help = std::rand() % 4;
    mustBeNextDirection = directions[help];
    std::array<int, 6> directionsInNumbers = moves[help];

    // Bewegungsprüfung für zufällige Richtung
    for (std::size_t i = 0; i < (turn == 0 ? White.size() : Black.size()); i++) {
      char examplePiece = (turn == 0 ? White[i] : Black[i]);
      auto [row, File] = (turn == 0 ? WhitePositions[i] : BlackPositions[i]);
      for (int x = 0; x < 3; x++) {
        int newRow = row + directionsInNumbers[x * 2];
        int newFile = File + directionsInNumbers[x * 2 + 1];
        if (newRow >= 0 && newRow < 8 && newFile >= 0 && newFile < 8) {
          char targetPiece = board[newRow][newFile];
          if ((turn == 0 && !std::isupper(targetPiece)) || (turn == 1 && !std::islower(targetPiece))) {
            Move move = {row, File, newRow, newFile, targetPiece, examplePiece, targetPiece != ' '};
            if (canMove_notCheck(board, turn, pieceMap, move)) {
              stop = 1;
              std::cout << "You have to move in direction " << mustBeNextDirection << std::endl;
              break;
            }
          }
        }
      }
      if (stop == 1) break;
    }
    if (stop == 1) break;
  }
}

void switchTurn(int randomNumber, int &turn) {
  std::cout << "3: It's " << (turn == 0 ? "Black's" : "White's") << " turn again." << std::endl;
  turn = 1 - turn;
  std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
}

void chooseFrozenPiece(int randomNumber, bool &chooseFrozen, char &frozenPiece, int turn) {
  if (frozenPiece != 'z') {
    randomNumber = 5;
  } else {
    std::cout << "4: Choose a " << (turn == 0 ? "black" : "white") << " piece that can't be moved the next round."
              << std::endl;
    chooseFrozen = true;
  }
}

void chooseSquareForOpponent(int randomNumber, bool &chooseSquare, int &frozenRow, int &frozenFile, int turn,
                             bool &PawnsLikeKnights) {
  if (frozenRow != 10) {
    activatePawnsLikeKnights(randomNumber, PawnsLikeKnights);
  } else {
    std::cout << "5: Choose a square your opponent can't move to in the next round." << std::endl;
    chooseSquare = true;
  }
}

void activatePawnsLikeKnights(int randomNumber, bool &PawnsLikeKnights) {
  std::cout << "6: Pawns can only move like Knights this round (but don't take "
               "the king ;))."
            << std::endl;
  PawnsLikeKnights = true;
}
