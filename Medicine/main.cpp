#include "mainwindow.h"
#include <QApplication>
#include <QSqlDatabase>
//#include "register.h"
#include <iostream>
#include <string>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //ConnectSql();
    std::string open = "Sql Open\n";
    w.show();
    //connect()
    std::cout<<open;

    return a.exec();
}

