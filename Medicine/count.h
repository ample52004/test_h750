#ifndef COUNT_H
#define COUNT_H
#include <QSqlDatabase>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>
#include <QWidget>
#include <QTimer>
namespace Ui {
class count;
}

class count : public QWidget
{
    Q_OBJECT

public:
    explicit count(QWidget *parent = nullptr);
    QSqlDatabase m_db;
    ~count();
signals:
    void backToMainWindow_count();

private slots:
    void on_lineEdit_returnPressed();

    void on_lineEdit_3_editingFinished();

private:
    Ui::count *ui;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // COUNT_H
