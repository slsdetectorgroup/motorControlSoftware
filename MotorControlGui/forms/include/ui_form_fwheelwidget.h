/********************************************************************************
** Form generated from reading UI file 'form_fwheelwidget.ui'
**
** Created: Mon May 2 15:12:20 2016
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_FWHEELWIDGET_H
#define UI_FORM_FWHEELWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FwheelWidgetObject
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *fgroupbox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *Pos;
    QComboBox *fValue;

    void setupUi(QWidget *FwheelWidgetObject)
    {
        if (FwheelWidgetObject->objectName().isEmpty())
            FwheelWidgetObject->setObjectName(QString::fromUtf8("FwheelWidgetObject"));
        FwheelWidgetObject->resize(260, 53);
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
        FwheelWidgetObject->setPalette(palette);
        layoutWidget = new QWidget(FwheelWidgetObject);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 251, 51));
        verticalLayout_3 = new QVBoxLayout(layoutWidget);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        fgroupbox = new QGroupBox(layoutWidget);
        fgroupbox->setObjectName(QString::fromUtf8("fgroupbox"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fgroupbox->sizePolicy().hasHeightForWidth());
        fgroupbox->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        fgroupbox->setFont(font);
        fgroupbox->setAlignment(Qt::AlignCenter);
        gridLayout = new QGridLayout(fgroupbox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(4);
        gridLayout->setContentsMargins(9, 6, -1, 6);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, 0, 0);
        Pos = new QLabel(fgroupbox);
        Pos->setObjectName(QString::fromUtf8("Pos"));
        sizePolicy.setHeightForWidth(Pos->sizePolicy().hasHeightForWidth());
        Pos->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(Pos);

        fValue = new QComboBox(fgroupbox);
        fValue->setObjectName(QString::fromUtf8("fValue"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(fValue->sizePolicy().hasHeightForWidth());
        fValue->setSizePolicy(sizePolicy1);
        fValue->setMaximumSize(QSize(90, 16777215));
        fValue->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(fValue);


        gridLayout->addLayout(horizontalLayout, 0, 2, 1, 1);


        verticalLayout_3->addWidget(fgroupbox);


        retranslateUi(FwheelWidgetObject);

        QMetaObject::connectSlotsByName(FwheelWidgetObject);
    } // setupUi

    void retranslateUi(QWidget *FwheelWidgetObject)
    {
        FwheelWidgetObject->setWindowTitle(QApplication::translate("FwheelWidgetObject", "Form", 0, QApplication::UnicodeUTF8));
        fgroupbox->setTitle(QApplication::translate("FwheelWidgetObject", "fwheel_a", 0, QApplication::UnicodeUTF8));
        Pos->setText(QApplication::translate("FwheelWidgetObject", "Absorption Constant:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FwheelWidgetObject: public Ui_FwheelWidgetObject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_FWHEELWIDGET_H
