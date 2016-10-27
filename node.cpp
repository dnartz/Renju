#include "node.h"

#include <iterator>

namespace renju {

node::node():
x(0),
y(0)
{
    auto it = this->board.begin(),
         end = this->board.end();

    while (it != end) {
        (it++)->fill(board_status::null);
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

    int x1, y1, x2, y2, xv2, yv2, nblack;

    // 对于棋盘上任意一个位置,它拥有8个相邻的位置(边界的位置例外)。
    // 从一个位置转移到相邻的位置,需要对行、列坐标加一,减一或者不变。
    const int dir_vector[] = {-1, 0, 1};

    for (int xv1 : dir_vector) {
        for (int yv1 : dir_vector) {
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

// TODO
bool node::is_44(int x, int y) {
    return false;
}

// TODO
bool node::is_33(int x, int y) {
    return false;
}

bool node::is_forbidden(int x, int y) {
    return this->is_overline(x, y) || this->is_44(x, y) || this->is_33(x, y);
}


    // 模式数组，用于估值函数的模式匹配。
    // 0-空闲或者禁手
    // 1-我方棋子
    // 2-对手棋子
    static std::pair<std::vector<int>, int> rate_patterns[] = {
            {{2, 1, 1, 1, 0}, 30000}
    };
int64_t node::rating() {}

bool node::find_pattern(
        int x,
        int y,
        const std::vector<int> &pattern,
        board_status color)
{
    const int dir_vector[] = {-1, 0, 1};

    for (int xv : dir_vector) {
    for (int yv: dir_vector) {

    if (yv == 0 && xv == 0) {
        continue;
    }

    for (int reverse = 0; reverse < 2; ++reverse) {
        auto it = pattern.begin(), end = pattern.end();

        int i = x, j = y, match = 0;

        auto opponent = this->trans_player(color);

        while (it != end &&
               i >= 0 && i < BOARD_SIZE &&
               j >= 0 && j < BOARD_SIZE)
        {
            if (this->board[i][j] == color && *it == 1 ||
                ((this->board[i][j] == board_status::null ||
                 this->board[i][j] == board_status::forbid &&
                 color == board_status::white) &&
                 *it == 0) ||
                (this->board[i][j] == opponent && *it == 2))
            {
                match++;
            } else {
                break;
            }

            it++;
            i += xv;
            j += yv;
        }

        if (match == pattern.size()) {
            return true;
        }
    }
    }
    }

    return false;
}

}