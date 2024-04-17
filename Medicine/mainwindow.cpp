#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "register.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QtGui>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("56839ever");
    db.setDatabaseName("Medicine");
    if(db.open())
    {
        qDebug()<<"database establish";
    }
    else
    {
        qDebug()<<"open error";
    }
    this->setWindowTitle("中药房管理系统");
    ui->JobName->setReadOnly(true);
    ui->JobName->setStyleSheet("QLineEdit[readOnly=\"true\"] { color: black; }");
    //ui->LoginManage_2->setAutoFillBackground(true);;
    //显示已审核处方
    //QSqlTableModel reviewed_pre;
    //QSqlQuery reviewed_query;
    //reviewed_query.prepare("SELECT CFID,HZXM,SHYS,SHSJ from SH WHERE DATE(SHSJ) = CURDATE()");
    QSqlQuery reviewed_query(db);
    if(!reviewed_query.exec("SELECT CFID,HZXM,SHYS,SHSJ from sh")){
        qDebug()<<"Failed to execute reviewed_query ";
    }
    reviewed_pre.setQuery(std::move(reviewed_query));
    ui->Reviewed_Pre->showGrid();
    ui->Reviewed_Pre->show();
    reviewed_pre.setHeaderData(0, Qt::Horizontal, "处方ID");
    reviewed_pre.setHeaderData(1, Qt::Horizontal, "患者姓名");
    reviewed_pre.setHeaderData(2, Qt::Horizontal, "审核医生");
    reviewed_pre.setHeaderData(3, Qt::Horizontal, "审核时间");
    ui->Reviewed_Pre->setModel(&reviewed_pre);
    ui->Reviewed_Pre->horizontalHeader()->setStretchLastSection(true);
    ui->Reviewed_Pre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //设置药品ID栏选中即全选
    ui->Num_Pre->setFocus();
    QTimer::singleShot(0, ui->Num_Pre, &QLineEdit::selectAll);
    //每秒刷新已经审核名单
    QTimer *time_SH_re = new QTimer(this);
    time_SH_re->setInterval(1000);
    connect(time_SH_re,SIGNAL(timeout()),this,SLOT(update_SH()));
    time_SH_re->start();
    //创建re子窗口并连接返回按钮
    re  = new Register();
    connect(re,SIGNAL(backToMainWindow()),this, SLOT(reshow()));
    dis = new dispense();
    dis->show();
    //创建count子窗口并连接关闭信号
    count_window = new count();
    ui->PreInq_Show->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置只能选择一行
    //connect(ui->PreInq_Show,SIGNAL(doubleClicked();),this,SLOT(handlePreInq_ShowDoubleClick()));

    connect(count_window,SIGNAL(backToMainWindow_count()),this, SLOT(reshow_count()));

    //connect(ui->LoginManage, SIGNAL(clicked()), this, SLOT(on_LoginManage_pressed()));
    //connect(ui->JobNum,SIGNAL(&QLineEdit::editingFinished),this,SLOT(on_JobNum_editingFinished()));
    //connect(ui->Search_Pre,SIGNAL(&QLineEdit::textChanged),this,SLOT(on_Search_Pre_textChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
/***********************************
 * 按键register界面
 * *************************************/
void MainWindow::on_LoginManage_pressed()
{
    this->hide();
    re->show();
}
/***********************************
 * 按键count界面
 * *************************************/
void MainWindow::on_JobCount_pressed()
{
    this->hide();
    count_window->show();
}
/***********************************
 * 重新显示主界面
 * *************************************/
void MainWindow::reshow(){
    this->show();
}
/***********************************
 * 从count重新显示主界面
 * *************************************/
void MainWindow::reshow_count(){
    this->show();
}
/***********************************
 * 连接SQL库
 * 具体设置账户信息等
 * *************************************/
bool MainWindow::ConnectSql()
{
     QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
     db.setHostName("localhost");
     db.setUserName("root");
     db.setPassword("56839ever");
     db.setDatabaseName("Medicine");
     if(db.open())
     {
         qDebug()<<"database establish";
     }
     else
     {
         qDebug()<<"open error";
         return false;
     }
     return true;
}


/***********************************
 * 处方查询界面实际检索 判断不同功能
 * 内容更改则触发
 * *************************************/
