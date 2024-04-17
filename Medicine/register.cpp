#include "register.h"
#include "ui_register.h"
#include "mainwindow.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>
//#include "ui_mainwindow.h"
Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    //连接信号与槽

    //连接数据库显示表头信息
    //数据库查询语句
    //Check_Manage 审核人员管理名单数据库，主键为JobNum 工号
    MainWindow *mainWindow = dynamic_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        // 获取 MainWindow 中的 db 对象
        m_db = mainWindow->db;
    }
    QSqlQuery query(m_db);
    if (!query.exec("SELECT YSGH,SHYS from Check_Manage")) {
        qDebug() << "Failed to execute query!";
    }
    //创建模型
    model.setQuery(std::move(query));
    ui->SignView->setModel(&model);
    //ui->SignView->show();
    model.setHeaderData(0, Qt::Horizontal, "工号");
    model.setHeaderData(1, Qt::Horizontal, "姓名");
    //统一提交更改
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    //调整tableView布局
    ui->SignView->showGrid();
    ui->SignView->horizontalHeader()->setStretchLastSection(true);
    ui->SignView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //只能单行操作
    ui->SignView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SignView->show();
    //connect(ui->pushButton,SIGNAL(clicked()), this, SLOT(on_pushButton_pressed()));
    //connect(ui->Add,SIGNAL(clicked()),this,SLOT(on_Add_pressed()));
    //connect(ui->Delete,SIGNAL(clicked()),this,SLOT(on_Delete_pressed()));


}
Register::~Register()
{
    delete ui;
}

void Register::closeEvent(QCloseEvent *event)
{
    // 执行关闭窗口前的操作
    // 调用父类的closeEvent()函数，执行默认的窗口关闭操作
    this->hide();
    ui->JobNum->clear();
    ui->JobName->clear();
    emit backToMainWindow();
}


void Register::on_Add_pressed()
{
    MainWindow *mainWindow = dynamic_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        // 获取 MainWindow 中的 db 对象
        m_db = mainWindow->db;
    }
    QString Jobnum_temp = ui->JobNum->text();
    QString Jobname_temp = ui->JobName->text();
    if(!Jobnum_temp.isEmpty()){
        if(!Jobname_temp.isEmpty()){
            Jobname_temp = ui->JobName->text();
            QString insert_query = QString("SELECT count(*) from Check_Manage where YSGH ='%1';").arg(Jobnum_temp);
            qDebug()<<insert_query;
            QString insert_query1;
            QSqlQuery add_exm(m_db);
            QSqlQuery add(m_db);
            QSqlQuery query(m_db);
            if(!add_exm.exec(insert_query)){
                qDebug() << "Failed to execute 查询总量!";
            }
            qDebug()<<"第一次输入"<<add_exm.value(0).toInt();
            if (add_exm.next()) {
                 qDebug()<<"第2次输入"<<add_exm.value(0).toInt();
                if (add_exm.value(0).toInt()>=1)
                {
                    QMessageBox::StandardButton result=QMessageBox::warning( this,"错误","工号重复");
                }
                else {
                    insert_query1 = QString("INSERT INTO Check_Manage(SHYS,YSGH) VALUES ('%1','%2');").arg(Jobname_temp,Jobnum_temp);
                    if(add.exec(insert_query1)){
                        if (!query.exec("SELECT YSGH,SHYS from Check_Manage")) {
                            qDebug() << "Failed to execute query!";
                        }
                        //创建模型
                        model.setQuery(std::move(query));
                        ui->SignView->setModel(&model);
                    }
                }
            }
        }
        else {
            QMessageBox::StandardButton err = QMessageBox::warning(this, "错误", "请输入姓名");
        }
    }
    else {
        QMessageBox::StandardButton err = QMessageBox::warning(this, "错误", "请输入工号");
    }
}
void Register::on_Delete_pressed()
{
    MainWindow *mainWindow = dynamic_cast<MainWindow*>(parentWidget());
    if (mainWindow) {
        // 获取 MainWindow 中的 db 对象
        m_db = mainWindow->db;
    }
    QModelIndexList selectRows = ui->SignView->selectionModel()->selectedRows();
    QString firstColumnData;
    foreach(const QModelIndex &index, selectRows) {
        QVariant data = index.sibling(index.row(), 0).data(); // 获取选中行的第一列数据
        firstColumnData = data.toString();
    }
    QString deleteQuery = QString("DELETE FROM Check_Manage WHERE YSGH = '%1' LIMIT 1;").arg(firstColumnData);
    QSqlQuery query(m_db);
    if(query.exec(deleteQuery)) {
        //QSqlQuery query(m_db);
        if (!query.exec("SELECT YSGH,SHYS from Check_Manage")) {
            qDebug() << "Failed to execute query!";
        }
        //创建模型
        model.setQuery(std::move(query));
        ui->SignView->setModel(&model);
        //ui->SignView->show();
        model.setHeaderData(0, Qt::Horizontal, "工号");
        model.setHeaderData(1, Qt::Horizontal, "姓名");
    } else {
        qDebug()<<"工号删除失败";
    }
}


void Register::on_pushButton_pressed()
{
    this->hide();
    ui->JobNum->clear();
    ui->JobName->clear();
    emit backToMainWindow();
}

