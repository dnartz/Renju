/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   newmain.cpp
 * Author: Administrator
 *
 * Created on 2016年10月27日, 上午10:18
 */

#include <iostream>
#include <ctime>
#include <vector>
#include<stdlib.h>

#include "chess.h"

//输出上一轮落子位置和棋形
void drawMap(Map map, char m_i, char m_j)
{
    cout << "这是第" << hand + 1 << "手棋" << endl;
    cout << endl;
    cout << "人的行棋位置：O\t计算机的行棋位置：X" << endl;
    if (m_i != '0' && m_j != '0')
        cout << "落子位置：(" << m_i << ',' << m_j << ')' << endl;
    cout << "   1  2  3  4  5  6  7  8  9  A  B  C  D  E  F" << endl;
    for (int i = 0; i < 15; i++)
    {
        if (i + 1 < 10)
            cout << i + 1 << "  ";
        else
            cout << (char)(i + 1 - 10 + 'A') << "  ";
        for (int j = 0; j < 15; j++)
        {
            switch (map[i][j])
            {
                case 0: cout << "." << "  "; break;
                case 1: cout << "O"  << "  "; break;
                case 2: cout << "X"  << "  "; break;
            }
        }
        cout << endl;
    }
    cout << endl;
}

//棋盘中加入一个棋子，加入成功，返回true，否则返回false
void addToMap(Map &map, int i, int j, int whoseTurn)
{
    map[i - 1][j - 1] = whoseTurn + 1;
}

//初始化棋盘
void initMap()
{
    cout << "选择人先手（0），计算机先手（1）：";
    cin >> whoseTurn;
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            currentMap[i][j] = 0;
    if (whoseTurn == 1)
    {
        currentMap[7][7] = 2;
        whoseTurn = 0;
        drawMap(currentMap, '8', '8');
        hand++;
    }
    else
        drawMap(currentMap, '0', '0');
}

//整个程序的初始化
void init()
{
    srand((unsigned int)time(NULL));
    hand = 0;
    initMap();
    win = false;
}

//游戏的主过程
void playGame(int i, int j)
{
    addToMap(currentMap, i, j, whoseTurn);

    hand++;

    if (LinkF5(i - 1, j - 1, currentMap))
    {
        // 对手赢了
        win = 1;
        return;
    }
    whoseTurn = 1;

    cut = 0;
    computer_think();

    hand++;
    whoseTurn = 0;

    if (hand > 100)
        win = 3;
}

void computer_think()
{
    Tree *thinkTree = new Tree();
    thinkTree->father = NULL;
    thinkTree->depth = 0;
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            thinkTree->map[i][j] = currentMap[i][j];
    thinkTree->childs.clear();
    thinkTree->value = MIN;
    thinkTree->first = -1;
    thinkTree->cut = false;
    alpha_beta(thinkTree);
    if (thinkTree->first != -1)
    {
        char x, y;
        x = thinkTree->firstPoint[0] < 10 ? thinkTree->firstPoint[0] + '0' : thinkTree->firstPoint[0] - 10 + 'A';
        y = thinkTree->firstPoint[1] < 10 ? thinkTree->firstPoint[1] + '0' : thinkTree->firstPoint[1] - 10 + 'A';
        addToMap(currentMap, thinkTree->firstPoint[0], thinkTree->firstPoint[1], whoseTurn);
        drawMap(currentMap, x, y);
        if (LinkF5(thinkTree->firstPoint[0] - 1, thinkTree->firstPoint[1] - 1, currentMap))
            win = 2;
    }
    else
    {
        // 找不到下棋的位置
        win = 1;
    }
    clear(thinkTree);
}

