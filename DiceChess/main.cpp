
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <tuple>
#include <utility>

#include "./chess.h"
#include "./dice.h"

int main(int argc, char** argv) {
  // Figuren-Mapping
  std::map<char, Piece*> pieceMap = {{'P', new Pawn()},   {'p', new Pawn()},   {'R', new Rook()},   {'r', new Rook()},
                                     {'N', new Knight()}, {'n', new Knight()}, {'B', new Bishop()}, {'b', new Bishop()},
                                     {'Q', new Queen()},  {'q', new Queen()},  {'K', new King()},   {'k', new King()}};

  char board[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}, {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                      {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                      {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'}, {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

  int turn = 0, moveCount = 0;
  Move history[1000];
  std::string input;
  bool Checkmate = false;
  // Seed setzen
  srand(time(NULL));

  // Variablen für das Würfelspiel
  char mustBeNextPiece = 'x';
  std::string mustBeNextDirection = "y";
  bool chooseFrozen = false;
  char frozenPiece = 'z';
  bool chooseSquare = false;
  int frozenRow = 10;
  int frozenFile = 10;
  int tmp = -1;
  bool PawnsLikeKnights = false;
  bool PawnLikeKnight = false;

  // Status für Rochade und En Passant
  bool whiteCanCastleKingSide = true;
  bool whiteCanCastleQueenSide = true;
  bool blackCanCastleKingSide = true;
  bool blackCanCastleQueenSide = true;
  int enPassantTarget = -1;

  printBoard(board);
  std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";

  // Spiel beginnen
  while (std::getline(std::cin, input)) {
    // Inputüberprüfung
    if (input.empty()) continue;
    if (Checkmate) {
      break;
    }
    // Möglichkeit, im Notfall den Angaben des Würfelspiels auszuweichen
    if (input[0] == 'x') {
      undoChanges(mustBeNextPiece, mustBeNextDirection, chooseFrozen, frozenPiece, chooseSquare, frozenRow, frozenFile,
                  1 - turn, tmp, PawnsLikeKnights, PawnLikeKnight);
      std::cout << (turn == 0 ? "White" : "Black") << ", enter next move without any conditions: ";
      continue;
    }
    // Möglichkeit, die Figurenkonstellation des Boards neu zu definieren
    if (input[0] == 'V') {
      if (input.length() < 65) continue;
      int idx = 1;
      // Board überschreiben
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
          board[i][j] = input[idx++];
        }
      }
      printBoard(board);
      turn = 0;
      moveCount = 0;
      if (isCheckmate(board, turn, pieceMap)) {
        std::cout << "Checkmate! " << (turn == 1 ? "White" : "Black") << " wins." << std::endl;
      } else {
        std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
      }
      continue;
    }

    // DiceCheck ob Würfelvorgaben erfüllt sind
    if (badDiceCheck(input, mustBeNextPiece, chooseFrozen, frozenPiece, chooseSquare, frozenRow, frozenFile, turn,
                     tmp)) {
      continue;
    }

    // Inputverarbeitung
    bool castle = true;
    bool rochade = isRochade(input);
    if (input.length() < (rochade ? 3 : 5)) {
      std::cout << "invalid" << std::endl;
      continue;
    }
    bool capture = iscapture(input);
    char piece = rochade ? (turn == 0 ? 'K' : 'k') : input[0];
    int startingFile = rochade ? 4 : input[1] - 'a';
    int startingRow = rochade ? (turn == 0 ? 7 : 0) : 7 - (input[2] - '1');
    bool promotion = ispromotion(input);
    char promotingPiece = promotion ? input[capture ? 7 : 6] : ' ';
    int targetFile = rochade ? 0 : input[capture ? 4 : 3] - 'a';
    int targetRow = rochade ? 0 : 7 - (input[capture ? 5 : 4] - '1');

    Move move = {startingRow, startingFile, targetRow, targetFile, board[targetRow][targetFile], piece, capture};

    // Überprüfungen der Eingabe
    if ((!promotion && (piece == 'p' || piece == 'P') && (targetRow == 0 || targetRow == 7)) ||
        (promotion && !PromotionCheck(input, move, turn, promotingPiece))) {
      promotion = false;
      std::cout << "invalid promotion" << std::endl;
      continue;
    }
    if (!isMoveInCorrectDirection(mustBeNextDirection, startingRow, startingFile, targetRow, targetFile)) {
      continue;
    }
    if (startingRow < 0 || startingRow >= 8 || startingFile < 0 || startingFile >= 8 ||
        board[startingRow][startingFile] != piece || (turn == 0 && !isWhite(piece)) || (turn == 1 && isWhite(piece))) {
      std::cout << "invalid" << std::endl;
      std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
      continue;
    }
    if (isSquareFrozen(chooseSquare, targetRow, targetFile, frozenRow, frozenFile, turn, tmp)) {
      continue;
    } else if (PawnsLikeKnights && (piece == 'P' || piece == 'p')) {
      if (!canPawnMoveLikeKnight(turn, piece, board, targetRow, targetFile)) {
        continue;
      }
      PawnLikeKnight = true;
      piece = (turn == 0 ? 'N' : 'n');
    }

    // Zugüberprüfung und Ausführung
    if (isEnPassant(move, enPassantTarget) || pieceMap[piece]->CanMove(move, board) || isRochade(input)) {
      if (PawnLikeKnight && PawnsLikeKnights) {
        turn == 0 ? piece = 'P' : piece = 'p';
      }
      if (isEnPassant(move, enPassantTarget)) {
        board[startingRow][targetFile] = ' ';
      }
      if (isRochade(input)) {
        if (input[0] == '0' && input[1] == '-' && input[2] == '0') {
          if (input.length() == 3) {
            makeCastleMove(startingFile, startingRow, "0-0", board, whiteCanCastleKingSide, whiteCanCastleQueenSide,
                           blackCanCastleKingSide, blackCanCastleQueenSide, pieceMap, castle);
          } else if (input.length() == 5) {
            makeCastleMove(startingFile, startingRow, "0-0-0", board, whiteCanCastleKingSide, whiteCanCastleQueenSide,
                           blackCanCastleKingSide, blackCanCastleQueenSide, pieceMap, castle);
          }
        }
      } else {
        // Zugausführung
        history[moveCount++] = move;
        board[targetRow][targetFile] = promotion ? promotingPiece : piece;
        board[startingRow][startingFile] = ' ';
      }
      if (!castle) {
        std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
        continue;
      } else {
        turn = 1 - turn;
      }

      // Überprüfung, falls Spieler noch im Schach, Zug rückgängig machen
      if (isKingInCheck(board, 1 - turn, pieceMap)) {
        if (PawnLikeKnight) {
          turn == 0 ? piece = 'P' : piece = 'p';
          PawnsLikeKnights = true;
          PawnLikeKnight = false;
        }
        std::cout << "invalid" << std::endl;
        turn = 1 - turn;
        std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
        if (rochade) {
          continue;
        }

        moveCount--;
        Move lastMove = history[moveCount];
        board[lastMove.startingRow][lastMove.startingFile] = lastMove.movedPiece;
        board[lastMove.targetRow][lastMove.targetFile] = lastMove.capturedPiece;

        continue;
      }
      // falls Spieler nicht im Schach, nächsten Zug vorbereiten
      else {
        printBoard(board);
        PawnsLikeKnights = false;
        PawnLikeKnight = false;

        if (isCheckmate(board, turn, pieceMap)) {
          Checkmate = true;
          std::cout << "Checkmate! " << (turn == 1 ? "White" : "Black") << " wins." << std::endl;
          continue;
        } else if (isKingInCheck(board, turn, pieceMap)) {
          std::cout << "Check!" << std::endl;
        } else {
          std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
        }
      }
    } else {  // Zug nicht möglich
      if (PawnsLikeKnights && PawnLikeKnight) {
        turn == 0 ? piece = 'P' : piece = 'p';
        PawnLikeKnight = false;
      }
      std::cout << "invalid" << std::endl;
      if (!Checkmate) {
        std::cout << (turn == 0 ? "White" : "Black") << ", enter next move: ";
      }
      continue;
    }

    // Würfeländerungen rückgängig machen
    undoChanges(mustBeNextPiece, mustBeNextDirection, chooseFrozen, frozenPiece, chooseSquare, frozenRow, frozenFile,
                turn, tmp, PawnsLikeKnights, PawnLikeKnight);

    // Rochaderechte verlieren, wenn der König oder die Türme bewegt werden
    if (piece == 'K') whiteCanCastleKingSide = whiteCanCastleQueenSide = false;
    if (piece == 'k') blackCanCastleKingSide = blackCanCastleQueenSide = false;
    if (piece == 'R' && startingRow == 7) {
      if (startingFile == 0) whiteCanCastleQueenSide = false;
      if (startingFile == 7) whiteCanCastleKingSide = false;
    }
    if (piece == 'r' && startingRow == 0) {
      if (startingRow == 0) blackCanCastleQueenSide = false;
      if (startingRow == 7) blackCanCastleKingSide = false;
    }

    // random Runde, ist anpassbar je nach Frequenz der Würfelrunden
    if ((1 == rand() % 3) && !isKingInCheck(board, turn, pieceMap)) {
      std::vector<char> White;
      std::vector<char> Black;
      std::vector<std::pair<int, int>> WhitePositions;
      std::vector<std::pair<int, int>> BlackPositions;
      // alle möglichen Figuren der Spieler finden
      for (int row = 0; row < 8; ++row) {
        for (int File = 0; File < 8; ++File) {
          char piece = board[row][File];
          if (piece != ' ') {
            if (turn == 0 && isWhite(piece)) {
              White.emplace_back(piece);
              WhitePositions.emplace_back(row, File);
            } else if (turn == 1 && !isWhite(piece)) {
              Black.emplace_back(piece);
              BlackPositions.emplace_back(row, File);
            }
          }
        }
      }
      std::cout << "Dice-Time!" << std::endl;
      int randomNumber = rand() % 6 + 1;

      // Würfelspiel ausführen
      handleDiceRoll(randomNumber, White, Black, WhitePositions, BlackPositions, mustBeNextPiece, mustBeNextDirection,
                     chooseFrozen, frozenPiece, chooseSquare, frozenRow, frozenFile, turn, tmp, board, PawnsLikeKnights,
                     PawnLikeKnight, pieceMap);
    }
  }

  // Speicher freigeben für Piece-Objekte
  for (auto& pair : pieceMap) {
    delete pair.second;
  }

  return 0;
}
