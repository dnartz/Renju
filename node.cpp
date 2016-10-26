#include "node.h"

namespace renju {

node::node():
x(0),
y(0)
{
    auto begin = this->board.begin(),
         end = this->board.end();

    while (begin < end) {
        (begin++)->fill(board_status::null);
    }
}

node::node(const node::board_t & prev, int x, int y, board_status player):
board(prev),
x(x),
y(y),
player(player)
{
    this->board[x][y] = this->player;
}

// 判断是否为连五以上的禁手。
bool node::is_overline(int x, int y) {
    if (this->has_chess(x, y)) {
        return false;
    }

    bool result = false;

    this->board[x][y] = board_status::black;

    int x1, y1, x2, y2, xv1, yv1, xv2, yv2, nblack;

    // 对于棋盘上任意一个位置,它拥有8个相邻的位置(边界的位置例外)。
    // 从一个位置转移到相邻的位置,需要对行、列坐标加一,减一或者不变。
    const int dir_vector[] = {-1, 0, 1};

    for (xv1 : dir_vector) {
        for (yv1 : dir_vector) {
            // 两个方向向量都为0的情况显然应该略过。
            // 由于对称性的原因,我们只要遍历xv1和yv1全部排列中的一半即可。
            if (yv1 < xv1 || (xv1 == 0 && yv1 == 0)) {
                continue;
            }

            xv2 = -xv1;
            yv2 = -yv1;

            x1 = x2 = x;
            y1 = y2 = y;

            // 下面两个while分别是正反两个方向的遍历。
            // 两次遍历都是从原点开始,计数时会多加一次,因此我们用-1初始化,抵消一次。
            nblack = -1;
            while (0 <= x1 && x1 < BOARD_SIZE &&
                   0 <= y1 && y1 < BOARD_SIZE &&
                   this->board[x1][y1] == board_status::black)
            {
                nblack += 1;
                x1 += xv1;
                y1 += yv1;
            }

            while (0 <= x2 && x1 < BOARD_SIZE &&
                   0 <= y2 && y2 < BOARD_SIZE &&
                   this->board[x2][y2] == board_status::black)
            {
                nblack += 1;
                x2 += xv2;
                y2 += yv2;
            }

            if (nblack > 5) {
                goto finish;
            }
        }
    }

    finish:
        this->board[x][y] = board_status::null;
        return result;
}

bool node::is_forbidden(int x, int y) {
    return this->is_overline(x, y) || this->is_44(x, y) || this->is_33(x, y);
}

}