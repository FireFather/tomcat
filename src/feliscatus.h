#pragma once

#include <memory>
#include <vector>
#include "protocol.h"
#include "worker.h"

struct PawnHashTable;
class Search;

struct Felis final : public ProtocolListener {
  Felis();

  int new_game() override;

  int set_fen(std::string_view fen) override;

  int go(const SearchLimits &limits) override;

  void ponder_hit() override;

  void stop() override;

  bool make_move(std::string_view m) const;

  void go_search(const SearchLimits &limits);

  void start_workers();

  void stop_workers();

  bool set_option(std::string_view name, std::string_view value) override;

  int run(int argc, char* argv[]);

private:
  std::unique_ptr<Game> game;
  std::unique_ptr<Search> search;
  std::unique_ptr<UCIProtocol> protocol;
  std::unique_ptr<PawnHashTable> pawnt;
  std::vector<Worker> workers{};
  std::size_t num_threads;
};
