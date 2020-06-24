#include <fmt/format.h>
#include "perft.h"
#include "stopwatch.h"
#include "game.h"
#include "position.h"

namespace {

uint64_t p(Game *g, const int depth, const int flags) {
  if (depth == 0)
    return 1;

  g->pos->generate_moves(nullptr, MOVE_NONE, flags);

  uint64_t nodes{};

  if ((flags & STAGES) == 0 && depth == 1)
    nodes = g->pos->move_count();
  else
  {
    while (const MoveData *move_data = g->pos->next_move())
    {
      const auto *m = &move_data->move;

      if (!g->make_move(*m, flags & LEGALMOVES ? false : true, true))
        continue;

      nodes += p(g, depth - 1, flags);
      g->unmake_move();
    }
  }
  return nodes;
}

}

struct Perft final {
  Perft() = delete;
  explicit Perft(Game *game);
  explicit Perft(Game *game, int flags);

  uint64_t perft(int depth) const;

  uint64_t perft_divide(int depth) const;

private:
  Game *g{};
  int perft_flags{};
};

Perft::Perft(Game *game, const int flags) : g(game), perft_flags(flags) {}

Perft::Perft(Game *game) : g(game), perft_flags(LEGALMOVES) {}

uint64_t Perft::perft(const int depth) const {
  std::size_t nps{};

  uint64_t total_nodes{};

  for (auto i = 1; i <= depth; i++)
  {
    Stopwatch sw;
    const auto nodes = p(g, i, perft_flags);
    total_nodes += nodes;
    const auto time = sw.elapsed_milliseconds() + 1;
    nps = nodes / time * 1000;
    fmt::print("depth {}: {} nodes, {} ms, {} nps\n", i, nodes, time, nps);
  }

  return total_nodes;
}

uint64_t Perft::perft_divide(const int depth) const {
  fmt::print("depth: {}\n", depth);

  uint64_t nodes{};
  auto *pos = g->pos;
  TimeUnit time{};

  pos->generate_moves(nullptr, MOVE_NONE, perft_flags);

  while (const MoveData *move_data = pos->next_move())
  {
    const auto *const m = &move_data->move;

    if (!g->make_move(*m, perft_flags == 0, true))
      continue;

    const auto nodes_start = nodes;
    Stopwatch sw;
    nodes += p(g, depth - 1, perft_flags);
    time += sw.elapsed_milliseconds();
    g->unmake_move();
    fmt::print("move {}: {} nodes\n", g->move_to_string(*m), nodes - nodes_start);
  }

  const auto nps = nodes / (time + 1) * 1000;

  fmt::print("{} nodes, {} nps", nodes, nps);

  return nodes;
}

uint64_t perft::perft(Game *g, int depth, int flags) {
  return Perft(g, flags).perft(depth);
}

uint64_t perft::divide(Game *g, int depth, int flags) {
  return Perft(g, flags).perft_divide(depth);
}
