#pragma once

#include "moves.h"
#include "material.h"
#include "hash.h"

class Position : public Moves {
public:
  Position() { clear(); }

  void clear() {
    in_check                   = false;
    castle_rights              = 0;
    reversible_half_move_count = 0;
    pawn_structure_key         = 0;
    key                        = 0;
    material.clear();
    last_move         = 0;
    null_moves_in_row = 0;
    transposition     = 0;
    last_move         = 0;
  }

  const uint32_t *stringToMove(const char *m) {
    int castle_type = -1;// 0 = short, 1 = long

    if (!isCastleMove(m, castle_type) && (m[0] < 'a' || m[0] > 'h' || m[1] < '1' || m[1] > '8' || m[2] < 'a' || m[2] > 'h' || m[3] < '1' || m[3] > '8'))
    {
      return NULL;
    }
    uint64_t from = 0;
    uint64_t to   = 0;

    if (castle_type == -1)
    {
      from = square(m[0] - 'a', m[1] - '1');
      to   = square(m[2] - 'a', m[3] - '1');

      // chess 960 - shredder fen
      if ((board->getPiece(from) == King && board->getPiece(to) == Rook) || (board->getPiece(from) == King + 8 && board->getPiece(to) == Rook + 8))
      {
        castle_type = to > from ? 0 : 1;// ga na
      }
    }

    if (castle_type == 0)
    {
      from = oo_king_from[side_to_move];
      to   = oo_king_to[side_to_move];
    } else if (castle_type == 1)
    {
      from = ooo_king_from[side_to_move];
      to   = ooo_king_to[side_to_move];
    }
    generateMoves();

    while (const MoveData *move_data = nextMove())
    {
      const uint32_t *move = &move_data->move;

      if (moveFrom(*move) == from && moveTo(*move) == to)
      {
        if (::isCastleMove(*move) && castle_type == -1)
        {
          continue;
        }

        if (isPromotion(*move))
        {
          if (tolower(m[strlen(m) - 1]) != piece_notation[movePromoted(*move) & 7])
          {
            continue;
          }
        }
        return move;
      }
    }
    return NULL;
  }

  bool isCastleMove(const char *m, int &castle_type) {
    if (strieq(m, "O-O") || strieq(m, "OO") || strieq(m, "0-0") || strieq(m, "00") || (strieq(m, "e1g1") && board->getPieceType(e1) == King)
        || (strieq(m, "e8g8") && board->getPieceType(e8) == King))
    {
      castle_type = 0;
      return true;
    }

    if (strieq(m, "O-O-O") || strieq(m, "OOO") || strieq(m, "0-0-0") || strieq(m, "000") || (strieq(m, "e1c1") && board->getPieceType(e1) == King)
        || (strieq(m, "e8c8") && board->getPieceType(e8) == King))
    {
      castle_type = 1;
      return true;
    }
    return false;
  }

  __forceinline int isDraw() { return flags & RECOGNIZEDDRAW; }

  int reversible_half_move_count;
  uint64_t pawn_structure_key;
  uint64_t key;
  Material material;
  int null_moves_in_row;
  int pv_length;
  uint32_t last_move;
  int eval_score;
  int transp_score;
  int transp_depth;
  int transp_type;
  uint32_t transp_move;
  int flags;
  HashEntry *transposition;
};