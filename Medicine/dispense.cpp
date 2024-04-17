#include "dispense.h"
#include "ui_dispense.h"

dispense::dispense(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dispense)
{
    ui->setupUi(this);
    statusBar = new QStatusBar();
    QLabel *wait_mes = new QLabel("未显示的患者请耐心等待！",this);
    //设置字体大小
    ft.setPointSize(20);
    wait_mes->setFont(ft);
    //设置警告字体颜色
    pa.setColor(QPalette::WindowText,Qt::red);
    wait_mes->setPalette(pa);
    statusBar->addWidget(wait_mes);
    //叫号未取栏
//    QLabel *call = new QLabel("叫号未取：",this);
//    call->setFont(ft);
//    statusBar->addWidget(call);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(1);  // 在控件之前添加一个伸缩项
    layout->addWidget(statusBar);
    peiyao_model.setColumnCount(1);
    hedui_model.setColumnCount(1);
    fayao_model.setColumnCount(1);
    peiyao_model.setRowCount(10);
    hedui_model.setRowCount(10);
    peiyao_model.setRowCount(10);
    QLabel *uncatch= new QLabel("",this);
    uncatch->setFont(ft);
    //配药查询 还要加状态表 下面同理

    QSqlQuery peiyao_query;
    if(!peiyao_query.exec("SELECT HZXM from hiscfxxb")){
        qDebug()<<"Unable to find uncatch";
    }
    int size_peiyao;
    if(peiyao_query.size()<10){
        size_peiyao = peiyao_query.size();
    }
    else {
        size_peiyao =10;
    }
    int rowCounter_peiyao = 0;
    for (int i = 0; i < size_peiyao; ++i){
        if(peiyao_query.next()){
            QString value = peiyao_query.value(0).toString();
            QStandardItem *item = new QStandardItem(value);
            item->setTextAlignment(Qt::AlignCenter);
            peiyao_model.setItem(i,0,item);
        }
    }
    //具体细节还需要商议

    ui->tableView->setModel(&peiyao_model);
    ui->tableView->show();

    //核对查询
    QSqlQuery hedui_query;
    if(!hedui_query.exec("SELECT HZXM from hiscfxxb")){
        qDebug()<<"Unable to find uncatch";
    }
    int size_hedui;
    if(hedui_query.size()<10){
        size_hedui = hedui_query.size();
    }
    else {
        size_hedui =10;
    }
    for (int g = 0; g <size_hedui; ++g){
        if(hedui_query.next()){
        QString value_hedui = hedui_query.value(0).toString();
        QStandardItem *item = new QStandardItem(value_hedui);
        item->setTextAlignment(Qt::AlignCenter);
        hedui_model.setItem(g,0,item);
        }
    }

    ui->tableView_2->setModel(&hedui_model);
    ui->tableView_2->show();

    //发药查询
    QSqlQuery find_uncatch;
    if(!find_uncatch.exec("SELECT HZXM from hiscfxxb")){
        qDebug()<<"Unable to find uncatch";
    }
    int size_fayao;
    if(find_uncatch.size()<10){
        size_fayao = find_uncatch.size();
    }
    else {
        size_fayao =10;
    }
//    int rowCounter_fayao = 0;
    for (int j = 0; j < size_fayao; ++j){
        if (find_uncatch.next()) {
            QString value_fayao = find_uncatch.value(0).toString();
            QStandardItem *item = new QStandardItem(value_fayao);
            item->setTextAlignment(Qt::AlignCenter);
            fayao_model.setItem(j,0,item);
        }
    }

    ui->tableView_3->setModel(&fayao_model);
    ui->tableView_3->show();
    //设置定时器刷新
    QTimer *time_uncatch = new QTimer();
    //每隔10s查询一次数据库
    time_uncatch->setInterval(1000);
    connect(time_uncatch,SIGNAL(timeout()),this,SLOT(NextPatient()));
//    call->setFont(ft);
    time_uncatch->start();

    //connect(this,SIGNAL(poll_complete),this,SLOT(ReFind()));
}
dispense::~dispense()
{
    delete ui;
}

void dispense::NextPatient(){
    ui->tableView->model()->removeRows(0, ui->tableView->model()->rowCount(), QModelIndex());;
    ui->tableView_2->model()->removeRows(0, ui->tableView_2->model()->rowCount(), QModelIndex());;
    ui->tableView_3->model()->removeRows(0, ui->tableView_3->model()->rowCount(), QModelIndex());;
    peiyao_model.clear();
    hedui_model.clear();
    fayao_model.clear();

    QSqlQuery peiyao_query;
    if(!peiyao_query.exec("SELECT HZXM from hiscfxxb")){
        qDebug()<<"Unable to find uncatch";
    }
    int size_peiyao;
    if(peiyao_query.size()<10){
        size_peiyao = peiyao_query.size();
    }
    else {
        size_peiyao =10;
    }
    int rowCounter_peiyao = 0;
    for (int i = 0; i < size_peiyao; ++i){
        if(peiyao_query.next()){
            QString value = peiyao_query.value(0).toString();
            QStandardItem *item = new QStandardItem(value);
            item->setTextAlignment(Qt::AlignCenter);
            peiyao_model.setItem(i,0,item);
        }
    }
    //具体细节还需要商议

    ui->tableView->setModel(&peiyao_model);
    ui->tableView->show();

    //核对查询
    //hedui_model = new QStandardItemModel(0,1,this);
    QSqlQuery hedui_query;
    if(!hedui_query.exec("SELECT HZXM from hiscfxxb")){
        qDebug()<<"Unable to find uncatch";
    }
    int size_hedui;
    if(hedui_query.size()<10){
        size_hedui = hedui_query.size();
    }
    else {
        size_hedui =10;
    }
    for (int i1 = 0; i1 < size_hedui; ++i1){
        if(hedui_query.next()){
        QString value_hedui = hedui_query.value(0).toString();
        QStandardItem *item = new QStandardItem(value_hedui);
        item->setTextAlignment(Qt::AlignCenter);
        hedui_model.setItem(i1,0,item);
        }
    }

    ui->tableView_2->setModel(&hedui_model);
    ui->tableView_2->show();

    //发药查询
    QSqlQuery find_uncatch;
    if(!find_uncatch.exec("SELECT HZXM from hiscfxxb")){
        qDebug()<<"Unable to find uncatch";
    }
    int size_fayao;
    if(find_uncatch.size()<10){
        size_fayao = find_uncatch.size();
    }
    else {
        size_fayao =10;
    }
//    int rowCounter_fayao = 0;
    for (int j = 0; j < size_fayao; ++j){
        if (find_uncatch.next()) {
            QString value_fayao = find_uncatch.value(0).toString();
            QStandardItem *item = new QStandardItem(value_fayao);
            item->setTextAlignment(Qt::AlignCenter);
            fayao_model.setItem(j,0,item);
        }
    }
    ui->tableView_3->setModel(&fayao_model);
    ui->tableView_3->show();
}

    //emit poll_complete();
//void dispense::ReFind(){
//    QLabel *uncatch = new QLabel("",this);
//    uncatch->setFont(ft);
//    QSqlQuery find_uncatch;
//    find_uncatch.prepare("SELECT HZXM from hiscfxxb ORDER BY SCSJ");
//    if(!find_uncatch.exec()){
//        qDebug()<<"Unable to find uncatch";
//    }
//    while(find_uncatch.next()){
//        QThread::msleep(10000);
//        int currentRow = find_uncatch.at();
//        QString rowString = QString::number(currentRow);
//        uncatch->setText(rowString);
//    }
//    emit poll_complete();
//}
