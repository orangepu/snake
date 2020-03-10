#include"connect.h"


connect::connect(){
    first = true;
    QString IP = "175.24.77.134";
    int port = 1024;
    socket = new QTcpSocket;

    //取消已有的连接
    socket->abort();
    //连接服务器
    socket->connectToHost(IP, port);

    //等待连接成功
    if(!socket->waitForConnected(30000))
    {
        qDebug() << "Connection failed!";
        return;
    }
    qDebug() << "Connect successfully!";

}

QByteArray connect::receive(){
    QByteArray buffer;
    //读取缓冲区数据
    if(socket->isReadable())
        buffer = socket->readAll();
    return buffer;
}

void connect::sendSnake(QList<snakeNode*> snakeThis){
    QByteArray str;
    for(auto it:snakeThis)
    {
        str+=char(it->x);
        str+=char(it->y);
    }

    int body_length = str.size()+1;
    char temp_header[header_length+2]="";
    sprintf(temp_header, "%4d", body_length);
    temp_header[header_length]='1';
    str.insert(0,temp_header);

    socket->write(str);
    socket->flush();
}

void connect::sendFood(snakeNode *head)
{
    QByteArray str;
    str+=char(head->x);
    str+=char(head->y);
    int body_length = str.size()+1;
    char temp_header[header_length+2]="";
    sprintf(temp_header, "%4d", body_length);
    temp_header[header_length]='0';
    str.insert(0,temp_header);
    qDebug()<<str;
    socket->write(str);
    socket->flush();
}

void connect::esc(){
    socket->close();
}

bool connect::isFirst()
{
    return first;
}

void connect::changeFirst()
{
    if(first)
        first=false;
    else {
        first=true;
    }
}

connect::~connect(){
    delete socket;
}