void alpha_beta(Tree* pTree)
{
    //如果被剪枝，直接返回
    if (pTree->cut)
        return;
    //如果达到了最大深度，则计算评估值
    if (pTree->depth >= SEARCH_DEPTH)
    {
        pTree->value = total_score(pTree->map, 2 - pTree->depth % 2);
        return;
    }

    //生成待扩展的子节点，最多扩展MAX_CHILDS个。
    scanMap(pTree, 2 - pTree->depth % 2);
    //print(pTree);

    int count = 0;//和it对应的一个遍历量，记录下it的次序

    //遍历计算子节点，判断是否剪枝
    for (vector<Tree*>::iterator it = pTree->childs.begin(); it != pTree->childs.end(); it++, count++)
    {
        alpha_beta(*it);
        int v = (*it)->value;
        if (pTree->first == -1)
        {
            pTree->value = v;
            pTree->first = count;
            pTree->firstPoint[0] = (pTree->childs.at(pTree->first))->point[0];
            pTree->firstPoint[1] = (pTree->childs.at(pTree->first))->point[1];
        }
        else if (pTree->depth % 2 == 0 && v >= pTree->value)
        {
            double p = (double)rand() / (double)(RAND_MAX);
            if (v > pTree->value || p > p_accept)
            {
                pTree->value = v;
                pTree->first = count;
                pTree->firstPoint[0] = (pTree->childs.at(pTree->first))->point[0];
                pTree->firstPoint[1] = (pTree->childs.at(pTree->first))->point[1];
            }
        }
        else if (pTree->depth % 2 != 0 && v <= pTree->value)
        {
            double p = (double)rand() / (double)(RAND_MAX);
            if (v < pTree->value || p > p_accept)
            {
                pTree->value = v;
                pTree->first = count;
                pTree->firstPoint[0] = (pTree->childs.at(pTree->first))->point[0];
                pTree->firstPoint[1] = (pTree->childs.at(pTree->first))->point[1];
            }
        }

        //以下开始判断是否可以剪枝
        Tree *pF = pTree->father;
        while(pF != NULL)
        {
            if (((pTree->depth % 2 == 0 && pTree->value > pF->value) || (pTree->depth % 2 == 1 && pTree->value < pF->value))
                && pF->value != MIN && pF->value != MAX)
            {
                it++;

                //可以剪枝，标记剪枝的有关信息
                while (it != pTree->childs.end())
                {
                    (*it)->cut = true;
                    cut++;
                    it++;
                }

                return; //疑问：其实直接return就行了，因为兄弟们就不用做了，也就不用标记了。
            }
            //之前的所有祖先节点都要进行判断
            pF = pF->father;
            if (pF != NULL)
                pF = pF->father;
            else
                break;
        }
    }
}

void clear(Tree* pTree)
{
    for (vector<Tree*>::iterator it = pTree->childs.begin(); it != pTree->childs.end(); it++)
        if (*it != NULL)
            delete *it;
    delete pTree;
}

void printMap(Map map)
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
            cout << map[i][j] << ' ';
        cout << endl;
    }
}

//统计出在棋盘上的得分
int total_score(Map map, int turn)
{
    //cout << "-----------score-----------\n";
    //printMap(map);
    int score = 0;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (map[i][j] != 0)
            {
                int temp = 0;
                for (int level = 17; level > 0; level--)
                {
                    temp = PriorityGet(i, j, level, map) * level_score[level];
                    if (temp != 0)
                    {
                        score += (map[i][j] == 2) ?  temp : temp * (int)(-1);
                        break;
                    }
                }
            }
        }
    }
    return score;
}

void scanMap(Tree *&pTree, int turn)
{
    Map record;
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            record[i][j] = pTree->map[i][j];

    //按照优先级从高到低的顺序寻找MAX_CHILDS个子节点
    for(int level = 17; level > 0; level--)
    {
        //从中间开始扫描，i，j 为扫描到的坐标
        int i = 7;
        int j = 7;
        int step = 0;		//扫描过的步数
        int direct = 0;		//标记扫描的方向
        int direct_num = 1;	//在该方向上要走的步数

        while(i < 15 && j < 15 && i >= 0 && j >= 0)
        {
            if(record[i][j] == 0)
            {
                bool flag = false;
                record[i][j] = turn;
                //如果满足优先级的要求就加入到子节点的向量中
                if(PriorityGet(i, j, level, record) == 1)
                {
                    flag = true;
                    Tree *child = new Tree();
                    for (int p_i = 0; p_i < 15; p_i++)
                        for (int p_j = 0; p_j < 15; p_j++)
                            child->map[p_i][p_j] = record[p_i][p_j];
                    child->depth = pTree->depth + 1;
                    child->value = child->depth % 2 == 0 ? MIN : MAX;
                    child->first = -1;
                    child->cut = false;
                    child->father = pTree;
                    child->point[0] = i + 1;
                    child->point[1] = j + 1;
                    pTree->childs.push_back(child);
                    if (pTree->childs.size() == MAX_CHILDS)
                        return;
                }

                //有些点对方占了就会有很大优势
                if (flag == false && level > 11)
                {
                    record[i][j] = 3 - turn;
                    if(PriorityGet(i, j, level, record) == 1)
                    {
                        record[i][j] = turn;
                        Tree *child = new Tree();
                        for (int p_i = 0; p_i < 15; p_i++)
                            for (int p_j = 0; p_j < 15; p_j++)
                                child->map[p_i][p_j] = record[p_i][p_j];
                        child->depth = pTree->depth + 1;
                        child->value = child->depth % 2 == 0 ? MIN : MAX;
                        child->first = -1;
                        child->cut = false;
                        child->father = pTree;
                        child->point[0] = i + 1;
                        child->point[1] = j + 1;
                        pTree->childs.push_back(child);
                        if (pTree->childs.size() == MAX_CHILDS)
                            return;
                    }
                }
                record[i][j] = 0;
            }

            step++;
            if(step > direct_num)
            {
                direct = (direct + 1) % 4;
                if(direct % 2 == 0)
                {
                    direct_num++;
                }
                step = 1;
            }

            i += dir[direct][0];
            j += dir[direct][1];
        }
    }
}

