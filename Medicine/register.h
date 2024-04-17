#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDebug>
#include <QtSql>
namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT
public:
    explicit Register(QWidget *parent = nullptr);
    QSqlDatabase m_db;
    ~Register();

signals:
    void backToMainWindow();

private slots:

    void on_Add_pressed();

    void on_Delete_pressed();

    void on_pushButton_pressed();

private:
    Ui::Register *ui;
    QSqlTableModel model;
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // REGISTER_H
