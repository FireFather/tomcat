#pragma once

#include <cmath>
#include <cstdint>
#include <array>

namespace squares {


enum Square {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8,
  no_square
};

constexpr std::array<Square, 64> Squares
  { a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
  };

constexpr std::array<uint32_t, 2> oo_allowed_mask{1, 4};

constexpr std::array<uint32_t, 2> ooo_allowed_mask{2, 8};

static Square oo_king_from[2];
constexpr std::array<Square, 2> oo_king_to{g1, g8};

static Square ooo_king_from[2];
constexpr std::array<Square, 2> ooo_king_to{c1, c8};

static Square rook_castles_to[64];  // indexed by position of the king
static Square rook_castles_from[64];// also
static std::array<int, 64> castle_rights_mask{};
static uint32_t dist[64][64];// chebyshev distance
static Square flip[2][64];

constexpr int rank_of(const Square sq) {
  return sq >> 3;
}

constexpr int file_of(const Square sq) {
  return sq & 7;
}

constexpr Square square(const int f, const int r) {
  return static_cast<Square>((r << 3) + f);
}

constexpr bool is_dark(const Square sq) {
  return ((9 * sq) & 8) == 0;
}

constexpr bool same_color(const Square sq1, const Square sq2) {
  return is_dark(sq1) == is_dark(sq2);
}


static void init() {
  for (const auto sq : Squares)
  {
    flip[0][sq] = static_cast<Square>(file_of(sq) + ((7 - rank_of(sq)) << 3));
    flip[1][sq] = static_cast<Square>(file_of(sq) + (rank_of(sq) << 3));
  }

  for (const auto sq1 : Squares)
  {
    for (const auto sq2 : Squares)
    {
      const uint32_t ranks = std::abs(rank_of(sq1) - rank_of(sq2));
      const uint32_t files = std::abs(file_of(sq1) - file_of(sq2));
      dist[sq1][sq2]       = std::max(ranks, files);
    }
  }

  for (auto side = 0; side <= 1; side++)
  {
    rook_castles_to[flip[side][g1]] = flip[side][f1];
    rook_castles_to[flip[side][c1]] = flip[side][d1];
  }
  // Arrays castle_right_mask, rook_castles_from, ooo_king_from and oo_king_from
  // are initd in method setupCastling of class Game.
}

inline const char *square_to_string(const Square sq, char *buf) {
  sprintf(buf, "%c%d", static_cast<char>(file_of(sq) + 'a'), rank_of(sq) + 1);
  return buf;
}
}// namespace squares

using namespace squares;