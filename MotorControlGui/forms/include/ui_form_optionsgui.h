/********************************************************************************
** Form generated from reading UI file 'form_optionsgui.ui'
**
** Created: Mon Jul 11 15:14:50 2016
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_OPTIONSGUI_H
#define UI_FORM_OPTIONSGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OptionsObject
{
public:
    QGroupBox *mainBox;
    QFrame *frame1;
    QGroupBox *motorBox;
    QFrame *optionsFrame;
    QPushButton *moveAbs;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLineEdit *posInput;
    QPushButton *setPos;
    QLineEdit *speedInput;
    QPushButton *setSpeed;
    QLineEdit *upperInput;
    QPushButton *setUpper;
    QLineEdit *lowerInput;
    QPushButton *setLower;
    QPushButton *home;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *motorName;
    QPushButton *done;

    void setupUi(QWidget *OptionsObject)
    {
        if (OptionsObject->objectName().isEmpty())
            OptionsObject->setObjectName(QString::fromUtf8("OptionsObject"));
        OptionsObject->resize(624, 574);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(OptionsObject->sizePolicy().hasHeightForWidth());
        OptionsObject->setSizePolicy(sizePolicy);
        mainBox = new QGroupBox(OptionsObject);
        mainBox->setObjectName(QString::fromUtf8("mainBox"));
        mainBox->setGeometry(QRect(30, 26, 561, 515));
        mainBox->setFocusPolicy(Qt::NoFocus);
        frame1 = new QFrame(mainBox);
        frame1->setObjectName(QString::fromUtf8("frame1"));
        frame1->setGeometry(QRect(127, 46, 312, 392));
        sizePolicy.setHeightForWidth(frame1->sizePolicy().hasHeightForWidth());
        frame1->setSizePolicy(sizePolicy);
        frame1->setFrameShape(QFrame::NoFrame);
        frame1->setFrameShadow(QFrame::Plain);
        motorBox = new QGroupBox(frame1);
        motorBox->setObjectName(QString::fromUtf8("motorBox"));
        motorBox->setGeometry(QRect(12, 42, 291, 337));
        optionsFrame = new QFrame(motorBox);
        optionsFrame->setObjectName(QString::fromUtf8("optionsFrame"));
        optionsFrame->setGeometry(QRect(16, 179, 261, 110));
        sizePolicy.setHeightForWidth(optionsFrame->sizePolicy().hasHeightForWidth());
        optionsFrame->setSizePolicy(sizePolicy);
        optionsFrame->setFrameShape(QFrame::NoFrame);
        optionsFrame->setFrameShadow(QFrame::Plain);
        moveAbs = new QPushButton(motorBox);
        moveAbs->setObjectName(QString::fromUtf8("moveAbs"));
        moveAbs->setGeometry(QRect(158, 298, 109, 25));
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(moveAbs->sizePolicy().hasHeightForWidth());
        moveAbs->setSizePolicy(sizePolicy1);
        moveAbs->setFocusPolicy(Qt::StrongFocus);
        layoutWidget = new QWidget(motorBox);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 19, 271, 151));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        posInput = new QLineEdit(layoutWidget);
        posInput->setObjectName(QString::fromUtf8("posInput"));
        sizePolicy1.setHeightForWidth(posInput->sizePolicy().hasHeightForWidth());
        posInput->setSizePolicy(sizePolicy1);
        posInput->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(posInput, 0, 0, 1, 1);

        setPos = new QPushButton(layoutWidget);
        setPos->setObjectName(QString::fromUtf8("setPos"));
        sizePolicy1.setHeightForWidth(setPos->sizePolicy().hasHeightForWidth());
        setPos->setSizePolicy(sizePolicy1);
        setPos->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(setPos, 0, 1, 1, 1);

        speedInput = new QLineEdit(layoutWidget);
        speedInput->setObjectName(QString::fromUtf8("speedInput"));
        sizePolicy1.setHeightForWidth(speedInput->sizePolicy().hasHeightForWidth());
        speedInput->setSizePolicy(sizePolicy1);
        speedInput->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(speedInput, 1, 0, 1, 1);

        setSpeed = new QPushButton(layoutWidget);
        setSpeed->setObjectName(QString::fromUtf8("setSpeed"));
        sizePolicy1.setHeightForWidth(setSpeed->sizePolicy().hasHeightForWidth());
        setSpeed->setSizePolicy(sizePolicy1);
        setSpeed->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(setSpeed, 1, 1, 1, 1);

        upperInput = new QLineEdit(layoutWidget);
        upperInput->setObjectName(QString::fromUtf8("upperInput"));
        sizePolicy1.setHeightForWidth(upperInput->sizePolicy().hasHeightForWidth());
        upperInput->setSizePolicy(sizePolicy1);
        upperInput->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(upperInput, 2, 0, 1, 1);

        setUpper = new QPushButton(layoutWidget);
        setUpper->setObjectName(QString::fromUtf8("setUpper"));
        sizePolicy1.setHeightForWidth(setUpper->sizePolicy().hasHeightForWidth());
        setUpper->setSizePolicy(sizePolicy1);
        setUpper->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(setUpper, 2, 1, 1, 1);

        lowerInput = new QLineEdit(layoutWidget);
        lowerInput->setObjectName(QString::fromUtf8("lowerInput"));
        sizePolicy1.setHeightForWidth(lowerInput->sizePolicy().hasHeightForWidth());
        lowerInput->setSizePolicy(sizePolicy1);
        lowerInput->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(lowerInput, 3, 0, 1, 1);

        setLower = new QPushButton(layoutWidget);
        setLower->setObjectName(QString::fromUtf8("setLower"));
        sizePolicy1.setHeightForWidth(setLower->sizePolicy().hasHeightForWidth());
        setLower->setSizePolicy(sizePolicy1);
        setLower->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(setLower, 3, 1, 1, 1);

        home = new QPushButton(motorBox);
        home->setObjectName(QString::fromUtf8("home"));
        home->setGeometry(QRect(25, 298, 109, 25));
        sizePolicy1.setHeightForWidth(home->sizePolicy().hasHeightForWidth());
        home->setSizePolicy(sizePolicy1);
        home->setFocusPolicy(Qt::NoFocus);
        layoutWidget1 = new QWidget(frame1);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(22, 13, 272, 23));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget1);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(label);

        motorName = new QComboBox(layoutWidget1);
        motorName->setObjectName(QString::fromUtf8("motorName"));
        sizePolicy1.setHeightForWidth(motorName->sizePolicy().hasHeightForWidth());
        motorName->setSizePolicy(sizePolicy1);
        motorName->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(motorName);

        done = new QPushButton(mainBox);
        done->setObjectName(QString::fromUtf8("done"));
        done->setGeometry(QRect(462, 450, 75, 25));
        done->setFocusPolicy(Qt::NoFocus);

        retranslateUi(OptionsObject);

        QMetaObject::connectSlotsByName(OptionsObject);
    } // setupUi

    void retranslateUi(QWidget *OptionsObject)
    {
        OptionsObject->setWindowTitle(QApplication::translate("OptionsObject", "Form", 0, QApplication::UnicodeUTF8));
        mainBox->setTitle(QApplication::translate("OptionsObject", "Options", 0, QApplication::UnicodeUTF8));
        motorBox->setTitle(QApplication::translate("OptionsObject", "GroupBox", 0, QApplication::UnicodeUTF8));
        moveAbs->setText(QApplication::translate("OptionsObject", "Move Absolute", 0, QApplication::UnicodeUTF8));
        setPos->setText(QApplication::translate("OptionsObject", "Set Position", 0, QApplication::UnicodeUTF8));
        setSpeed->setText(QApplication::translate("OptionsObject", "Set Speed", 0, QApplication::UnicodeUTF8));
        setUpper->setText(QApplication::translate("OptionsObject", "Set Upper Limit", 0, QApplication::UnicodeUTF8));
        setLower->setText(QApplication::translate("OptionsObject", "Set Lower Limit", 0, QApplication::UnicodeUTF8));
        home->setText(QApplication::translate("OptionsObject", "Home", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("OptionsObject", "Name of Motor:", 0, QApplication::UnicodeUTF8));
        done->setText(QApplication::translate("OptionsObject", "OK", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class OptionsObject: public Ui_OptionsObject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_OPTIONSGUI_H
