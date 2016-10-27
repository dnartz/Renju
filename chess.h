#ifndef RENJU_CHESS_H
#define RENJU_CHESS_H

#include <ctime>
#include <vector>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    void initMap();
    void playGame(int i, int j);
    int win;					//记录是不是已经分出胜负，0表示人赢了，1表示计算机赢了

#ifdef __cplusplus
}

using namespace std;

typedef int Map[15][15];

const int MIN = -1000000;
const int MAX = 1000000;
const int MAX_CHILDS = 10;
const int SEARCH_DEPTH = 4;
const int dir[4][2] = {{0,  1}, {-1, 0}, {0, -1}, {1,  0}};
const int level_score[18] = {0, 10,   50,   50,   100,    100,    200,   500, 1000, 1000, 2000,    2000,      5000 ,     5000,     5000,   10000, 10000, 80000};
// 活一，冲三，活二，多活二，双跳三，双死三，活三，冲四，跳4，双活三，跳四+活三，冲四+活三，冲四+跳四，双跳冲四，双冲4，活4，  成5
const double p_accept = 0.3;	// 在评估函数值相等时接受新的解的概率

struct Tree
{
    vector<Tree*> childs;	//记录子女节点
    Tree* father;			//记录父节点
    int depth;				//记录层次深度
    int value;				//记录当前计算的值
    Map map;				//这一步的棋形
    int first;				//记录当前节点首选的下一步的节点的名字
    int firstPoint[2];		//下一步走的坐标
    int point[2];			//处于这个节点时和上一步相比增加的走步
    bool cut;				//标记这个节点是否被剪枝
};

Map currentMap;				//记录当前的棋形,0表示还没有棋子，1表示人的棋子，2表示计算机的棋子
int whoseTurn;				//0表示到人走，1表示到计算机走
int hand;					//记录一共走了几手棋。
int cut;					//记录一共剪了多少次枝

void drawMap(Map, int, int);
void addToMap(Map &, int, int, int);
void computer_think();
void alpha_beta(Tree*);
void clear(Tree*);
void scanMap(Tree *&pTree, int turn);

int POW(int s, int t);
int score_row(int i, int j, Map map);
int score_col(int i, int j, Map map);
int score_diag_45(int i, int j, Map map);
int score_diag_135(int i, int j, Map map);
int total_score(Map map, int turn);

int PriorityGet(int i, int j, int level, Map record);
int LinkSS4(int x,int y, Map map);
/*211110 or 211101 or 211011*/
/*210111 or 11110 or 11101 */
/* 11011 or 10111*/
int LinkSS3(int x,int y, Map map);
/*211100 or 211010 or 211001*/
/*210110 or 210101 or 21011*/
/*11100 or 10110 or 10011 */
/*10101*/
int LinkS5(int x,int y, Map map);	/*1011101*/
int LinkS4(int x,int y, Map map);	/*1010101*/
int LinkS3(int x,int y, Map map);	/*1010100*/
int LinkF5(int x,int y, Map map);	/*11111*/
int LinkF4(int x,int y, Map map);	/*11110*/
int LinkF3(int x,int y, Map map);	/*011100 or 010110*/
int LinkF2(int x,int y, Map map);	/*011000 or 010100 or 010010*/
int LinkF1(int x,int y, Map map);	/*1*/
#endif

#endif
