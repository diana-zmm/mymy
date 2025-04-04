#pragma once
#include <map>
#include <string>
#include <vector>

#include "./chess.h"

bool badDiceCheck(const std::string &input, char mustBeNextPiece, bool chooseFrozen, char &frozenPiece,
                  bool chooseSquare, int &frozenRow, int &frozenFile, int turn, int &tmp);

bool isMoveInCorrectDirection(const std::string &mustBeNextDirection, int startingRow, int startingFile, int targetRow,
                              int targetFile);

bool canPawnMoveLikeKnight(int turn, char piece, char board[8][8], int targetRow, int targetFile);
bool isSquareFrozen(bool chooseSquare, int targetRow, int targetFile, int frozenRow, int frozenFile, int turn, int tmp);

void undoChanges(char &mustBeNextPiece, std::string &mustBeNextDirection, bool &chooseFrozen, char &frozenPiece,
                 bool &chooseSquare, int &frozenRow, int &frozenFile, int turn, int tmp, bool &PawnsLikeKnights,
                 bool &PawnLikeKnight);
bool checkMove(int row, int File, int newRow, int newFile, char mustBeNextPiece, char board[8][8], int turn,
               const std::map<char, Piece *> &pieceMap, int &stop);

void handleDiceRoll(int randomNumber, std::vector<char> &White, std::vector<char> &Black,
                    std::vector<std::pair<int, int>> &WhitePositions, std::vector<std::pair<int, int>> &BlackPositions,
                    char &mustBeNextPiece, std::string &mustBeNextDirection, bool &chooseFrozen, char &frozenPiece,
                    bool &chooseSquare, int &frozenRow, int &frozenFile, int &turn, int &tmp, char board[8][8],
                    bool &PawnsLikeKnights, bool &PawnLikeKnight, const std::map<char, Piece *> &pieceMap);

void movePiece(int randomNumber, std::vector<char> &White, std::vector<char> &Black,
               std::vector<std::pair<int, int>> &WhitePositions, std::vector<std::pair<int, int>> &BlackPositions,
               char &mustBeNextPiece, int &stop, int turn, char board[8][8], const std::map<char, Piece *> &pieceMap);

void moveDirection(int randomNumber, std::vector<char> &White, std::vector<char> &Black,
                   std::vector<std::pair<int, int>> &WhitePositions, std::vector<std::pair<int, int>> &BlackPositions,
                   std::string &mustBeNextDirection, bool &chooseFrozen, char &frozenPiece, bool &chooseSquare,
                   int &frozenRow, int &frozenFile, int &stop, int turn, char board[8][8],
                   const std::map<char, Piece *> &pieceMap);

void switchTurn(int randomNumber, int &turn);

void chooseFrozenPiece(int randomNumber, bool &chooseFrozen, char &frozenPiece, int turn);

void chooseSquareForOpponent(int randomNumber, bool &chooseSquare, int &frozenRow, int &frozenFile, int turn,
                             bool &PawnsLikeKnights);

void activatePawnsLikeKnights(int randomNumber, bool &PawnsLikeKnights);