void MainWindow::on_Search_Pre_textChanged(const QString &arg1)
{
    QString s = ui->Search_Pre->text();
    QString searchsp;
    Sp_show.setHeaderData(0, Qt::Horizontal, "处方ID");
    Sp_show.setHeaderData(1, Qt::Horizontal, "患者姓名");

    if (s.isEmpty()) {
        // 如果搜索框为空，显示所有数据
        searchsp = "SELECT CFID, HZXM FROM hiscfxxb;";
    }
    else if (s.at(0).isDigit()) {
        // 如果第一个字符是数字，则按处方ID进行模糊搜索
        searchsp = QString("SELECT CFID, HZXM FROM hiscfxxb WHERE CFID LIKE '%%1%';").arg(s);
    }
    else {
        // 否则按患者姓名进行模糊搜索
        searchsp = QString("SELECT CFID, HZXM FROM hiscfxxb WHERE HZXM LIKE '%%1%';").arg(s);
    }

    QSqlQuery sp(db);
    if (!sp.exec(searchsp)) {
        qDebug() << "Failed to execute Search_Pre!";
    }

    Sp_show.setQuery(searchsp);  // 设置查询结果到 QSqlQueryModel

    ui->PreInq_Show->showGrid();
    ui->PreInq_Show->show();
    ui->PreInq_Show->setModel(&Sp_show);
    ui->PreInq_Show->horizontalHeader()->setStretchLastSection(true);
    ui->PreInq_Show->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/*****************************
 * 定时更新审核表
 * 每一秒更新一次
******************************/
void MainWindow::update_SH(){
    QSqlQuery reviewed_query;
    if(!reviewed_query.exec("SELECT CFID,HZXM,SHYS,SHSJ from sh")){
        qDebug()<<"Failed to execute reviewed_query ";
    }
    reviewed_pre.setQuery(std::move(reviewed_query));
    ui->Reviewed_Pre->setModel(&reviewed_pre);    
}

/******************************
 * 审核完成按钮
 * 按下显示下一个处方 并统计工作量
 *
******************************/
void MainWindow::on_Rev_Complete_pressed()
{
    ui->Detail_Pre->model()->removeRows(0, ui->Detail_Pre->model()->rowCount(), QModelIndex());;
    static int currentPosition = 1;
    //解析药品清单并显示
    QSqlQuery pre_detail_query_qd_Rev(db);
    if (!pre_detail_query_qd_Rev.exec("SELECT YPQD from hiscfxxb"))
    {
         qDebug()<<"Failed to execute pre_detail_query ";
    }
    //pre_detail.setQuery(std::move(pre_detail_query));
    pre_detail_query_qd_Rev.seek(currentPosition);
    QString QD_list;
    if(1)
    {
        QD_list = pre_detail_query_qd_Rev.value(0).toString();
        int count = QD_list.length();
        QString QD_list_temp = "";
        // yypid_po_count; 记录药品清单第一个位置
        int yypid_po_count=0;
        //设置在tableview的条目
        //遍历QD信息
        QStandardItemModel pre_detail_show;
        QStandardItem pre_detail_show_Detail;
        QString yypid;
        QString ypmc;
        QString gg;
        QList<QStandardItem *> rowItems;
        for (int i = 0; i < count; ++i) {
            if (QD_list[i] == ':') {
               if(yypid_po_count == 0){
                   yypid_po_count = 1;
                   //查询药品的原药品ID
                   QSqlQuery serach_yp(db);
                   QString serach_yp_query = QString("select yypid,ypmc,gg from YPXX where yypid ='%1';").arg(QD_list_temp);
                   if(serach_yp.exec(serach_yp_query))
                   {
                      if(serach_yp.next()){
                        yypid = serach_yp.value(0).toString();
                        ypmc = serach_yp.value(1).toString();
                        gg = serach_yp.value(2).toString();
                        rowItems.append(new QStandardItem(yypid));
                        rowItems.append(new QStandardItem(ypmc));
                        rowItems.append(new QStandardItem(gg));
                        QD_list_temp.clear();
                        i = i+1;
                      }
                   }
            }
               else{
                      int num_yypid;
                      int gg_int;
                      int position = gg.indexOf('g');  // 找到'g'的位置
                      if (position != -1) {  // 确保找到了'g'
                          QString number = gg.left(position);  // 获取'g'前面的所有字符
                          gg_int = number.toInt();
                      }
                      num_yypid = QD_list_temp.toInt()/gg_int;
                      rowItems.append(new QStandardItem( QString("%1包").arg(num_yypid)));
                      pre_detail.appendRow(rowItems);
                      qDebug()<<rowItems;
                      //ui->Detail_Pre->setModel(&pre_detail);
                  }
            }
            if (QD_list[i] == ';') {
                yypid_po_count = 0;
                QD_list_temp.clear();
                rowItems.clear();
                i = i + 1;
            }
            if (i<count) {
                QD_list_temp += QD_list[i];
            }

        }
        ui->Detail_Pre->setModel(&pre_detail);
        ui->Detail_Pre->showGrid();
        ui->Detail_Pre->show();
        pre_detail.setHeaderData(0, Qt::Horizontal, "药品ID");
        pre_detail.setHeaderData(1, Qt::Horizontal, "药品名称");
        pre_detail.setHeaderData(2, Qt::Horizontal, "规格");
        pre_detail.setHeaderData(3, Qt::Horizontal, "数量");
        ui->Detail_Pre->horizontalHeader()->setStretchLastSection(true);
        ui->Detail_Pre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        //显示清单其他信息
        QSqlQuery pre_detail_query1_Rev(db);
        if (!pre_detail_query1_Rev.exec("SELECT HZXM,YSXM,TS,WS,CFID from hiscfxxb"))
        {
             qDebug()<<"Failed to execute pre_detail_query ";
        }
        pre_detail_query1_Rev.seek(currentPosition);
        //pre_detail_query1_Rev.next()
        if(1){
        //ui->Detail_Pre->setModel(&pre_detail);
            ui->Patientname->setText(pre_detail_query1_Rev.value(0).toString());
            ui->doctor->setText(pre_detail_query1_Rev.value(1).toString());
            ui->TS->setText(pre_detail_query1_Rev.value(2).toString());
            ui->WS->setText(pre_detail_query1_Rev.value(3).toString());
            ui->Num_Pre->setText(pre_detail_query1_Rev.value(4).toString());
        }
 }
    pre_detail_query_qd_Rev.next();
    currentPosition = pre_detail_query_qd_Rev.at();

}

/**************************************
 * 登陆审核并显示工号姓名
 *
****************************************/
void MainWindow::on_LoginManage_2_pressed()
{
    QString JobNum_temp;
    JobNum_temp = ui->JobNum->text();
    if (JobNum_temp.isEmpty()) {
        QMessageBox::StandardButton err = QMessageBox::warning(this, "错误", "请先登录");
    }
    else {
        QSqlQuery find_Jobnum(db);
        QString Sql_find_num =QString("SELECT count(*) from Check_Manage where YSGH ='%1';").arg(JobNum_temp);
        int shysValue;
        if(!find_Jobnum.exec(Sql_find_num)){
           qDebug() << "Failed to execute find_JobNum!";
        }
        if (find_Jobnum.next()) {
           shysValue = find_Jobnum.value(0).toInt();
        } else {
            qDebug() << "No result found!";
        }
        if(shysValue)
        {
           Sql_find_num = QString("SELECT SHYS,YSGH from Check_Manage where YSGH ='%1';").arg(JobNum_temp);
           if(!find_Jobnum.exec(Sql_find_num)){
           qDebug() << "Failed to execute find_JobNum!";
           }
           if (find_Jobnum.next()) {
               QString Job_name_show = find_Jobnum.value(0).toString();
               QString num = find_Jobnum.value(1).toString();
               ui->JobName->setText(Job_name_show);
               ui->label_11->setText(num);
               ui->label_12->setText(Job_name_show);
           }
           QSqlQuery pre_detail_query_qd(db);
           if (!pre_detail_query_qd.exec("SELECT YPQD from hiscfxxb"))
           {
                qDebug()<<"Failed to execute pre_detail_query ";
           }
           //pre_detail.setQuery(std::move(pre_detail_query));
           QString QD_list;
           if(pre_detail_query_qd.next())
           {
               QD_list = pre_detail_query_qd.value(0).toString();
               int count = QD_list.length();
               QString QD_list_temp = "";
               // yypid_po_count; 记录药品清单第一个位置
               int yypid_po_count=0;
               //设置在tableview的条目
               //遍历QD信息
               QStandardItemModel pre_detail_show;
               QStandardItem pre_detail_show_Detail;
               QString yypid;
               QString ypmc;
               QString gg;
               QList<QStandardItem *> rowItems;
               for (int i = 0; i < count; ++i) {
                   if (QD_list[i] == ':') {
                      if(yypid_po_count == 0){
                          yypid_po_count = 1;
                          //查询药品的原药品ID
                          QSqlQuery serach_yp(db);
                          QString serach_yp_query = QString("select yypid,ypmc,gg from YPXX where yypid ='%1';").arg(QD_list_temp);
                          if(serach_yp.exec(serach_yp_query))
                          {
                             if(serach_yp.next()){
                               yypid = serach_yp.value(0).toString();
                               ypmc = serach_yp.value(1).toString();
                               gg = serach_yp.value(2).toString();
                               rowItems.append(new QStandardItem(yypid));
                               rowItems.append(new QStandardItem(ypmc));
                               rowItems.append(new QStandardItem(gg));
                               QD_list_temp.clear();
                               i = i+1;
                             }
                          }
                   }
                      else{
                             int num_yypid;
                             int gg_int;
                             int position = gg.indexOf('g');  // 找到'g'的位置
                             if (position != -1) {  // 确保找到了'g'
                                 QString number = gg.left(position);  // 获取'g'前面的所有字符
                                 gg_int = number.toInt();
                             }
                             num_yypid = QD_list_temp.toInt()/gg_int;

                             rowItems.append(new QStandardItem( QString("%1包").arg(num_yypid)));
                             pre_detail.appendRow(rowItems);
                             //ui->Detail_Pre->setModel(&pre_detail);
                         }
                   }
                   if (QD_list[i] == ';') {
                       yypid_po_count = 0;
                       QD_list_temp.clear();
                       rowItems.clear();
                       i = i + 1;
                   }
                   if (i<count) {
                       QD_list_temp += QD_list[i];
                   }

               }
               ui->Detail_Pre->setModel(&pre_detail);
               ui->Detail_Pre->showGrid();
               ui->Detail_Pre->show();
               pre_detail.setHeaderData(0, Qt::Horizontal, "药品ID");
               pre_detail.setHeaderData(1, Qt::Horizontal, "药品名称");
               pre_detail.setHeaderData(2, Qt::Horizontal, "规格");
               pre_detail.setHeaderData(3, Qt::Horizontal, "数量");
               ui->Detail_Pre->horizontalHeader()->setStretchLastSection(true);
               ui->Detail_Pre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
               //显示清单其他信息
               QSqlQuery pre_detail_query1(db);
               if (!pre_detail_query1.exec("SELECT HZXM,YSXM,TS,WS,CFID from hiscfxxb"))
               {
                    qDebug()<<"Failed to execute pre_detail_query ";
               }
               if(pre_detail_query1.next()){
               //ui->Detail_Pre->setModel(&pre_detail);
               ui->Patientname->setText(pre_detail_query1.value(0).toString());
               ui->doctor->setText(pre_detail_query1.value(1).toString());
               ui->TS->setText(pre_detail_query1.value(2).toString());
               ui->WS->setText(pre_detail_query1.value(3).toString());
               ui->Num_Pre->setText(pre_detail_query1.value(4).toString());
               }
        }
        else{
           QMessageBox::StandardButton err=QMessageBox::warning(this, "错误","工号查询不到");
        }
    }
}
}

/****************************
 * 扫码枪扫码然后查询显示
 *
 * ****************************/
void MainWindow::on_Num_Pre_textChanged(const QString &arg1)
{
    ui->Detail_Pre->model()->removeRows(0, ui->Detail_Pre->model()->rowCount(), QModelIndex());;
    QString Num_Pre_temp = ui->Num_Pre->text();
    QSqlQuery pre_detail_query_qd(db);
    QString Num_Pre_temp_query = QString("SELECT YPQD from hiscfxxb where CFID = '%1';").arg(Num_Pre_temp);
    qDebug()<<"药品查询语句"<<Num_Pre_temp_query;
    if (!pre_detail_query_qd.exec(Num_Pre_temp_query))
    {
         qDebug()<<"Failed to execute pre_detail_query——更改查询 ";
    }
    QString QD_list;
    if(pre_detail_query_qd.next())
    {
        QD_list = pre_detail_query_qd.value(0).toString();
        int count = QD_list.length();
        QString QD_list_temp = "";
        // yypid_po_count; 记录药品清单第一个位置
        int yypid_po_count=0;
        //设置在tableview的条目
        //遍历QD信息
        QStandardItemModel pre_detail_show;
        QStandardItem pre_detail_show_Detail;
        QString yypid;
        QString ypmc;
        QString gg;
        QList<QStandardItem *> rowItems;
        for (int i = 0; i < count; ++i) {
            if (QD_list[i] == ':') {
               if(yypid_po_count == 0){
                   yypid_po_count = 1;
                   //查询药品的原药品ID
                   QSqlQuery serach_yp(db);
                   QString serach_yp_query = QString("select yypid,ypmc,gg from YPXX where yypid ='%1';").arg(QD_list_temp);
                   if(serach_yp.exec(serach_yp_query))
                   {
                      if(serach_yp.next()){
                        yypid = serach_yp.value(0).toString();
                        ypmc = serach_yp.value(1).toString();
                        gg = serach_yp.value(2).toString();
                        rowItems.append(new QStandardItem(yypid));
                        rowItems.append(new QStandardItem(ypmc));
                        rowItems.append(new QStandardItem(gg));
                        QD_list_temp.clear();
                        i = i+1;
                      }
                   }
            }
               else{
                      int num_yypid;
                      int gg_int;
                      int position = gg.indexOf('g');  // 找到'g'的位置
                      if (position != -1) {  // 确保找到了'g'
                          QString number = gg.left(position);  // 获取'g'前面的所有字符
                          gg_int = number.toInt();
                      }
                      num_yypid = QD_list_temp.toInt()/gg_int;

                      rowItems.append(new QStandardItem( QString("%1包").arg(num_yypid)));
                      pre_detail.appendRow(rowItems);
                  }
            }
            if (QD_list[i] == ';') {
                yypid_po_count = 0;
                QD_list_temp.clear();
                rowItems.clear();
                i = i + 1;
            }
            if (i<count) {
                QD_list_temp += QD_list[i];
            }

        }
        ui->Detail_Pre->setModel(&pre_detail);
        ui->Detail_Pre->showGrid();
        ui->Detail_Pre->show();
        pre_detail.setHeaderData(0, Qt::Horizontal, "药品ID");
        pre_detail.setHeaderData(1, Qt::Horizontal, "药品名称");
        pre_detail.setHeaderData(2, Qt::Horizontal, "规格");
        pre_detail.setHeaderData(3, Qt::Horizontal, "数量");
        ui->Detail_Pre->horizontalHeader()->setStretchLastSection(true);
        ui->Detail_Pre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        //显示清单其他信息
        QSqlQuery pre_detail_query1(db);
        QString pre_detail_query1_query = QString("SELECT HZXM,YSXM,TS,WS from hiscfxxb where CFID = '%1'").arg(Num_Pre_temp);
        if (!pre_detail_query1.exec(pre_detail_query1_query))
        {
             qDebug()<<"Failed to execute pre_detail_query ";
        }
        if(pre_detail_query1.next()){
        //ui->Detail_Pre->setModel(&pre_detail);
        ui->Patientname->setText(pre_detail_query1.value(0).toString());
        ui->doctor->setText(pre_detail_query1.value(1).toString());
        ui->TS->setText(pre_detail_query1.value(2).toString());
        ui->WS->setText(pre_detail_query1.value(3).toString());
        //ui->Num_Pre->setText(pre_detail_query1.value(4).toString());
        }
 }
}

void MainWindow::on_PreInq_Show_doubleClicked(const QModelIndex &index)
{
    QModelIndexList selectRows = ui->PreInq_Show->selectionModel()->selectedRows();
    QString firstColumnData;
    foreach(const QModelIndex &index, selectRows) {
        QVariant data = index.sibling(index.row(), 0).data(); // 获取选中行的第一列数据 CFID
        firstColumnData = data.toString();
    }
    //ui->Detail_Pre->model()->removeRows(0, ui->Detail_Pre->model()->rowCount(), QModelIndex());;
    QSqlQuery pre_detail_query_qd(db);
    QString Num_Pre_temp_query = QString("SELECT YPQD from hiscfxxb where CFID = '%1';").arg(firstColumnData);
    qDebug()<<"药品查询语句"<<Num_Pre_temp_query;
    if (!pre_detail_query_qd.exec(Num_Pre_temp_query))
    {
         qDebug()<<"Failed to execute pre_detail_query——更改查询 ";
    }
    QString QD_list;
    if(pre_detail_query_qd.next())
    {
        QD_list = pre_detail_query_qd.value(0).toString();
        int count = QD_list.length();
        QString QD_list_temp = "";
        // yypid_po_count; 记录药品清单第一个位置
        int yypid_po_count=0;
        //设置在tableview的条目
        //遍历QD信息
        QStandardItemModel pre_detail_show;
        QStandardItem pre_detail_show_Detail;
        QString yypid;
        QString ypmc;
        QString gg;
        QList<QStandardItem *> rowItems;
        for (int i = 0; i < count; ++i) {
            if (QD_list[i] == ':') {
               if(yypid_po_count == 0){
                   yypid_po_count = 1;
                   //查询药品的原药品ID
                   QSqlQuery serach_yp(db);
                   QString serach_yp_query = QString("select yypid,ypmc,gg from YPXX where yypid ='%1';").arg(QD_list_temp);
                   if(serach_yp.exec(serach_yp_query))
                   {
                      if(serach_yp.next()){
                        yypid = serach_yp.value(0).toString();
                        ypmc = serach_yp.value(1).toString();
                        gg = serach_yp.value(2).toString();
                        rowItems.append(new QStandardItem(yypid));
                        rowItems.append(new QStandardItem(ypmc));
                        rowItems.append(new QStandardItem(gg));
                        QD_list_temp.clear();
                        i = i+1;
                      }
                   }
            }
               else{
                      int num_yypid;
                      int gg_int;
                      int position = gg.indexOf('g');  // 找到'g'的位置
                      if (position != -1) {  // 确保找到了'g'
                          QString number = gg.left(position);  // 获取'g'前面的所有字符
                          gg_int = number.toInt();
                      }
                      num_yypid = QD_list_temp.toInt()/gg_int;

                      rowItems.append(new QStandardItem( QString("%1包").arg(num_yypid)));
                      pre_detail.appendRow(rowItems);
                  }
            }
            if (QD_list[i] == ';') {
                yypid_po_count = 0;
                QD_list_temp.clear();
                rowItems.clear();
                i = i + 1;
            }
            if (i<count) {
                QD_list_temp += QD_list[i];
            }

        }
        ui->Detail_Pre->setModel(&pre_detail);
        ui->Detail_Pre->showGrid();
        ui->Detail_Pre->show();
        pre_detail.setHeaderData(0, Qt::Horizontal, "药品ID");
        pre_detail.setHeaderData(1, Qt::Horizontal, "药品名称");
        pre_detail.setHeaderData(2, Qt::Horizontal, "规格");
        pre_detail.setHeaderData(3, Qt::Horizontal, "数量");
        ui->Detail_Pre->horizontalHeader()->setStretchLastSection(true);
        ui->Detail_Pre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        //显示清单其他信息
        QSqlQuery pre_detail_query1(db);
        QString pre_detail_query1_query = QString("SELECT HZXM,YSXM,TS,WS from hiscfxxb where CFID = '%1'").arg(firstColumnData);
        if (!pre_detail_query1.exec(pre_detail_query1_query))
        {
             qDebug()<<"Failed to execute pre_detail_query ";
        }
        if(pre_detail_query1.next()){
        //ui->Detail_Pre->setModel(&pre_detail);
        ui->Patientname->setText(pre_detail_query1.value(0).toString());
        ui->doctor->setText(pre_detail_query1.value(1).toString());
        ui->TS->setText(pre_detail_query1.value(2).toString());
        ui->WS->setText(pre_detail_query1.value(3).toString());
        ui->Num_Pre->setText(firstColumnData);
        }
 }
}

