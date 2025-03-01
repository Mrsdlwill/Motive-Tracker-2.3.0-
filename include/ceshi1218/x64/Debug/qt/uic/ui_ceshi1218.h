/********************************************************************************
** Form generated from reading UI file 'ceshi1218.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CESHI1218_H
#define UI_CESHI1218_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ceshi1218Class
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *connectBtn;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *lineEdit;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *textEdit;
    QTextEdit *dataEdit;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *filePathEdit;
    QPushButton *findPath;
    QPushButton *saveDataButton;
    QPushButton *clearData;

    void setupUi(QMainWindow *ceshi1218Class)
    {
        if (ceshi1218Class->objectName().isEmpty())
            ceshi1218Class->setObjectName("ceshi1218Class");
        ceshi1218Class->resize(700, 569);
        centralWidget = new QWidget(ceshi1218Class);
        centralWidget->setObjectName("centralWidget");
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(centralWidget);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        connectBtn = new QPushButton(widget);
        connectBtn->setObjectName("connectBtn");

        horizontalLayout->addWidget(connectBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout->addWidget(lineEdit);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        textEdit = new QTextEdit(widget_2);
        textEdit->setObjectName("textEdit");

        horizontalLayout_2->addWidget(textEdit);

        dataEdit = new QTextEdit(widget_2);
        dataEdit->setObjectName("dataEdit");

        horizontalLayout_2->addWidget(dataEdit);


        verticalLayout->addWidget(widget_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        filePathEdit = new QLineEdit(centralWidget);
        filePathEdit->setObjectName("filePathEdit");

        horizontalLayout_3->addWidget(filePathEdit);

        findPath = new QPushButton(centralWidget);
        findPath->setObjectName("findPath");

        horizontalLayout_3->addWidget(findPath);

        saveDataButton = new QPushButton(centralWidget);
        saveDataButton->setObjectName("saveDataButton");

        horizontalLayout_3->addWidget(saveDataButton);

        clearData = new QPushButton(centralWidget);
        clearData->setObjectName("clearData");

        horizontalLayout_3->addWidget(clearData);


        verticalLayout->addLayout(horizontalLayout_3);

        ceshi1218Class->setCentralWidget(centralWidget);

        retranslateUi(ceshi1218Class);

        QMetaObject::connectSlotsByName(ceshi1218Class);
    } // setupUi

    void retranslateUi(QMainWindow *ceshi1218Class)
    {
        ceshi1218Class->setWindowTitle(QCoreApplication::translate("ceshi1218Class", "ceshi1218", nullptr));
        connectBtn->setText(QCoreApplication::translate("ceshi1218Class", "\350\277\236\346\216\245", nullptr));
        lineEdit->setText(QCoreApplication::translate("ceshi1218Class", "127.0.0.1", nullptr));
        filePathEdit->setText(QCoreApplication::translate("ceshi1218Class", "Select Your path to save data File", nullptr));
        findPath->setText(QCoreApplication::translate("ceshi1218Class", "\346\265\217\350\247\210", nullptr));
        saveDataButton->setText(QCoreApplication::translate("ceshi1218Class", "\345\274\200\345\247\213\345\257\274\345\207\272", nullptr));
        clearData->setText(QCoreApplication::translate("ceshi1218Class", "\346\270\205\347\251\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ceshi1218Class: public Ui_ceshi1218Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CESHI1218_H
