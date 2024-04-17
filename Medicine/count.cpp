#include "count.h"
#include "ui_count.h"
#include "mainwindow.h"
#include <QDateTime>
count::count(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::count)
{

    MainWindow *mainWindow = dynamic_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        // 获取 MainWindow 中的 db 对象
        m_db = mainWindow->db;
    }
    ui->setupUi(this);
    //姓名不可编辑
    ui->lineEdit_2->setReadOnly(true);
    //设置药品ID栏选中即全选
    ui->lineEdit_3->setFocus();
    QTimer::singleShot(0, ui->lineEdit_3, &QLineEdit::selectAll);
}

count::~count()
{
    delete ui;
}
/***********************************
 * 查询验证工号
 * **********************************/
void count::on_lineEdit_returnPressed()
{
    QString JobNum_temp;
    JobNum_temp = ui->lineEdit->text();
    QSqlQuery find_Jobnum(m_db);
    QString Sql_find_num = QString("SELECT SHYS from Check_Manage where YSGH ='%1';").arg(JobNum_temp);
    if(!find_Jobnum.exec(Sql_find_num)){
        qDebug() << "Failed to execute find_JobNum!";
        QMessageBox::StandardButton result=QMessageBox::warning( this,"错误","查询不到工号");
    }
    if (find_Jobnum.next()) {
        QString Job_name_show = find_Jobnum.value(0).toString();
        ui->lineEdit_2->setText(Job_name_show);
    }
}

void count::closeEvent(QCloseEvent *event)
{
    // 执行关闭窗口前的操作
    // 调用父类的closeEvent()函数，执行默认的窗口关闭操作
    this->hide();
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    emit backToMainWindow_count();
}
/***********************************
 * 查询验证CFID是否重复
 * 扫码枪输入触发
 * 还没有验证
 * **********************************/
void count::on_lineEdit_3_editingFinished()
{
    QString Num_pre_temp;
    Num_pre_temp = ui->lineEdit_3->text();
    QString YSGH_temp = ui->lineEdit->text();
    QSqlQuery find_Num_pre_temp(m_db);
    QString TS_temp;
    QString WS_temp;
    QString Sql_find_Num_pre_temp_query = QString("SELECT count(*) from Count_Manage where CFID ='%1';").arg(Num_pre_temp);
    qDebug()<<"查询语句"+Sql_find_Num_pre_temp_query;
    if (!Num_pre_temp.isEmpty()) {
        if(!find_Num_pre_temp.exec(Sql_find_Num_pre_temp_query)){

            qDebug() << "Failed to execute Num_pre_temp_query!";        }
        if (find_Num_pre_temp.next()) {
            qDebug()<<"执行成功";
            if (find_Num_pre_temp.value(0).toInt()>=1) {
                 QMessageBox::StandardButton result=QMessageBox::warning( this,"错误","订单重复");
            }
            else{
                QSqlQuery Search_Num_pre_temp(m_db);
                QString Search_find_Num_pre_temp_query = QString("SELECT TS,WS from hiscfxxb where CFID = '%1';").arg(Num_pre_temp);
                qDebug()<<"Search_find_Num_pre_temp_query语句"+Search_find_Num_pre_temp_query;
                if (Search_Num_pre_temp.exec(Search_find_Num_pre_temp_query)) {
                    qDebug()<<"Search_find_Num_pre_temp_query 执行成功";
                    if (Search_Num_pre_temp.next()) {
                        TS_temp = Search_Num_pre_temp.value(0).toString();
                        WS_temp = Search_Num_pre_temp.value(1).toString();
                        qDebug()<< "TS = "+TS_temp;
                        qDebug()<< "WS = "+WS_temp;
                    }
                }
                else {
                    qDebug()<<"Search_find_Num_pre_temp_query 执行失败";
                }
                QDateTime currentTime = QDateTime::currentDateTime();
                QSqlQuery Insert_Num_pre_temp(m_db);
                QString Insert_find_Num_pre_temp_query = QString("INSERT INTO Count_Manage(YSGH,TS,WS,SHSJ,CFID) VALUES ('%1','%2','%3','%4','%5');")\
                        .arg(YSGH_temp,TS_temp,WS_temp,currentTime.toString("yyyy-MM-dd HH:mm:ss"),Num_pre_temp);
                if (!Insert_Num_pre_temp.exec(Insert_find_Num_pre_temp_query)) {
                    qDebug()<<"插入失败";
                }
                else {
                     qDebug()<<"插入成功";
                }
            }
        }
        else {
            QMessageBox::StandardButton result=QMessageBox::warning( this,"错误","请扫订单号");
        }
    }

}

