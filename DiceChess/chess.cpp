#include "./chess.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

void printBoard(char board[8][8]) {
  for (int i = 0; i < 8; ++i) {
    std::cout << (8 - i) << " | ";
    for (int j = 0; j < 8; ++j) {
      std::cout << board[i][j] << " | ";
    }
    std::cout << std::endl;
    std::cout << "  ---------------------------------" << std::endl;
  }
  std::cout << "    a   b   c   d   e   f   g   h" << std::endl;
}

bool iscapture(const std::string& input) {
  if (input[3] == 'x') {
    return input.length() == 6 || input.length() == 8;
  }
  return false;
}

bool ispromotion(const std::string& input) {
  if (input.length() == 7 || input.length() == 8) {
    if (input.length() == 7 ? input[5] == '=' : input[6] == '=') {
      return true;
    }
  }
  return false;
}

bool PromotionCheck(const std::string& input, const Move& move, int turn, char PromotionPiece) {
  // Voraussetzung: Figur ist ein Pawn in der richtigen Reihe
  if ((turn == 0 ? move.movedPiece == 'P' : move.movedPiece == 'p') &&
      (turn == 0 ? move.targetRow == 0 : move.targetRow == 7) &&
      (PromotionPiece == 'Q' || PromotionPiece == 'B' || PromotionPiece == 'R' || PromotionPiece == 'N' ||
       PromotionPiece == 'q' || PromotionPiece == 'b' || PromotionPiece == 'r' || PromotionPiece == 'n')) {
    return isWhite(move.movedPiece) == isWhite(PromotionPiece);
  }
  return false;
}

bool isWhite(char piece) { return std::isupper(piece); }

bool isEnPassant(const Move& move, int& enPassantTarget) {
  if ((move.movedPiece == 'P' || move.movedPiece == 'p')) {
    if (move.startingFile != move.targetFile && move.capturedPiece == ' ' &&
        move.capture) {  // Diagonaler Zug ohne Figur => En Passant
      if (enPassantTarget == move.targetFile &&
          ((move.movedPiece == 'P' && move.startingRow == 3) || (move.movedPiece == 'p' && move.startingRow == 4))) {
        return true;
      }
    }
    if (abs(move.startingRow - move.targetRow) == 2) {  // Doppelzug => En Passant möglich
      enPassantTarget = move.targetFile;
    } else {
      enPassantTarget = -1;
    }
  } else {
    enPassantTarget = -1;  // Kein Bauernzug -> En Passant wird zurückgesetzt
  }
  return false;
}

bool isRochade(const std::string& input) { return input.find('-') != std::string::npos; }

bool isSquareAttacked(int squareRow, int squareFile, int turn, const char board[8][8],
                      const std::map<char, Piece*>& pieceMap) {
  char squarepiece = board[squareRow][squareFile];
  // Überprüfung durch alle gegnerischen Figuren
  for (int row = 0; row < 8; ++row) {
    for (int File = 0; File < 8; ++File) {
      char piece = board[row][File];

      // Gegnerische Figuren überprüfen
      if ((turn == 0 && std::islower(piece)) || (turn == 1 && std::isupper(piece))) {
        Move move = {row, File, squareRow, squareFile, squarepiece, piece, (squarepiece == ' ' ? false : true)};
        if (pieceMap.count(piece) == 0) continue;
        if (pieceMap.at(piece)->CanMove(move, const_cast<char(*)[8]>(board))) {
          return true;
        }
      }
    }
  }
  return false;
}

bool canCastle(int kingRow, int rookFile, bool canCastleSide, int turn, char board[8][8],
               const std::map<char, Piece*>& pieceMap) {
  if (!canCastleSide) return false;  // Rochaderecht verloren?

  if (board[kingRow][rookFile] != (turn == 0 ? 'R' : 'r')) return false;
  // Turm vorhanden?

  int kingFile = 4;
  if (isSquareAttacked(kingRow, kingFile, turn, board, pieceMap)) return false;
  int direction = (rookFile == 7) ? 1 : -1;
  for (int i = kingFile + direction; i != rookFile; i += direction) {
    if (board[kingRow][i] != ' ') return false;  // Blockierte Felder?
    if (i != rookFile - direction && isSquareAttacked(kingRow, i, turn, board, pieceMap)) return false;
    // Angriffe prüfen
  }
  return true;
}