int PriorityGet(int i, int j, int level, Map map)
{
    switch(level)
    {
        case 17:
            if(LinkF5(i, j, map))//30000
            {
                return 1;
            }
            break;
        case 16:
            if(LinkF4(i, j, map))//10000
            {
                return 1;
            }
            break;
        case 15:
            if(LinkSS4(i, j, map) > 2)//10000
            {
                return 1;
            }
            break;
        case 14:
            if(LinkS5(i, j, map))//10000
            {
                return 1;
            }
            break;
        case 13:
            if(LinkSS4(i, j, map) && LinkS4(i, j, map))//5000
            {
                return 1;
            }
            break;
        case 12:
            if(LinkSS4(i, j, map) && LinkF3(i, j, map))//5000
            {
                return 1;
            }
            break;
        case 11:
            if(LinkS4(i, j, map) && LinkF3(i, j, map))//5000
            {
                return 1;
            }
            break;
        case 10:
            if(LinkF3(i, j, map) > 2)//5000
            {
                return 1;
            }
            break;
        case 9:
            if(LinkS4(i, j, map))//500
            {
                return 1;
            }
            break;
        case 8:
            if(LinkSS4(i, j, map))//200
            {
                return 1;
            }
            break;
        case 7:
            if(LinkF3(i, j, map))//200
            {
                return 1;
            }
            break;
        case 6:
            if(LinkSS3(i, j, map) > 1)//100
            {
                return 1;
            }
            break;
        case 5:
            if(LinkS3(i, j, map) > 1)//100
            {
                return 1;
            }
            break;
        case 4:
            if(LinkF2(i, j, map) > 2)//100
            {
                return 1;
            }
            break;
        case 3:
            if(LinkF2(i, j, map))//50
            {
                return 1;
            }
            break;
        case 2:
            if(LinkSS3(i, j, map))//50
            {
                return 1;
            }
            break;
        case 1:
            if(LinkF1(i, j, map))//10
            {
                return 1;
            }
            break;
    }
    return 0;
}

int LinkSS4(int x,int y, Map map)
{
    int a[8][6]={{2,1,1,1,1,0},{2,1,0,1,1,1},{2,1,1,0,1,1},
                 {2,1,1,1,0,1},{1,2,2,2,2,0},{1,2,0,2,2,2},
                 {1,2,2,0,2,2},{1,2,2,2,0,2}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;

    num=map[x][y];
    Old_x=x;
    Old_y=y;

    for(int i = 0; i < 8; i++)
    {
        if(num != a[i][1])continue;
        for(int j = 0; j < 2;j++)
        {
            for(int k = 0; k < 4; k++){
                x = Old_x;
                y = Old_y;
                while(x>=0&&y<15&&x<15&&y>=0)
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J=5;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2)
                            A_J--;
                        else
                            A_J++;
                    }
                    if((A_J == 6 && j == 0)||(A_J < 0 && j == 1))
                        count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }

    for(int i = 0;i < 8; i++)
    {
        if(num != a[i][1])
            continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                x=Old_x;
                y=Old_y;

                while(x >= 0 && y < 15 && x < 15 && y >= 0)
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 5;
                    else A_J = 1;

                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J])
                          && (A_J > 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }

                    if((A_J == 6 && j == 0) || (A_J == 0 && j == 1))
                    {
                        if((y == 0 && M_J != 0) || (x == 0 && M_I != 0)
                           || (y == 14 && M_J < 14))count++;
                        if((M_I == 15 && x < 14)||(M_J == 15 && y < 14)) count++;
                    }
                    x += b[k][0];
                    y += b[k][1];

                }

            }
        }
    }

    return count;
}


int LinkSS3(int x,int y, Map map)
{
    int a[12][6] = {{2,1,1,1,0,0},{2,1,1,0,1,0},{2,1,1,0,0,1},
                    {2,1,0,1,1,0},{2,1,0,1,0,1},{2,1,0,0,1,1},
                    {1,2,2,2,0,0},{1,2,2,0,2,0},{1,2,2,0,0,2},
                    {1,2,0,2,2,0},{1,2,0,2,0,2},{1,2,0,0,2,2}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;

    num=map[x][y];
    Old_x=x;
    Old_y=y;


    for(int i = 0; i < 12; i++)
    {
        if(num != a[i][1]) continue;
        for(int j = 0; j < 2;j++)
        {
            for(int k = 0;k < 4;k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x <15 && y >= 0)
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 5;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J])
                          && (A_J >= 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 6 && j == 0) || (A_J < 0 && j == 1))count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }

    for(int i = 0;i < 12;i++)
    {
        if(num != a[i][1])continue;
        for(int j = 0;j < 2;j++)
        {
            for(int k = 0;k < 4; k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0)
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 5;
                    else A_J = 1;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J])
                          && (A_J>0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 6 && j == 0) || (A_J == 0 && j == 1))
                    {
                        if((y == 0 && M_J != 0) || (x == 0 && M_I != 0)
                           || (y == 14 && M_J < 14)) count++;
                        if((M_I == 15 && x < 14) || (M_J == 15 && y < 14))count++;

                    }
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }

    return count;

}


