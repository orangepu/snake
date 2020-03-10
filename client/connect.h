
#ifndef CONNECT_H
#define CONNECT_H
#include<QTcpSocket>
#include<Qstring>
#include<QLabel>
//区域大小

const int MAX_X=150;
const int MAX_Y=120;
const int BLOCK_SIZE=8;
const int MAX_FOOD=30;
const int header_length=4;

//蛇的节点
class snakeNode
{
public:
    QLabel *label;
    int type;
    int x;
    int y;
};

struct snakeBody{
    int x;
    int y;
};

class connect{
private:
    int matrix[MAX_X][MAX_Y];
    QTcpSocket *socket;
    bool first;

public:
    connect();
    ~connect();
    void sendSnake(QList<snakeNode*> snakeThis);
    void sendFood(snakeNode *head);
    QByteArray receive();
    void esc();
    bool isFirst();
    void changeFirst();

};


#endif // CONNECT_H