void makeCastleMove(int startingFile, int startingRow, std::string castleNotation, char board[8][8],
                    bool& whiteCanCastleKingSide, bool& whiteCanCastleQueenSide, bool& blackCanCastleKingSide,
                    bool& blackCanCastleQueenSide, const std::map<char, Piece*>& pieceMap, bool& castle) {
  bool isWhite = (startingRow == 7);
  char king = isWhite ? 'K' : 'k';
  char rook = isWhite ? 'R' : 'r';
  if (castleNotation == "0-0") {  // Kurze Rochade
    if (canCastle(startingRow, 7, isWhite ? whiteCanCastleKingSide : blackCanCastleKingSide, isWhite ? 0 : 1, board,
                  pieceMap)) {
      board[startingRow][4] = ' ';
      board[startingRow][6] = king;
      board[startingRow][7] = ' ';
      board[startingRow][5] = rook;
      if (isWhite)
        whiteCanCastleKingSide = whiteCanCastleQueenSide = false;
      else
        blackCanCastleKingSide = blackCanCastleQueenSide = false;
    } else {
      castle = false;
      std::cout << "invalid, castle not possible" << std::endl;
    }
  } else if (castleNotation == "0-0-0") {  // Lange Rochade
    if (canCastle(startingRow, 0, isWhite ? whiteCanCastleQueenSide : blackCanCastleQueenSide, isWhite ? 0 : 1, board,
                  pieceMap)) {
      board[startingRow][4] = ' ';
      board[startingRow][2] = king;
      board[startingRow][0] = ' ';
      board[startingRow][3] = rook;
      if (isWhite)
        whiteCanCastleKingSide = whiteCanCastleQueenSide = false;
      else
        blackCanCastleKingSide = blackCanCastleQueenSide = false;
    } else {
      castle = false;
      std::cout << "invalid, castle not possible" << std::endl;
    }
  }
}

bool Pawn::CanMove(const Move& move, char board[8][8]) const {
  int drow = move.targetRow - move.startingRow;
  int dfile = std::abs(move.targetFile - move.startingFile);
  if (isWhite(move.movedPiece)) {  // Weiß
    if (dfile == 0) {              // kein capture
      if (drow == -1 && move.capturedPiece == ' ') return true;
      if (drow == -2 && move.startingRow == 6 && board[move.targetRow][move.targetFile] == ' ' &&
          board[move.startingRow - 1][move.startingFile] == ' ')
        return true;
    } else if (dfile == 1 && drow == -1) {  // capture
      if (move.capture && move.capturedPiece != ' ' && !isWhite(move.capturedPiece)) return true;
    }

  } else {             // Schwarz
    if (dfile == 0) {  // kein capture
      if (drow == 1 && move.capturedPiece == ' ') return true;
      if (drow == 2 && move.startingRow == 1 && board[move.targetRow][move.targetFile] == ' ' &&
          board[move.startingRow + 1][move.startingFile] == ' ')
        return true;
    } else if (dfile == 1 && drow == 1) {  // capture
      if (move.capture && move.capturedPiece != ' ' && isWhite(move.capturedPiece)) return true;
    }
  }
  return false;
}