int LinkS5(int x,int y, Map map)
{
    int a[2][7]={{1,0,1,1,1,0,1},{2,0,2,2,2,0,2}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;
    num = map[x][y];
    Old_x = x;
    Old_y = y;
    for(int i = 0;i < 2; i++)
    {
        if(num != a[i][0]) continue;
        for(int j = 0;j < 2; j++)
        {
            for(int k = 0; k < 4; k++){
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 6;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 7))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 7 && j == 0) || (A_J < 0 && j == 1)) count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }

    return count;
}

int LinkS4(int x,int y, Map map)
{
    int a[2][7]={{1,0,1,0,1,0,1},{2,0,2,0,2,0,2}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;
    num = map[x][y];
    Old_x = x;
    Old_y = y;
    for(int i = 0; i < 2; i++)
    {
        if(num != a[i][0]) continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++){
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 6;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0&&A_J < 7))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 7 && j == 0) || (A_J < 0 && j == 1))count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }

    return count;
}


int LinkS3(int x,int y, Map map)
{
    int a[2][7]={{1,0,1,0,1,0,0},{2,0,2,0,2,0,0}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;
    num = map[x][y];
    Old_x = x;
    Old_y = y;
    for(int i = 0; i < 2; i++)
    {
        if(num != a[i][0]) continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 6;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 7))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 7 && j == 0) || (A_J < 0 && j == 1))count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }

    return count;
}


int LinkF5(int x,int y, Map map)
{
    int a[2][6]={{1,1,1,1,1},{2,2,2,2,2}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;

    num = map[x][y];
    Old_x = x;
    Old_y = y;

    for(int i = 0; i < 2; i++)
    {
        if(num != a[i][1]) continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0&& y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J=4;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 5 && j == 0) || ( A_J < 0 && j == 1)) count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }
    return count;
}

int LinkF4(int x,int y, Map map)
{
    int a[2][6]={{0,1,1,1,1,0},{0,2,2,2,2,0}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;

    num = map[x][y];
    Old_x = x;
    Old_y = y;

    for(int i = 0; i < 2; i++)
    {
        if(num != a[i][1]) continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 5;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 6 && j == 0) || (A_J < 0 && j == 1)) count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }
    return count;
}


int LinkF3(int x,int y, Map map)
{
    int a[4][6]={{0,1,1,0,1,0},{0,1,1,1,0,0},{0,2,2,0,2,0},{0,2,2,2,0,0}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;

    num = map[x][y];
    Old_x = x;
    Old_y = y;

    for(int i = 0; i < 4; i++)
    {
        if(num != a[i][1]) continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 5;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 6 && j == 0) || (A_J < 0 && j == 1)) count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }
    return count;
}


int LinkF2(int x,int y, Map map)
{
    int a[6][6]={{0,1,1,0,0,0},{0,1,0,1,0,0},{0,1,0,0,1,0},
                 {0,2,2,0,0,0},{0,2,0,2,0,0},{0,2,0,0,2,0}};
    int b[4][2]={{-1,0},{0,-1},{-1,-1},{-1,1}};
    int M_I,M_J,count=0,A_J,Old_x,Old_y,num;
    num = map[x][y];
    Old_x = x;
    Old_y = y;
    for(int i = 0; i < 6; i++)
    {
        if(num != a[i][1]) continue;
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                x = Old_x;
                y = Old_y;
                while(x >= 0 && y < 15 && x < 15 && y >= 0
                      && (map[x][y] == 0 || map[x][y] == num))
                {
                    M_I = x;
                    M_J = y;
                    if(j % 2) A_J = 5;
                    else A_J = 0;
                    while(M_I >= 0 && M_I < 15 && M_J >= 0 && M_J < 15
                          && (map[M_I][M_J] == a[i][A_J]) && (A_J >= 0 && A_J < 6))
                    {
                        M_I -= b[k][0];
                        M_J -= b[k][1];
                        if(j % 2) A_J--;
                        else A_J++;
                    }
                    if((A_J == 6 && j == 0) || (A_J < 0 && j == 1)) count++;
                    x += b[k][0];
                    y += b[k][1];
                }
            }
        }
    }
    return count;
}


int LinkF1(int x,int y, Map map)
{
    if(x < 15 && x >= 0 && y < 15 && y >= 0)
        return 1;
    return 0;
}
