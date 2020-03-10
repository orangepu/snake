#include "snake.h"
#include <QApplication>
#include <QPixmap>
#include <QWidget>
#include <QSplashScreen>

int main(int argc,char *argv[])
{
    QApplication a(argc,argv);
    QPixmap pixmap("logo.ico");
    QSplashScreen spalsh(pixmap);

    spalsh.show();//启动此显示图片
    a.processEvents();//是程序在显示启动画面的同时响应鼠标等其他事件

    Snake w;
    w.show();

    spalsh.finish(&w);
    return a.exec();

}
