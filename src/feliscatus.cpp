#include <string_view>
#include <algorithm>
#include <memory>

#include "feliscatus.h"
#include "game.h"
#include "search.h"
#include "protocol.h"
#include "uci.h"
#include "worker.h"
#include "perft.h"
#include "tune.h"
#include "stopwatch.h"
#include "util.h"

namespace {

constexpr std::string_view on  = "ON";
constexpr std::string_view off = "OFF";

}

Felis::Felis()
  : num_threads(1) {
}

int Felis::new_game() {
  //game->new_game(Game::kStartPosition.data());
  // TODO : test effect of not clearing
  //pawnt->clear();
  //TT.clear();
  return 0;
}

int Felis::set_fen(const std::string_view fen) { return game->new_game(fen); }

int Felis::go(const int wtime, const int btime, const int movestogo, const int winc, const int binc, const int movetime) {
  game->pos->pv_length = 0;

  if (game->pos->pv_length == 0)
    go_search(wtime, btime, movestogo, winc, binc, movetime);

  if (game->pos->pv_length)
  {
    protocol->post_moves(search->pv[0][0].move, game->pos->pv_length > 1 ? search->pv[0][1].move : 0u);
    game->make_move(search->pv[0][0].move, true, true);
  }
  return 0;
}

void Felis::ponder_hit() { search->search_time += static_cast<int>(search->start_time.elapsed_milliseconds()); }

void Felis::stop() { search->stop_search.store(true); }

bool Felis::make_move(const std::string_view m) const {
  const auto *const move = game->pos->string_to_move(m);
  return move ? game->make_move(*move, true, true) : false;
}

void Felis::go_search(const int wtime, const int btime, const int movestogo, const int winc, const int binc, const int movetime) {
  // Shared transposition table
  start_workers();
  search->go(wtime, btime, movestogo, winc, binc, movetime, num_threads);
  stop_workers();
}

void Felis::start_workers() {
  for (auto &worker : workers)
    worker.start(game.get());
}

void Felis::stop_workers() {
  for (auto &worker : workers)
    worker.stop();
}

int Felis::set_option(const std::string_view name, const std::string_view value) {
  if (!value.empty())
  {
    if (name == "Hash")
    {
      TT.init(std::clamp(util::to_integral<uint64_t>(value), 8ULL, 65536ULL));
      fmt::print("info string Hash:{}\n", TT.get_size_mb());
    } else if (name == "Threads" || name == "NumThreads")
    {
      num_threads = std::clamp(util::to_integral<uint64_t>(value), 1ULL, 64ULL);
      workers.resize(num_threads - 1);
      workers.shrink_to_fit();
      fmt::print("info string Threads:{}\n", num_threads);
    } else if (name == "UCI_Chess960")
    {
      game->chess960 = value == "true";
      fmt::print("info string UCI_Chess960:{}\n", game->chess960 ? on : off);
    } else if (name == "UCI_Chess960_Arena")
    {
      game->chess960 = game->xfen = value == "true";
      fmt::print("info string UCI_Chess960_Arena:{}\n", game->chess960 ? on : off);
    }
  }
  return 0;
}

int Felis::run() {
  setbuf(stdout, nullptr);

  game     = std::make_unique<Game>();
  protocol = std::make_unique<UCIProtocol>(this, game.get());
  pawnt    = std::make_unique<PawnHashTable>();
  search   = std::make_unique<Search>(protocol.get(), game.get(), pawnt.get());
  TT.init(256);
  
  new_game();

  auto console_mode = true;
  auto quit         = 0;
  char line[16384];

  while (quit == 0)
  {
    game->pos->generate_moves();

    (void)fgets(line, 16384, stdin);

    if (feof(stdin))
      exit(0);

    char *tokens[1024];
    const auto num_tokens = util::tokenize(util::trim(line), tokens, 1024);

    if (num_tokens == 0)
      continue;

    if (util::strieq(tokens[0], "uci") || !console_mode)
    {
      quit         = protocol->handle_input(const_cast<const char **>(tokens), num_tokens);
      console_mode = false;
    } else if (util::strieq(tokens[0], "go"))
    {
      protocol->set_flags(INFINITE_MOVE_TIME);
      go();
    } else if (util::strieq(tokens[0], "perft"))
    {
      Perft(game.get()).perft(6);
    } else if (util::strieq(tokens[0], "divide"))
    {
      Perft(game.get()).perft_divide(6);
    } else if (util::strieq(tokens[0], "tune"))
    {
      Stopwatch sw;
      eval::Tune(game.get());
      const auto seconds = sw.elapsed_seconds();
      printf("%f\n", seconds);
    } else if (util::strieq(tokens[0], "quit") || util::strieq(tokens[0], "exit"))
    {
      quit = 1;
    }

    for (auto i = 0; i < num_tokens; i++)
      delete[] tokens[i];
  }
  return 0;
}
