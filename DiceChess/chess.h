#pragma once

#include <cctype>
#include <map>
#include <string>
#include <vector>

void printBoard(char board[8][8]);

struct Move {
  int startingRow, startingFile;
  int targetRow, targetFile;
  char capturedPiece;
  char movedPiece;
  bool capture;
};

class Piece {
 public:
  virtual bool CanMove(const Move& move, char board[8][8]) const = 0;
  virtual ~Piece() = default;
};

class Pawn : public Piece {
 public:
  bool CanMove(const Move& move, char board[8][8]) const override;
};

class Rook : public Piece {
 public:
  bool CanMove(const Move& move, char board[8][8]) const override;
};

class Knight : public Piece {
 public:
  bool CanMove(const Move& move, char board[8][8]) const override;
};

class Bishop : public Piece {
 public:
  bool CanMove(const Move& move, char board[8][8]) const override;
};

class Queen : public Piece {
 public:
  bool CanMove(const Move& move, char board[8][8]) const override;
};

class King : public Piece {
 public:
  bool CanMove(const Move& move, char board[8][8]) const override;
};

bool iscapture(const std::string& input);
bool isWhite(char piece);
bool ispromotion(const std::string& input);
bool PromotionCheck(const std::string& input, const Move& move, int turn, char PromotionPiece);
bool isEnPassant(const Move& move, int& enPassantTarget);

bool isRochade(const std::string& input);
bool isSquareAttacked(int row, int File, char attackingFileor, const char board[8][8],
                      const std::map<char, Piece*>& pieceMap);
bool canCastle(int kingRow, int rookFile, bool canCastleSide, char attackingFileor, char board[8][8],
               const std::map<char, Piece*>& pieceMap);
void makeCastleMove(int startingFile, int startingRow, std::string castleNotation, char board[8][8],
                    bool& whiteCanCastleKingSide, bool& whiteCanCastleQueenSide, bool& blackCanCastleKingSide,
                    bool& blackCanCastleQueenSide, const std::map<char, Piece*>& pieceMap, bool& castle);

void findKing(const char board[8][8], char king, int& kingRow, int& kingFile);

bool isKingInCheck(const char board[8][8], int turn, const std::map<char, Piece*>& pieceMap);

bool canMove_notCheck(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap, const Move& move);

int countCheckThreats(const char board[8][8], int turn, const std::map<char, Piece*>& pieceMap,
                      std::vector<std::pair<int, int>>& attackers);

bool canKingEscape(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap);

bool canCaptureAttacker(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap,
                        std::vector<std::pair<int, int>>& attackers);

bool canBlockCheck(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap, std::pair<int, int> attacker);

bool isCheckmate(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap);
