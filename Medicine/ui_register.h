/********************************************************************************
** Form generated from reading UI file 'register.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTER_H
#define UI_REGISTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Register
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QTableView *SignView;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *JobNum;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *JobName;
    QHBoxLayout *horizontalLayout;
    QPushButton *Add;
    QPushButton *Delete;
    QPushButton *pushButton;

    void setupUi(QWidget *Register)
    {
        if (Register->objectName().isEmpty())
            Register->setObjectName(QString::fromUtf8("Register"));
        Register->resize(276, 545);
        layoutWidget = new QWidget(Register);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(11, 22, 258, 511));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        SignView = new QTableView(layoutWidget);
        SignView->setObjectName(QString::fromUtf8("SignView"));
        SignView->setSortingEnabled(true);
        SignView->setWordWrap(true);

        verticalLayout_2->addWidget(SignView);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(20);
        label->setFont(font);

        horizontalLayout_2->addWidget(label);

        JobNum = new QLineEdit(layoutWidget);
        JobNum->setObjectName(QString::fromUtf8("JobNum"));

        horizontalLayout_2->addWidget(JobNum);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        horizontalLayout_3->addWidget(label_2);

        JobName = new QLineEdit(layoutWidget);
        JobName->setObjectName(QString::fromUtf8("JobName"));

        horizontalLayout_3->addWidget(JobName);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Add = new QPushButton(layoutWidget);
        Add->setObjectName(QString::fromUtf8("Add"));
        Add->setMaximumSize(QSize(16777215, 50));
        Add->setFont(font);

        horizontalLayout->addWidget(Add);

        Delete = new QPushButton(layoutWidget);
        Delete->setObjectName(QString::fromUtf8("Delete"));
        Delete->setMaximumSize(QSize(16777215, 50));
        Delete->setFont(font);

        horizontalLayout->addWidget(Delete);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setFont(font);

        verticalLayout_2->addWidget(pushButton);


        retranslateUi(Register);

        QMetaObject::connectSlotsByName(Register);
    } // setupUi

    void retranslateUi(QWidget *Register)
    {
        Register->setWindowTitle(QCoreApplication::translate("Register", "\347\231\273\351\231\206\344\277\241\346\201\257\347\256\241\347\220\206", nullptr));
        label->setText(QCoreApplication::translate("Register", "\345\214\273\347\224\237\345\267\245\345\217\267", nullptr));
        label_2->setText(QCoreApplication::translate("Register", "\345\214\273\347\224\237\345\247\223\345\220\215", nullptr));
        Add->setText(QCoreApplication::translate("Register", "\346\267\273\345\212\240", nullptr));
        Delete->setText(QCoreApplication::translate("Register", "\345\210\240\351\231\244", nullptr));
        pushButton->setText(QCoreApplication::translate("Register", "\350\277\224\345\233\236\344\270\273\347\225\214\351\235\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Register: public Ui_Register {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTER_H
