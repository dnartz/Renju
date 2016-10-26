#ifndef RENJU_NODE_H
#define RENJU_NODE_H

#include <cstdint>
#include <array>

#define BOARD_SIZE 15

namespace renju {

enum class board_status {
    forbid = -1,
    null = 0,
    white = 1,
    black = 2
};

class node {
public:
    typedef std::array<std::array<board_status, BOARD_SIZE>, BOARD_SIZE> board_t;

    node();

    node(const board_t & prev, int x, int y, board_status player);

    inline board_status trans_player(board_status player) {
        return player == board_status::white ?
               board_status::black :
               board_status::white;
    }

    inline bool has_chess(int x, int y) {
        return this->board[x][y] == board_status::white ||
               this->board[x][y] == board_status::black;
    }

    bool is_forbidden(int x, int y);
    bool is_overline(int x, int y);
    bool is_44(int x, int y);
    bool is_33(int x, int y);

private:
    board_t board;
    board_status player;
    int x, y;
};

}

#endif //RENJU_NODE_H
