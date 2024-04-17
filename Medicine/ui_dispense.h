/********************************************************************************
** Form generated from reading UI file 'dispense.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPENSE_H
#define UI_DISPENSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dispense
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QLabel *label;
    QTableView *tableView;
    QSplitter *splitter_2;
    QLabel *label_2;
    QTableView *tableView_2;
    QSplitter *splitter_3;
    QLabel *label_3;
    QTableView *tableView_3;

    void setupUi(QWidget *dispense)
    {
        if (dispense->objectName().isEmpty())
            dispense->setObjectName(QString::fromUtf8("dispense"));
        dispense->resize(855, 547);
        widget = new QWidget(dispense);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(20, 30, 790, 491));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        splitter = new QSplitter(widget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        label = new QLabel(splitter);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(30);
        label->setFont(font);
        label->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        splitter->addWidget(label);
        tableView = new QTableView(splitter);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        splitter->addWidget(tableView);

        horizontalLayout->addWidget(splitter);

        splitter_2 = new QSplitter(widget);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        label_2 = new QLabel(splitter_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        splitter_2->addWidget(label_2);
        tableView_2 = new QTableView(splitter_2);
        tableView_2->setObjectName(QString::fromUtf8("tableView_2"));
        splitter_2->addWidget(tableView_2);

        horizontalLayout->addWidget(splitter_2);

        splitter_3 = new QSplitter(widget);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Vertical);
        label_3 = new QLabel(splitter_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        splitter_3->addWidget(label_3);
        tableView_3 = new QTableView(splitter_3);
        tableView_3->setObjectName(QString::fromUtf8("tableView_3"));
        splitter_3->addWidget(tableView_3);

        horizontalLayout->addWidget(splitter_3);


        retranslateUi(dispense);

        QMetaObject::connectSlotsByName(dispense);
    } // setupUi

    void retranslateUi(QWidget *dispense)
    {
        dispense->setWindowTitle(QCoreApplication::translate("dispense", "\344\270\255\350\215\211\350\215\257\347\252\227\345\217\243", nullptr));
        label->setText(QCoreApplication::translate("dispense", "\346\255\243\345\234\250\351\205\215\350\215\257", nullptr));
        label_2->setText(QCoreApplication::translate("dispense", "\346\255\243\345\234\250\346\240\270\345\257\271", nullptr));
        label_3->setText(QCoreApplication::translate("dispense", "\345\217\221\350\215\257\345\217\253\345\217\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dispense: public Ui_dispense {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPENSE_H
