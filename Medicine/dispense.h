#ifndef DISPENSE_H
#define DISPENSE_H
#include <QStatusBar>
#include <QSqlQuery>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QSqlDatabase>
#include <QtSql>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QWidget>
namespace Ui {
class dispense;
}

class dispense : public QWidget
{
    Q_OBJECT
public:
    explicit dispense(QWidget *parent = nullptr);
    ~dispense();
signals:
    //void poll_complete();
private:
    Ui::dispense *ui;
    QStatusBar * statusBar;
    QFont ft;
    QPalette pa;
    QList<QStandardItem*> rowItems_peiyao;
    QList<QStandardItem*> rowItems_hedui;
    QList<QStandardItem*> rowItems_fayao;
    QStandardItemModel peiyao_model;
    QStandardItemModel hedui_model;
    QStandardItemModel fayao_model;
private slots:
   void NextPatient();
   //void ReFind();
};

#endif // DISPENSE_H
