#ifndef SNAKE_H
#define SNAKE_H
#pragma execution_character_set("utf-8")


//枚举Label
enum Label
{
    NORMAL_LABEL,//普通类型，背景
    BORDER_LABEL,//边界
    SNAKE_LABEL,//蛇节点
    FOOD_LABEL,//食物
};

#include"connect.h"

#include <QMainWindow>
#include <QLabel>
#include <QList>
#include <QWidget>
#include <QPushButton>
#include <QTimer>


class Snake :public QMainWindow
{
    Q_OBJECT
public:
    explicit Snake (QWidget *parent=0 );//表明Snake没有父窗口
    void init();                        //初始化游戏
    void drawBorder();                  //画边界
    void initSnake();                   //初始化蛇身
    void drawViewArea();                //分数区域
    void moveSnake();                   //移动蛇
    void getHeadTail();                 //获取头指针
    void createFood();                  //创建食物
    void gameover();                    //游戏结束
    void showViewArea();                //显示分数
    bool up();                          //判断能否上下左右
    bool down();
    bool left();
    bool right();
    bool decoder(QByteArray);

    QTimer timer;
    QPushButton *startGame;
    ~Snake();

private slots:
    void snakeMoveSlots();
    void startGameSlots();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    snakeNode *matrix[MAX_X][MAX_Y];    //存储游戏画面
    QList<snakeNode*> snake;

    int level,score;                    //分数等级
    int foodCount,moveSpeed;            //食物数 移动速度
    int directionX,directionY;
    QLabel *mLabel;                     //level score标签
    QString viewText;                   //显示区域文本

    snakeNode *head;
    snakeNode *tail;               //蛇头尾指针
    bool canChangeDirection;
    class connect Connect;
};



#endif // SNAKE_H
