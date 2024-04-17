/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_13;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *JobNum;
    QLineEdit *JobName;
    QVBoxLayout *verticalLayout_9;
    QPushButton *LoginManage_2;
    QPushButton *LoginManage;
    QVBoxLayout *verticalLayout_11;
    QHBoxLayout *horizontalLayout_7;
    QSplitter *splitter;
    QLabel *label_13;
    QLineEdit *Search_Pre;
    QTableView *PreInq_Show;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_14;
    QTableView *Reviewed_Pre;
    QVBoxLayout *verticalLayout_17;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLineEdit *Num_Pre;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QLabel *Patientname;
    QLabel *label_5;
    QLabel *doctor;
    QLabel *label_6;
    QLabel *TS;
    QLabel *Weishu;
    QLabel *WS;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_6;
    QPushButton *JobCount;
    QPushButton *Rev_Complete;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_10;
    QLabel *label_9;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_11;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_9;
    QTableView *Detail_Pre;
    QTableView *Rev_Detail_Pre;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(999, 665);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(5, 20, 981, 611));
        horizontalLayout_13 = new QHBoxLayout(layoutWidget);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(20);
        label->setFont(font);

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        verticalLayout_2->addWidget(label_2);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        JobNum = new QLineEdit(layoutWidget);
        JobNum->setObjectName(QString::fromUtf8("JobNum"));

        verticalLayout_3->addWidget(JobNum);

        JobName = new QLineEdit(layoutWidget);
        JobName->setObjectName(QString::fromUtf8("JobName"));

        verticalLayout_3->addWidget(JobName);


        horizontalLayout_3->addLayout(verticalLayout_3);


        horizontalLayout_6->addLayout(horizontalLayout_3);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        LoginManage_2 = new QPushButton(layoutWidget);
        LoginManage_2->setObjectName(QString::fromUtf8("LoginManage_2"));
        LoginManage_2->setFont(font);
        LoginManage_2->setMouseTracking(false);
        LoginManage_2->setCheckable(false);
        LoginManage_2->setAutoRepeat(false);
        LoginManage_2->setAutoExclusive(false);
        LoginManage_2->setAutoDefault(false);

        verticalLayout_9->addWidget(LoginManage_2);

        LoginManage = new QPushButton(layoutWidget);
        LoginManage->setObjectName(QString::fromUtf8("LoginManage"));
        LoginManage->setFont(font);
        LoginManage->setMouseTracking(false);
        LoginManage->setAutoRepeat(false);
        LoginManage->setAutoExclusive(false);
        LoginManage->setAutoDefault(false);

        verticalLayout_9->addWidget(LoginManage);


        horizontalLayout_6->addLayout(verticalLayout_9);


        verticalLayout_10->addLayout(horizontalLayout_6);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        splitter = new QSplitter(layoutWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        label_13 = new QLabel(splitter);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setFont(font);
        label_13->setAlignment(Qt::AlignCenter);
        splitter->addWidget(label_13);
        Search_Pre = new QLineEdit(splitter);
        Search_Pre->setObjectName(QString::fromUtf8("Search_Pre"));
        splitter->addWidget(Search_Pre);

        horizontalLayout_7->addWidget(splitter);

        PreInq_Show = new QTableView(layoutWidget);
        PreInq_Show->setObjectName(QString::fromUtf8("PreInq_Show"));

        horizontalLayout_7->addWidget(PreInq_Show);

        horizontalLayout_7->setStretch(1, 3);

        verticalLayout_11->addLayout(horizontalLayout_7);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        label_14 = new QLabel(layoutWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setFont(font);
        label_14->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(label_14);

        Reviewed_Pre = new QTableView(layoutWidget);
        Reviewed_Pre->setObjectName(QString::fromUtf8("Reviewed_Pre"));

        verticalLayout_7->addWidget(Reviewed_Pre);


        verticalLayout_11->addLayout(verticalLayout_7);

        verticalLayout_11->setStretch(0, 1);
        verticalLayout_11->setStretch(1, 3);

        verticalLayout_10->addLayout(verticalLayout_11);

        verticalLayout_10->setStretch(1, 1);

        horizontalLayout_13->addLayout(verticalLayout_10);

        verticalLayout_17 = new QVBoxLayout();
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        horizontalLayout_2->addWidget(label_3);

        Num_Pre = new QLineEdit(layoutWidget);
        Num_Pre->setObjectName(QString::fromUtf8("Num_Pre"));

        horizontalLayout_2->addWidget(Num_Pre);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font);

        horizontalLayout->addWidget(label_4);

        Patientname = new QLabel(layoutWidget);
        Patientname->setObjectName(QString::fromUtf8("Patientname"));
        Patientname->setFont(font);

        horizontalLayout->addWidget(Patientname);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);

        horizontalLayout->addWidget(label_5);

        doctor = new QLabel(layoutWidget);
        doctor->setObjectName(QString::fromUtf8("doctor"));
        doctor->setFont(font);

        horizontalLayout->addWidget(doctor);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font);

        horizontalLayout->addWidget(label_6);

        TS = new QLabel(layoutWidget);
        TS->setObjectName(QString::fromUtf8("TS"));
        TS->setFont(font);

        horizontalLayout->addWidget(TS);

        Weishu = new QLabel(layoutWidget);
        Weishu->setObjectName(QString::fromUtf8("Weishu"));
        Weishu->setFont(font);

        horizontalLayout->addWidget(Weishu);

        WS = new QLabel(layoutWidget);
        WS->setObjectName(QString::fromUtf8("WS"));
        WS->setFont(font);

        horizontalLayout->addWidget(WS);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_17->addLayout(verticalLayout);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_8);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        JobCount = new QPushButton(layoutWidget);
        JobCount->setObjectName(QString::fromUtf8("JobCount"));
        QFont font1;
        font1.setPointSize(13);
        JobCount->setFont(font1);

        verticalLayout_6->addWidget(JobCount);

        Rev_Complete = new QPushButton(layoutWidget);
        Rev_Complete->setObjectName(QString::fromUtf8("Rev_Complete"));
        Rev_Complete->setFont(font1);

        verticalLayout_6->addWidget(Rev_Complete);


        horizontalLayout_5->addLayout(verticalLayout_6);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_10 = new QLabel(layoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setFont(font1);

        verticalLayout_4->addWidget(label_10);

        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setFont(font1);

        verticalLayout_4->addWidget(label_9);


        horizontalLayout_4->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setFont(font1);

        verticalLayout_5->addWidget(label_11);

        label_12 = new QLabel(layoutWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font1);

        verticalLayout_5->addWidget(label_12);


        horizontalLayout_4->addLayout(verticalLayout_5);


        horizontalLayout_5->addLayout(horizontalLayout_4);


        horizontalLayout_8->addLayout(horizontalLayout_5);


        verticalLayout_8->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        Detail_Pre = new QTableView(layoutWidget);
        Detail_Pre->setObjectName(QString::fromUtf8("Detail_Pre"));

        horizontalLayout_9->addWidget(Detail_Pre);

        Rev_Detail_Pre = new QTableView(layoutWidget);
        Rev_Detail_Pre->setObjectName(QString::fromUtf8("Rev_Detail_Pre"));

        horizontalLayout_9->addWidget(Rev_Detail_Pre);


        verticalLayout_8->addLayout(horizontalLayout_9);


        verticalLayout_17->addLayout(verticalLayout_8);

        verticalLayout_17->setStretch(0, 1);
        verticalLayout_17->setStretch(1, 6);

        horizontalLayout_13->addLayout(verticalLayout_17);

        horizontalLayout_13->setStretch(0, 1);
        horizontalLayout_13->setStretch(1, 2);
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        statusbar->setEnabled(true);
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        LoginManage_2->setDefault(false);
        LoginManage->setDefault(false);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\270\255\350\215\257\346\210\277\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\214\273\347\224\237\345\267\245\345\217\267\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\345\214\273\347\224\237\345\247\223\345\220\215\357\274\232", nullptr));
        LoginManage_2->setText(QCoreApplication::translate("MainWindow", "\347\231\273\351\231\206", nullptr));
        LoginManage->setText(QCoreApplication::translate("MainWindow", "\346\263\250\345\206\214", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "\345\244\204\346\226\271\346\237\245\350\257\242", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "\345\267\262\345\256\241\346\240\270\345\244\204\346\226\271", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\345\244\204\346\226\271\347\274\226\345\217\267\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215\357\274\232", nullptr));
        Patientname->setText(QString());
        label_5->setText(QCoreApplication::translate("MainWindow", "\345\214\273\347\224\237\357\274\232", nullptr));
        doctor->setText(QString());
        label_6->setText(QCoreApplication::translate("MainWindow", "\350\264\264\346\225\260\357\274\232", nullptr));
        TS->setText(QString());
        Weishu->setText(QCoreApplication::translate("MainWindow", "\345\221\263\346\225\260\357\274\232", nullptr));
        WS->setText(QString());
        label_8->setText(QCoreApplication::translate("MainWindow", "\345\244\204\346\226\271\350\257\246\345\215\225", nullptr));
        JobCount->setText(QCoreApplication::translate("MainWindow", "\345\267\245\344\275\234\351\207\217\347\273\237\350\256\241", nullptr));
        Rev_Complete->setText(QCoreApplication::translate("MainWindow", "\345\256\241\346\240\270\345\256\214\346\210\220", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\345\214\273\347\224\237\345\267\245\345\217\267\357\274\232", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\345\214\273\347\224\237\345\247\223\345\220\215\357\274\232", nullptr));
        label_11->setText(QString());
        label_12->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
