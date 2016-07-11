/********************************************************************************
** Form generated from reading UI file 'form_laserboxgui.ui'
**
** Created: Mon Jul 11 16:48:39 2016
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_LASERBOXGUI_H
#define UI_FORM_LASERBOXGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LaserBoxGuiObject
{
public:
    QGroupBox *the_box;
    QFrame *frame1;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *labelRef;
    QComboBox *refValue;
    QPushButton *options;
    QGroupBox *the_box2;
    QFrame *frame2;
    QPushButton *updateAll;

    void setupUi(QWidget *LaserBoxGuiObject)
    {
        if (LaserBoxGuiObject->objectName().isEmpty())
            LaserBoxGuiObject->setObjectName(QString::fromUtf8("LaserBoxGuiObject"));
        LaserBoxGuiObject->resize(319, 798);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LaserBoxGuiObject->sizePolicy().hasHeightForWidth());
        LaserBoxGuiObject->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        LaserBoxGuiObject->setFont(font);
        the_box = new QGroupBox(LaserBoxGuiObject);
        the_box->setObjectName(QString::fromUtf8("the_box"));
        the_box->setEnabled(true);
        the_box->setGeometry(QRect(10, 10, 301, 461));
        sizePolicy.setHeightForWidth(the_box->sizePolicy().hasHeightForWidth());
        the_box->setSizePolicy(sizePolicy);
        the_box->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        frame1 = new QFrame(the_box);
        frame1->setObjectName(QString::fromUtf8("frame1"));
        frame1->setGeometry(QRect(10, 57, 281, 361));
        sizePolicy.setHeightForWidth(frame1->sizePolicy().hasHeightForWidth());
        frame1->setSizePolicy(sizePolicy);
        frame1->setFrameShape(QFrame::NoFrame);
        frame1->setFrameShadow(QFrame::Plain);
        layoutWidget = new QWidget(the_box);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 420, 261, 28));
        horizontalLayout = new QHBoxLayout(layoutWidget);
#ifndef Q_OS_MAC
        horizontalLayout->setSpacing(-1);
#endif
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        labelRef = new QLabel(layoutWidget);
        labelRef->setObjectName(QString::fromUtf8("labelRef"));
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelRef->sizePolicy().hasHeightForWidth());
        labelRef->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(labelRef);

        refValue = new QComboBox(layoutWidget);
        refValue->setObjectName(QString::fromUtf8("refValue"));
        sizePolicy1.setHeightForWidth(refValue->sizePolicy().hasHeightForWidth());
        refValue->setSizePolicy(sizePolicy1);
        refValue->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(refValue);

        options = new QPushButton(the_box);
        options->setObjectName(QString::fromUtf8("options"));
        options->setGeometry(QRect(210, 20, 71, 31));
        options->setFocusPolicy(Qt::NoFocus);
        options->setAutoRepeat(false);
        options->setAutoExclusive(false);
        options->setAutoDefault(false);
        options->setDefault(true);
        the_box2 = new QGroupBox(LaserBoxGuiObject);
        the_box2->setObjectName(QString::fromUtf8("the_box2"));
        the_box2->setEnabled(true);
        the_box2->setGeometry(QRect(10, 490, 301, 260));
        sizePolicy.setHeightForWidth(the_box2->sizePolicy().hasHeightForWidth());
        the_box2->setSizePolicy(sizePolicy);
        the_box2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        frame2 = new QFrame(the_box2);
        frame2->setObjectName(QString::fromUtf8("frame2"));
        frame2->setGeometry(QRect(10, 27, 281, 221));
        sizePolicy.setHeightForWidth(frame2->sizePolicy().hasHeightForWidth());
        frame2->setSizePolicy(sizePolicy);
        frame2->setFrameShape(QFrame::NoFrame);
        frame2->setFrameShadow(QFrame::Plain);
        updateAll = new QPushButton(LaserBoxGuiObject);
        updateAll->setObjectName(QString::fromUtf8("updateAll"));
        updateAll->setGeometry(QRect(220, 760, 71, 31));
        updateAll->setFocusPolicy(Qt::NoFocus);
        updateAll->setAutoRepeat(false);
        updateAll->setAutoExclusive(false);
        updateAll->setAutoDefault(false);
        updateAll->setDefault(true);

        retranslateUi(LaserBoxGuiObject);

        QMetaObject::connectSlotsByName(LaserBoxGuiObject);
    } // setupUi

    void retranslateUi(QWidget *LaserBoxGuiObject)
    {
        LaserBoxGuiObject->setWindowTitle(QApplication::translate("LaserBoxGuiObject", "Laserbox GUI", 0, QApplication::UnicodeUTF8));
        the_box->setTitle(QApplication::translate("LaserBoxGuiObject", "Corvus Motors", 0, QApplication::UnicodeUTF8));
        labelRef->setText(QApplication::translate("LaserBoxGuiObject", "Reference Point:", 0, QApplication::UnicodeUTF8));
        options->setText(QApplication::translate("LaserBoxGuiObject", "Options", 0, QApplication::UnicodeUTF8));
        the_box2->setTitle(QApplication::translate("LaserBoxGuiObject", "Filter Wheel Motors", 0, QApplication::UnicodeUTF8));
        updateAll->setText(QApplication::translate("LaserBoxGuiObject", "Update", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LaserBoxGuiObject: public Ui_LaserBoxGuiObject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_LASERBOXGUI_H
