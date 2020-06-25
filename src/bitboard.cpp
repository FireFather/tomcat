/*
  Feliscatus, a UCI chess playing engine derived from Tomcat 1.0 (Bobcat 8.0)
  Copyright (C) 2008-2016 Gunnar Harms (Bobcat author)
  Copyright (C) 2017      FireFather (Tomcat author)
  Copyright (C) 2020      Rudy Alex Kohn

  Feliscatus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Feliscatus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <string_view>

#include <fmt/format.h>

#include "bitboard.h"
#include "types.h"

std::string bitboard::print_bitboard(Bitboard b, std::string_view title) {

  fmt::memory_buffer buffer;

  constexpr std::string_view line = "+---+---+---+---+---+---+---+---+";

  if (!title.empty())
    fmt::format_to(buffer, "{}\n", title);

  fmt::format_to(buffer, "{}\n", line);

  for (const auto r : ReverseRanks)
  {
    for (const auto f : Files)
      fmt::format_to(buffer, "| {} ", b & make_square(f, r) ? "X" : " ");

    fmt::format_to(buffer, "| {}\n{}\n", std::to_string(1 + r), line);
  }
  fmt::format_to(buffer, "  a   b   c   d   e   f   g   h\n");

  return fmt::to_string(buffer);
}