bool Rook::CanMove(const Move& move, char board[8][8]) const {
  // Zielfeld überprüfen je nach capture oder nicht
  if (move.capture) {
    if (board[move.targetRow][move.targetFile] == ' ' ||
        isWhite(board[move.targetRow][move.targetFile]) == isWhite(move.movedPiece)) {
      return false;
    }
  } else {
    if (board[move.targetRow][move.targetFile] != ' ') {
      return false;
    }
  }
  // nach blockierten Feldern suchen
  if (move.startingRow == move.targetRow) {
    int step = (move.targetFile > move.startingFile) ? 1 : -1;
    for (int File = move.startingFile + step; File != move.targetFile; File += step) {
      if (board[move.startingRow][File] != ' ') {
        return false;
      }
    }
    return true;
  } else if (move.startingFile == move.targetFile) {
    int step = (move.targetRow > move.startingRow) ? 1 : -1;
    for (int row = move.startingRow + step; row != move.targetRow; row += step) {
      if (board[row][move.startingFile] != ' ') {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool Knight::CanMove(const Move& move, char board[8][8]) const {
  int drow = std::abs(move.targetRow - move.startingRow);
  int dFile = std::abs(move.targetFile - move.startingFile);
  return (drow == 2 && dFile == 1) || (drow == 1 && dFile == 2);
}

bool Bishop::CanMove(const Move& move, char board[8][8]) const {
  int drow = move.targetRow - move.startingRow;
  int dFile = move.targetFile - move.startingFile;
  // nach blockierten Feldern suchen
  if (std::abs(drow) == std::abs(dFile)) {
    int stepRow = (drow > 0) ? 1 : -1;
    int stepFile = (dFile > 0) ? 1 : -1;
    int row = move.startingRow + stepRow;
    int File = move.startingFile + stepFile;
    while (row != move.targetRow && File != move.targetFile) {
      if (board[row][File] != ' ') {
        return false;
      }
      row += stepRow;
      File += stepFile;
    }
    // Zielfeld überprüfen je nach capture oder nicht
    if (move.capture) {
      if (board[move.targetRow][move.targetFile] == ' ' ||
          isWhite(board[move.targetRow][move.targetFile]) == isWhite(move.movedPiece)) {
        return false;
      }
    } else {
      if (board[move.targetRow][move.targetFile] != ' ') {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool Queen::CanMove(const Move& move, char board[8][8]) const {
  Rook rook;
  Bishop bishop;
  return rook.CanMove(move, board) || bishop.CanMove(move, board);
}

bool King::CanMove(const Move& move, char board[8][8]) const {
  int drow = std::abs(move.targetRow - move.startingRow);
  int dFile = std::abs(move.targetFile - move.startingFile);

  if (move.capture) {
    if (move.capturedPiece == ' ' || isWhite(move.capturedPiece) == isWhite(move.movedPiece)) {
      return false;
    }
  } else {
    if (move.capturedPiece != ' ') {
      return false;
    }
  }
  // König darf jeweils nur ein Feld weiter gehen
  return drow <= 1 && dFile <= 1;
}

// Findet die Position des Königs auf dem Brett
void findKing(const char board[8][8], char king, int& kingRow, int& kingFile) {
  for (int row = 0; row < 8; ++row) {
    for (int File = 0; File < 8; ++File) {
      if (board[row][File] == king) {
        kingRow = row;
        kingFile = File;
        return;
      }
    }
  }
}

// Prüft, ob der König im Schach steht
bool isKingInCheck(const char board[8][8], int turn, const std::map<char, Piece*>& pieceMap) {
  int kingRow = -1, kingFile = -1;
  char king = (turn == 0) ? 'K' : 'k';
  findKing(board, king, kingRow, kingFile);

  // Falls König nicht gefunden wurde (sollte nie passieren)
  if (kingRow == -1 || kingFile == -1) return false;

  // Überprüfung durch alle gegnerischen Figuren
  for (int row = 0; row < 8; ++row) {
    for (int File = 0; File < 8; ++File) {
      char piece = board[row][File];  // 7- ?

      // Gegnerische Figuren überprüfen
      if ((turn == 0 && std::islower(piece)) || (turn == 1 && std::isupper(piece))) {
        Move move = {row, File, kingRow, kingFile, king, piece, true};
        if (pieceMap.at(piece)->CanMove(move, const_cast<char(*)[8]>(board))) {
          return true;
        }
      }
    }
  }
  return false;
}

bool canMove_notCheck(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap, const Move& move) {
  char tempBoard[8][8];
  std::memcpy(tempBoard, board, sizeof(tempBoard));

  // überprüfen, ob König auf das neue Feld ziehen darf
  if (pieceMap.at(move.movedPiece)->CanMove(move, tempBoard)) {
    // wenn ja, dann Zug ausführen und Turn wechseln
    tempBoard[move.targetRow][move.targetFile] = move.movedPiece;
    tempBoard[move.startingRow][move.startingFile] = ' ';
    // überprüfen, ob König auf neuem Feld im Schach steht
    if (isKingInCheck(tempBoard, turn, pieceMap)) {
      tempBoard[move.startingRow][move.startingFile] = move.movedPiece;
      tempBoard[move.targetRow][move.targetFile] = move.capturedPiece;
      return false;
    } else {
      tempBoard[move.startingRow][move.startingFile] = move.movedPiece;
      tempBoard[move.targetRow][move.targetFile] = move.capturedPiece;
      return true;
    }
  } else {
    return false;
  }
  return false;
}

// Zählt, wie viele Figuren den König angreifen (für Doppelschach)
int countCheckThreats(const char board[8][8], int turn, const std::map<char, Piece*>& pieceMap,
                      std::vector<std::pair<int, int>>& attackers) {
  int kingRow = -1;
  int kingFile = -1;
  char king = (turn == 0) ? 'K' : 'k';
  findKing(board, king, kingRow, kingFile);

  int threatCount = 0;
  attackers.clear();

  // alle gegnerischen Figuren prüfen, ob sie den König schlagen können
  for (int row = 0; row < 8; ++row) {
    for (int File = 0; File < 8; ++File) {
      char piece = board[row][File];

      if ((turn == 0 && std::islower(piece)) || (turn == 1 && std::isupper(piece))) {
        Move move = {row, File, kingRow, kingFile, king, piece, true};
        if (pieceMap.at(piece)->CanMove(move, const_cast<char(*)[8]>(board))) {
          threatCount++;
          attackers.emplace_back(row, File);
          if (threatCount > 1) return threatCount;  // Doppelschach → Abbruch
        }
      }
    }
  }
  return threatCount;
}

// Prüft, ob der König aus dem Schach fliehen kann
bool canKingEscape(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap) {
  int kingRow = -1;
  int kingFile = -1;
  char king = (turn == 0) ? 'K' : 'k';
  findKing(board, king, kingRow, kingFile);

  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      int newRow = kingRow + dr;
      int newFile = kingFile + dc;

      if (newRow >= 0 && newRow < 8 && newFile >= 0 && newFile < 8) {
        char targetPiece = board[newRow][newFile];
        if ((turn == 0 && !std::isupper(targetPiece)) || (turn == 1 && !std::islower(targetPiece))) {
          Move move = {kingRow, kingFile, newRow, newFile, targetPiece, king, (targetPiece != ' ') ? true : false};
          if (canMove_notCheck(board, turn, pieceMap, move)) {
            // König kann auf ein anderes Feld ziehen, ohne im Schach zu stehen
            return true;
          }
        }
      }
    }
  }
  return false;
}

// Prüft, ob die angreifende Figur geschlagen werden kann
bool canCaptureAttacker(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap,
                        std::vector<std::pair<int, int>>& attackers) {
  if (attackers.size() != 1) return false;  // Bei Doppelschach nicht möglich

  auto [attRow, attFile] = attackers[0];

  for (int row = 0; row < 8; ++row) {
    for (int File = 0; File < 8; ++File) {
      char piece = board[row][File];

      if ((turn == 0 && std::isupper(piece)) || (turn == 1 && std::islower(piece))) {
        Move move = {row, File, attRow, attFile, board[attRow][attFile], piece, true};
        if (canMove_notCheck(board, turn, pieceMap, move)) {
          return true;
        }
      }
    }
  }
  return false;
}

// Prüft, ob eine Figur zwischen König und Angreifer gezogen werden kann
bool canBlockCheck(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap, std::pair<int, int> attacker) {
  // Figur zwischen einen Knight zu stellen nicht möglich
  if (board[attacker.first][attacker.second] == 'n' || board[attacker.first][attacker.second] == 'N') {
    return false;
  }
  int kingRow = -1;
  int kingFile = -1;
  findKing(board, (turn == 0) ? 'K' : 'k', kingRow, kingFile);

  int dx = (attacker.first > kingRow) ? 1 : (attacker.first < kingRow ? -1 : 0);
  int dy = (attacker.second > kingFile) ? 1 : (attacker.second < kingFile ? -1 : 0);

  int r = kingRow + dx, c = kingFile + dy;
  while (r != attacker.first || c != attacker.second) {
    for (int row = 0; row < 8; ++row) {
      for (int File = 0; File < 8; ++File) {
        char piece = board[row][File];

        if ((turn == 0 && std::isupper(piece)) || (turn == 1 && std::islower(piece))) {
          Move move = {row, File, r, c, board[r][c], piece, false};
          if (canMove_notCheck(board, turn, pieceMap, move)) {
            return true;
          }
        }
      }
    }
    r += dx;
    c += dy;
  }
  return false;
}

// Prüft, ob Schachmatt vorliegt
bool isCheckmate(char board[8][8], int turn, const std::map<char, Piece*>& pieceMap) {
  std::vector<std::pair<int, int>> attackers;
  int checkCount = countCheckThreats(board, turn, pieceMap, attackers);

  if (checkCount == 0) return false;
  if (canKingEscape(board, turn, pieceMap)) return false;
  if (checkCount == 1 && canCaptureAttacker(board, turn, pieceMap, attackers)) return false;
  if (checkCount == 1 && canBlockCheck(board, turn, pieceMap, attackers[0])) return false;

  return true;
}
