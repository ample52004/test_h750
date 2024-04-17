#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtGui/qstandarditemmodel.h"
#include "register.h"
#include "dispense.h"
#include "count.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool ConnectSql(void);
    //审核表定义
    QSqlTableModel reviewed_pre;

    //QSqlQuery reviewed_query;
    QStandardItemModel pre_detail;
    //QStandardItemModel pre_detail1;
    QSqlDatabase db ;

    //处方查询界面Model
    QSqlTableModel Sp_show;
    //QSqlQuery pre_detail_query;
    ~MainWindow();

private slots:
    void on_Search_Pre_textChanged(const QString &arg1);

    void reshow();

    void reshow_count();

    void update_SH();


    void on_Rev_Complete_pressed();

    void on_LoginManage_2_pressed();

    void on_LoginManage_pressed();

    void on_JobCount_pressed();


    void on_Num_Pre_textChanged(const QString &arg1);

    void on_PreInq_Show_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    Register *re;
    dispense *dis;
    count *count_window;
};
#endif // MAINWINDOW_H
//bool ConnectSql(void);
