/********************************************************************************
** Form generated from reading UI file 'form_motorwidget.ui'
**
** Created: Mon Jul 11 15:14:50 2016
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_MOTORWIDGET_H
#define UI_FORM_MOTORWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MotorWidgetObject
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupbox;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *absDisplay;
    QPushButton *minus;
    QLineEdit *relDisplay;
    QPushButton *plus;
    QLabel *Pos;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_4;
    QLineEdit *posDisplay;

    void setupUi(QWidget *MotorWidgetObject)
    {
        if (MotorWidgetObject->objectName().isEmpty())
            MotorWidgetObject->setObjectName(QString::fromUtf8("MotorWidgetObject"));
        MotorWidgetObject->resize(261, 102);
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(61, 61, 61, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush1);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush1);
        MotorWidgetObject->setPalette(palette);
        layoutWidget = new QWidget(MotorWidgetObject);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 261, 101));
        verticalLayout_3 = new QVBoxLayout(layoutWidget);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        groupbox = new QGroupBox(layoutWidget);
        groupbox->setObjectName(QString::fromUtf8("groupbox"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupbox->sizePolicy().hasHeightForWidth());
        groupbox->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        groupbox->setFont(font);
        groupbox->setAlignment(Qt::AlignCenter);
        gridLayout = new QGridLayout(groupbox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(4);
        gridLayout->setContentsMargins(9, 6, -1, 6);
        horizontalSpacer_2 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 4, 1, 1);

        absDisplay = new QLineEdit(groupbox);
        absDisplay->setObjectName(QString::fromUtf8("absDisplay"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(absDisplay->sizePolicy().hasHeightForWidth());
        absDisplay->setSizePolicy(sizePolicy1);
        absDisplay->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(absDisplay, 1, 3, 1, 1);

        minus = new QPushButton(groupbox);
        minus->setObjectName(QString::fromUtf8("minus"));
        sizePolicy.setHeightForWidth(minus->sizePolicy().hasHeightForWidth());
        minus->setSizePolicy(sizePolicy);
        minus->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(minus, 2, 2, 1, 1);

        relDisplay = new QLineEdit(groupbox);
        relDisplay->setObjectName(QString::fromUtf8("relDisplay"));
        sizePolicy1.setHeightForWidth(relDisplay->sizePolicy().hasHeightForWidth());
        relDisplay->setSizePolicy(sizePolicy1);
        relDisplay->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(relDisplay, 2, 3, 1, 1);

        plus = new QPushButton(groupbox);
        plus->setObjectName(QString::fromUtf8("plus"));
        sizePolicy.setHeightForWidth(plus->sizePolicy().hasHeightForWidth());
        plus->setSizePolicy(sizePolicy);
        plus->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(plus, 2, 4, 1, 1);

        Pos = new QLabel(groupbox);
        Pos->setObjectName(QString::fromUtf8("Pos"));
        sizePolicy.setHeightForWidth(Pos->sizePolicy().hasHeightForWidth());
        Pos->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Pos, 0, 2, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 4, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 1, 2, 1, 1);

        posDisplay = new QLineEdit(groupbox);
        posDisplay->setObjectName(QString::fromUtf8("posDisplay"));
        sizePolicy.setHeightForWidth(posDisplay->sizePolicy().hasHeightForWidth());
        posDisplay->setSizePolicy(sizePolicy);
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush2(QColor(27, 13, 139, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        QBrush brush3(QColor(170, 170, 255, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush3);
        posDisplay->setPalette(palette1);
        posDisplay->setFrame(false);
        posDisplay->setAlignment(Qt::AlignCenter);
        posDisplay->setReadOnly(true);

        gridLayout->addWidget(posDisplay, 0, 3, 1, 1);


        verticalLayout_3->addWidget(groupbox);


        retranslateUi(MotorWidgetObject);

        QMetaObject::connectSlotsByName(MotorWidgetObject);
    } // setupUi

    void retranslateUi(QWidget *MotorWidgetObject)
    {
        MotorWidgetObject->setWindowTitle(QApplication::translate("MotorWidgetObject", "Form", 0, QApplication::UnicodeUTF8));
        groupbox->setTitle(QApplication::translate("MotorWidgetObject", "Slit_x1", 0, QApplication::UnicodeUTF8));
        minus->setText(QString());
        plus->setText(QString());
        Pos->setText(QApplication::translate("MotorWidgetObject", "Position", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MotorWidgetObject: public Ui_MotorWidgetObject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_MOTORWIDGET_H
