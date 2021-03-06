#include <iostream>
using namespace std;

namespace pgn
    {

    class PGNPlayer :
        public PGNFileReader
        {
        public:

        PGNPlayer (bool check_legal = true) :
            PGNFileReader(),
            game_(nullptr)
            {
            game_ = new Game();
            }

        virtual ~PGNPlayer ()
            {
            delete game_;
            }

        virtual void readPGNGame ()
            {
            game_->newGame(Game::kStartPosition);
            PGNFileReader::readPGNGame();
            }

        virtual void readTagPair ()
            {
            PGNFileReader::readTagPair();

            if (strieq(tag_name_, "FEN"))
                {
                game_->setFen(std::string(tag_value_).substr(1, strlen(tag_value_) - 2).c_str());
                }
            }

        virtual void readSANMove ()
            {
            PGNFileReader::readSANMove();

            int piece = (side_to_move << 3);

            if (pawn_move_)
                {
                piece |= Pawn;
                game_->pos->generatePawnMoves(capture_, bbSquare(to_square_));
                }
            else if (castle_move_)
                {
                piece |= King;
                game_->pos->generateMoves();
                }
            else if (piece_move_)
                {
                switch (from_piece_)
                    {
                    case 'N':
                        piece |= Knight;
                        break;

                    case 'B':
                        piece |= Bishop;
                        break;

                    case 'R':
                        piece |= Rook;
                        break;

                    case 'Q':
                        piece |= Queen;
                        break;

                    case 'K':
                        piece |= King;
                        break;

                    default:
                        cout << "default [" << token_str << "]" << endl;
                        exit(0);
                    }
                game_->pos->generateMoves(piece, bbSquare(to_square_));
                }
            else
                {
                cout << "else" << endl;
                exit(0);
                }
            int promoted = (side_to_move << 3);

            if (promoted_to != - 1)
                {
                switch (promoted_to)
                    {
                    case 'N':
                        promoted |= Knight;
                        break;

                    case 'B':
                        promoted |= Bishop;
                        break;

                    case 'R':
                        promoted |= Rook;
                        break;

                    case 'Q':
                        promoted |= Queen;
                        break;

                    default:
                        cout << "promoted_to error [" << token_str << "]" << endl;
                        exit(0);
                    }
                }
            bool found = false;
            int move_count = game_->pos->moveCount();

            for ( int i = 0; i < move_count; ++ i )
                {
                uint32_t m = game_->pos->move_list[i].move;

                if ((movePiece(m) != piece) || ((int)moveTo(m) != to_square_)
                    || (promoted_to != - 1 && movePromoted(m) != promoted) || (capture_ && ! isCapture(m))
                    || (from_file_ != - 1 && fileOf(moveFrom(m)) != from_file_)
                    || (from_rank_ != - 1 && rankOf(moveFrom(m)) != from_rank_))
                    {
                    continue;
                    }

                if (! game_->makeMove(m, true, true))
                    {
                    continue;
                    }
                found = true;
                break;
                }

            if (! found)
                {
                cout << "!found [" << token_str << "]" << endl;
                cout << "to_square_:" << to_square_ << endl;
                cout << "piece:" << piece << endl;
                cout << "from_file_:" << from_file_ << endl;
                cout << "from_rank_:" << from_rank_ << endl;
                cout << "pawn_move_:" << pawn_move_ << endl;
                cout << "castle_move_:" << castle_move_ << endl;
                cout << "side_to_move:" << side_to_move << endl;
                cout << "pos->in_check:" << game_->pos->in_check << endl;
                cout << "game_count_:" << game_count_ << endl;
                game_->board.print();
                exit(0);
                }
            }
        protected:
        Game * game_;
        };
    } //namespace pgn