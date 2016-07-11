/********************************************************************************
** Form generated from reading UI file 'form_xrayboxgui.ui'
**
** Created: Fri Apr 10 13:42:44 2015
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_XRAYBOXGUI_H
#define UI_FORM_XRAYBOXGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_XRayBoxGuiObject
{
public:
    QGroupBox *the_box;
    QFrame *frame1;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QComboBox *fluorLabel;
    QComboBox *fluorName;
    QLineEdit *energyDisplay;
    QFrame *frame3;
    QTabWidget *tabWidget;
    QWidget *individual;
    QFrame *frame2;
    QWidget *common;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout;
    QGroupBox *groupbox;
    QGridLayout *gridLayout_2;
    QLineEdit *centerAbsDisplay;
    QPushButton *centerMinus;
    QLineEdit *centerRelDisplay;
    QPushButton *centerPlus;
    QLabel *Pos;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_4;
    QLineEdit *centerDisplay;
    QPushButton *exactCenter;
    QGroupBox *groupbox_2;
    QGridLayout *gridLayout_3;
    QLineEdit *widthAbsDisplay;
    QPushButton *widthMinus;
    QLineEdit *widthRelDisplay;
    QPushButton *widthPlus;
    QLabel *Pos_2;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_7;
    QLineEdit *widthDisplay;
    QPushButton *zeroWidth;
    QFrame *frame4;
    QPushButton *updateAll;
    QPushButton *options;
    QGraphicsView *topView;
    QGraphicsView *frontView;
    QLabel *label;
    QLabel *label_2;
    QGroupBox *xrayGroup;
    QFrame *frame_2;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout_4;
    QLabel *label_8;
    QLineEdit *actualVDisp;
    QLabel *label_6;
    QLineEdit *actualCDisp;
    QGroupBox *warmupBox;
    QLineEdit *wtDisp;
    QLabel *label_5;
    QGraphicsView *graph2;
    QGraphicsView *graph1;
    QLabel *label_14;
    QLabel *label_15;
    QFrame *onFrame;
    QGroupBox *shutterBox;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *shutter1;
    QCheckBox *shutter2;
    QCheckBox *shutter3;
    QCheckBox *shutter4;
    QPushButton *warning;
    QPushButton *highVoltage;
    QLabel *warningText;
    QFrame *frame_3;
    QWidget *layoutWidget3;
    QGridLayout *gridLayout_5;
    QLabel *label_3;
    QLineEdit *setVDisp;
    QPushButton *setVoltage;
    QLineEdit *setV;
    QLabel *label_4;
    QLineEdit *setCDisp;
    QPushButton *setCurrent;
    QLineEdit *setC;
    QPushButton *getErrorCode;
    QWidget *layoutWidget4;
    QVBoxLayout *verticalLayout;
    QLabel *label_10;
    QLabel *unit1;
    QLabel *label_12;
    QLabel *unit2;
    QWidget *layoutWidget5;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_11;
    QLabel *label_13;

    void setupUi(QWidget *XRayBoxGuiObject)
    {
        if (XRayBoxGuiObject->objectName().isEmpty())
            XRayBoxGuiObject->setObjectName(QString::fromUtf8("XRayBoxGuiObject"));
        XRayBoxGuiObject->resize(1247, 949);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(XRayBoxGuiObject->sizePolicy().hasHeightForWidth());
        XRayBoxGuiObject->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        XRayBoxGuiObject->setFont(font);
        the_box = new QGroupBox(XRayBoxGuiObject);
        the_box->setObjectName(QString::fromUtf8("the_box"));
        the_box->setEnabled(true);
        the_box->setGeometry(QRect(930, 3, 311, 892));
        sizePolicy.setHeightForWidth(the_box->sizePolicy().hasHeightForWidth());
        the_box->setSizePolicy(sizePolicy);
        the_box->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        frame1 = new QFrame(the_box);
        frame1->setObjectName(QString::fromUtf8("frame1"));
        frame1->setGeometry(QRect(20, 40, 281, 231));
        sizePolicy.setHeightForWidth(frame1->sizePolicy().hasHeightForWidth());
        frame1->setSizePolicy(sizePolicy);
        frame1->setFrameShape(QFrame::NoFrame);
        frame1->setFrameShadow(QFrame::Plain);
        layoutWidget = new QWidget(the_box);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(23, 20, 279, 24));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        fluorLabel = new QComboBox(layoutWidget);
        fluorLabel->setObjectName(QString::fromUtf8("fluorLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(fluorLabel->sizePolicy().hasHeightForWidth());
        fluorLabel->setSizePolicy(sizePolicy1);
        fluorLabel->setMinimumSize(QSize(20, 21));
        fluorLabel->setFocusPolicy(Qt::NoFocus);
        fluorLabel->setEditable(false);
        fluorLabel->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        horizontalLayout->addWidget(fluorLabel);

        fluorName = new QComboBox(layoutWidget);
        fluorName->setObjectName(QString::fromUtf8("fluorName"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(fluorName->sizePolicy().hasHeightForWidth());
        fluorName->setSizePolicy(sizePolicy2);
        fluorName->setMinimumSize(QSize(30, 21));
        fluorName->setFocusPolicy(Qt::NoFocus);
        fluorName->setEditable(false);
        fluorName->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        horizontalLayout->addWidget(fluorName);

        energyDisplay = new QLineEdit(layoutWidget);
        energyDisplay->setObjectName(QString::fromUtf8("energyDisplay"));
        QSizePolicy sizePolicy3(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(energyDisplay->sizePolicy().hasHeightForWidth());
        energyDisplay->setSizePolicy(sizePolicy3);
        energyDisplay->setMinimumSize(QSize(0, 0));
        energyDisplay->setSizeIncrement(QSize(0, 0));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(27, 13, 139, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        QBrush brush2(QColor(170, 170, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        energyDisplay->setPalette(palette);
        energyDisplay->setMaxLength(32767);
        energyDisplay->setFrame(false);
        energyDisplay->setAlignment(Qt::AlignCenter);
        energyDisplay->setReadOnly(true);

        horizontalLayout->addWidget(energyDisplay);

        frame3 = new QFrame(the_box);
        frame3->setObjectName(QString::fromUtf8("frame3"));
        frame3->setGeometry(QRect(20, 540, 281, 236));
        sizePolicy.setHeightForWidth(frame3->sizePolicy().hasHeightForWidth());
        frame3->setSizePolicy(sizePolicy);
        frame3->setFrameShape(QFrame::NoFrame);
        frame3->setFrameShadow(QFrame::Plain);
        tabWidget = new QTabWidget(the_box);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 280, 292, 254));
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setLayoutDirection(Qt::LeftToRight);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideLeft);
        tabWidget->setUsesScrollButtons(true);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(false);
        individual = new QWidget();
        individual->setObjectName(QString::fromUtf8("individual"));
        frame2 = new QFrame(individual);
        frame2->setObjectName(QString::fromUtf8("frame2"));
        frame2->setGeometry(QRect(4, 1, 288, 227));
        sizePolicy.setHeightForWidth(frame2->sizePolicy().hasHeightForWidth());
        frame2->setSizePolicy(sizePolicy);
        frame2->setFrameShape(QFrame::NoFrame);
        frame2->setFrameShadow(QFrame::Plain);
        tabWidget->addTab(individual, QString());
        common = new QWidget();
        common->setObjectName(QString::fromUtf8("common"));
        layoutWidget1 = new QWidget(common);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(12, 11, 262, 208));
        gridLayout = new QGridLayout(layoutWidget1);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        groupbox = new QGroupBox(layoutWidget1);
        groupbox->setObjectName(QString::fromUtf8("groupbox"));
        sizePolicy.setHeightForWidth(groupbox->sizePolicy().hasHeightForWidth());
        groupbox->setSizePolicy(sizePolicy);
        groupbox->setFont(font);
        groupbox->setAlignment(Qt::AlignCenter);
        gridLayout_2 = new QGridLayout(groupbox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(4);
        gridLayout_2->setContentsMargins(9, 6, -1, 6);
        centerAbsDisplay = new QLineEdit(groupbox);
        centerAbsDisplay->setObjectName(QString::fromUtf8("centerAbsDisplay"));
        sizePolicy2.setHeightForWidth(centerAbsDisplay->sizePolicy().hasHeightForWidth());
        centerAbsDisplay->setSizePolicy(sizePolicy2);
        centerAbsDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(centerAbsDisplay, 1, 3, 1, 1);

        centerMinus = new QPushButton(groupbox);
        centerMinus->setObjectName(QString::fromUtf8("centerMinus"));
        sizePolicy.setHeightForWidth(centerMinus->sizePolicy().hasHeightForWidth());
        centerMinus->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(centerMinus, 2, 2, 1, 1);

        centerRelDisplay = new QLineEdit(groupbox);
        centerRelDisplay->setObjectName(QString::fromUtf8("centerRelDisplay"));
        sizePolicy2.setHeightForWidth(centerRelDisplay->sizePolicy().hasHeightForWidth());
        centerRelDisplay->setSizePolicy(sizePolicy2);
        centerRelDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(centerRelDisplay, 2, 3, 1, 1);

        centerPlus = new QPushButton(groupbox);
        centerPlus->setObjectName(QString::fromUtf8("centerPlus"));
        sizePolicy.setHeightForWidth(centerPlus->sizePolicy().hasHeightForWidth());
        centerPlus->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(centerPlus, 2, 4, 1, 1);

        Pos = new QLabel(groupbox);
        Pos->setObjectName(QString::fromUtf8("Pos"));
        sizePolicy.setHeightForWidth(Pos->sizePolicy().hasHeightForWidth());
        Pos->setSizePolicy(sizePolicy);
        Pos->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(Pos, 0, 2, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 1, 4, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 1, 2, 1, 1);

        centerDisplay = new QLineEdit(groupbox);
        centerDisplay->setObjectName(QString::fromUtf8("centerDisplay"));
        sizePolicy.setHeightForWidth(centerDisplay->sizePolicy().hasHeightForWidth());
        centerDisplay->setSizePolicy(sizePolicy);
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        centerDisplay->setPalette(palette1);
        centerDisplay->setFrame(false);
        centerDisplay->setAlignment(Qt::AlignCenter);
        centerDisplay->setReadOnly(true);

        gridLayout_2->addWidget(centerDisplay, 0, 3, 1, 1);

        exactCenter = new QPushButton(groupbox);
        exactCenter->setObjectName(QString::fromUtf8("exactCenter"));
        sizePolicy3.setHeightForWidth(exactCenter->sizePolicy().hasHeightForWidth());
        exactCenter->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(exactCenter, 0, 4, 1, 1);


        gridLayout->addWidget(groupbox, 0, 0, 1, 1);

        groupbox_2 = new QGroupBox(layoutWidget1);
        groupbox_2->setObjectName(QString::fromUtf8("groupbox_2"));
        sizePolicy.setHeightForWidth(groupbox_2->sizePolicy().hasHeightForWidth());
        groupbox_2->setSizePolicy(sizePolicy);
        groupbox_2->setFont(font);
        groupbox_2->setAlignment(Qt::AlignCenter);
        gridLayout_3 = new QGridLayout(groupbox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setVerticalSpacing(4);
        gridLayout_3->setContentsMargins(9, 6, -1, 6);
        widthAbsDisplay = new QLineEdit(groupbox_2);
        widthAbsDisplay->setObjectName(QString::fromUtf8("widthAbsDisplay"));
        sizePolicy2.setHeightForWidth(widthAbsDisplay->sizePolicy().hasHeightForWidth());
        widthAbsDisplay->setSizePolicy(sizePolicy2);
        widthAbsDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(widthAbsDisplay, 1, 3, 1, 1);

        widthMinus = new QPushButton(groupbox_2);
        widthMinus->setObjectName(QString::fromUtf8("widthMinus"));
        sizePolicy.setHeightForWidth(widthMinus->sizePolicy().hasHeightForWidth());
        widthMinus->setSizePolicy(sizePolicy);

        gridLayout_3->addWidget(widthMinus, 2, 2, 1, 1);

        widthRelDisplay = new QLineEdit(groupbox_2);
        widthRelDisplay->setObjectName(QString::fromUtf8("widthRelDisplay"));
        sizePolicy2.setHeightForWidth(widthRelDisplay->sizePolicy().hasHeightForWidth());
        widthRelDisplay->setSizePolicy(sizePolicy2);
        widthRelDisplay->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(widthRelDisplay, 2, 3, 1, 1);

        widthPlus = new QPushButton(groupbox_2);
        widthPlus->setObjectName(QString::fromUtf8("widthPlus"));
        sizePolicy.setHeightForWidth(widthPlus->sizePolicy().hasHeightForWidth());
        widthPlus->setSizePolicy(sizePolicy);

        gridLayout_3->addWidget(widthPlus, 2, 4, 1, 1);

        Pos_2 = new QLabel(groupbox_2);
        Pos_2->setObjectName(QString::fromUtf8("Pos_2"));
        sizePolicy.setHeightForWidth(Pos_2->sizePolicy().hasHeightForWidth());
        Pos_2->setSizePolicy(sizePolicy);
        Pos_2->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(Pos_2, 0, 2, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_6, 1, 4, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(22, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_7, 1, 2, 1, 1);

        widthDisplay = new QLineEdit(groupbox_2);
        widthDisplay->setObjectName(QString::fromUtf8("widthDisplay"));
        sizePolicy.setHeightForWidth(widthDisplay->sizePolicy().hasHeightForWidth());
        widthDisplay->setSizePolicy(sizePolicy);
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        widthDisplay->setPalette(palette2);
        widthDisplay->setFrame(false);
        widthDisplay->setAlignment(Qt::AlignCenter);
        widthDisplay->setReadOnly(true);

        gridLayout_3->addWidget(widthDisplay, 0, 3, 1, 1);

        zeroWidth = new QPushButton(groupbox_2);
        zeroWidth->setObjectName(QString::fromUtf8("zeroWidth"));
        sizePolicy3.setHeightForWidth(zeroWidth->sizePolicy().hasHeightForWidth());
        zeroWidth->setSizePolicy(sizePolicy3);

        gridLayout_3->addWidget(zeroWidth, 0, 4, 1, 1);


        gridLayout->addWidget(groupbox_2, 1, 0, 1, 1);

        tabWidget->addTab(common, QString());
        frame4 = new QFrame(the_box);
        frame4->setObjectName(QString::fromUtf8("frame4"));
        frame4->setGeometry(QRect(20, 763, 281, 123));
        sizePolicy.setHeightForWidth(frame4->sizePolicy().hasHeightForWidth());
        frame4->setSizePolicy(sizePolicy);
        frame4->setFrameShape(QFrame::NoFrame);
        frame4->setFrameShadow(QFrame::Plain);
        updateAll = new QPushButton(XRayBoxGuiObject);
        updateAll->setObjectName(QString::fromUtf8("updateAll"));
        updateAll->setGeometry(QRect(1146, 905, 71, 31));
        updateAll->setFocusPolicy(Qt::NoFocus);
        updateAll->setAutoRepeat(false);
        updateAll->setAutoExclusive(false);
        updateAll->setAutoDefault(false);
        updateAll->setDefault(true);
        options = new QPushButton(XRayBoxGuiObject);
        options->setObjectName(QString::fromUtf8("options"));
        options->setGeometry(QRect(954, 904, 71, 31));
        options->setFocusPolicy(Qt::NoFocus);
        options->setAutoRepeat(false);
        options->setAutoExclusive(false);
        options->setAutoDefault(false);
        options->setDefault(true);
        topView = new QGraphicsView(XRayBoxGuiObject);
        topView->setObjectName(QString::fromUtf8("topView"));
        topView->setGeometry(QRect(10, 20, 907, 427));
        sizePolicy.setHeightForWidth(topView->sizePolicy().hasHeightForWidth());
        topView->setSizePolicy(sizePolicy);
        topView->setFocusPolicy(Qt::NoFocus);
        topView->setFrameShape(QFrame::StyledPanel);
        topView->setFrameShadow(QFrame::Plain);
        QBrush brush3(QColor(255, 255, 255, 255));
        brush3.setStyle(Qt::NoBrush);
        topView->setBackgroundBrush(brush3);
        QBrush brush4(QColor(255, 255, 255, 255));
        brush4.setStyle(Qt::NoBrush);
        topView->setForegroundBrush(brush4);
        frontView = new QGraphicsView(XRayBoxGuiObject);
        frontView->setObjectName(QString::fromUtf8("frontView"));
        frontView->setGeometry(QRect(10, 470, 907, 215));
        frontView->setFocusPolicy(Qt::NoFocus);
        frontView->setFrameShadow(QFrame::Plain);
        QBrush brush5(QColor(255, 255, 255, 255));
        brush5.setStyle(Qt::NoBrush);
        frontView->setBackgroundBrush(brush5);
        label = new QLabel(XRayBoxGuiObject);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(12, 2, 94, 18));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label_2 = new QLabel(XRayBoxGuiObject);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(11, 453, 136, 16));
        label_2->setFont(font1);
        xrayGroup = new QGroupBox(XRayBoxGuiObject);
        xrayGroup->setObjectName(QString::fromUtf8("xrayGroup"));
        xrayGroup->setGeometry(QRect(10, 693, 906, 248));
        xrayGroup->setFocusPolicy(Qt::NoFocus);
        xrayGroup->setCheckable(true);
        xrayGroup->setChecked(false);
        frame_2 = new QFrame(xrayGroup);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 145, 151, 59));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy4);
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Plain);
        frame_2->setLineWidth(0);
        layoutWidget2 = new QWidget(frame_2);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(1, 4, 149, 54));
        gridLayout_4 = new QGridLayout(layoutWidget2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(layoutWidget2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        sizePolicy4.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy4);

        gridLayout_4->addWidget(label_8, 0, 0, 1, 1);

        actualVDisp = new QLineEdit(layoutWidget2);
        actualVDisp->setObjectName(QString::fromUtf8("actualVDisp"));
        sizePolicy4.setHeightForWidth(actualVDisp->sizePolicy().hasHeightForWidth());
        actualVDisp->setSizePolicy(sizePolicy4);
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette3.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        actualVDisp->setPalette(palette3);
        actualVDisp->setFrame(false);
        actualVDisp->setAlignment(Qt::AlignCenter);
        actualVDisp->setReadOnly(true);

        gridLayout_4->addWidget(actualVDisp, 0, 1, 1, 1);

        label_6 = new QLabel(layoutWidget2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy4.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy4);

        gridLayout_4->addWidget(label_6, 1, 0, 1, 1);

        actualCDisp = new QLineEdit(layoutWidget2);
        actualCDisp->setObjectName(QString::fromUtf8("actualCDisp"));
        sizePolicy4.setHeightForWidth(actualCDisp->sizePolicy().hasHeightForWidth());
        actualCDisp->setSizePolicy(sizePolicy4);
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette4.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        actualCDisp->setPalette(palette4);
        actualCDisp->setFrame(false);
        actualCDisp->setAlignment(Qt::AlignCenter);
        actualCDisp->setReadOnly(true);

        gridLayout_4->addWidget(actualCDisp, 1, 1, 1, 1);

        warmupBox = new QGroupBox(xrayGroup);
        warmupBox->setObjectName(QString::fromUtf8("warmupBox"));
        warmupBox->setGeometry(QRect(190, 20, 461, 218));
        sizePolicy4.setHeightForWidth(warmupBox->sizePolicy().hasHeightForWidth());
        warmupBox->setSizePolicy(sizePolicy4);
        warmupBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        wtDisp = new QLineEdit(warmupBox);
        wtDisp->setObjectName(QString::fromUtf8("wtDisp"));
        wtDisp->setGeometry(QRect(347, 16, 105, 22));
        sizePolicy4.setHeightForWidth(wtDisp->sizePolicy().hasHeightForWidth());
        wtDisp->setSizePolicy(sizePolicy4);
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette5.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette5.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        wtDisp->setPalette(palette5);
        wtDisp->setFrame(false);
        wtDisp->setAlignment(Qt::AlignCenter);
        wtDisp->setReadOnly(true);
        label_5 = new QLabel(warmupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(242, 17, 100, 22));
        sizePolicy4.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy4);
        graph2 = new QGraphicsView(warmupBox);
        graph2->setObjectName(QString::fromUtf8("graph2"));
        graph2->setGeometry(QRect(240, 46, 211, 163));
        graph2->setFocusPolicy(Qt::NoFocus);
        graph2->setFrameShadow(QFrame::Plain);
        graph1 = new QGraphicsView(warmupBox);
        graph1->setObjectName(QString::fromUtf8("graph1"));
        graph1->setGeometry(QRect(8, 46, 213, 163));
        graph1->setFocusPolicy(Qt::NoFocus);
        graph1->setFrameShadow(QFrame::Plain);
        label_14 = new QLabel(warmupBox);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(40, 47, 160, 16));
        QFont font2;
        font2.setPointSize(8);
        label_14->setFont(font2);
        label_15 = new QLabel(warmupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(264, 48, 166, 16));
        label_15->setFont(font2);
        onFrame = new QFrame(xrayGroup);
        onFrame->setObjectName(QString::fromUtf8("onFrame"));
        onFrame->setGeometry(QRect(660, 27, 234, 212));
        onFrame->setFrameShape(QFrame::StyledPanel);
        onFrame->setFrameShadow(QFrame::Raised);
        shutterBox = new QGroupBox(onFrame);
        shutterBox->setObjectName(QString::fromUtf8("shutterBox"));
        shutterBox->setGeometry(QRect(10, 14, 101, 187));
        shutterBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        shutterBox->setCheckable(false);
        layoutWidget_2 = new QWidget(shutterBox);
        layoutWidget_2->setObjectName(QString::fromUtf8("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(12, 16, 81, 162));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        shutter1 = new QCheckBox(layoutWidget_2);
        shutter1->setObjectName(QString::fromUtf8("shutter1"));
        shutter1->setFocusPolicy(Qt::NoFocus);

        verticalLayout_2->addWidget(shutter1);

        shutter2 = new QCheckBox(layoutWidget_2);
        shutter2->setObjectName(QString::fromUtf8("shutter2"));
        shutter2->setEnabled(false);
        shutter2->setFocusPolicy(Qt::NoFocus);

        verticalLayout_2->addWidget(shutter2);

        shutter3 = new QCheckBox(layoutWidget_2);
        shutter3->setObjectName(QString::fromUtf8("shutter3"));
        shutter3->setFocusPolicy(Qt::NoFocus);

        verticalLayout_2->addWidget(shutter3);

        shutter4 = new QCheckBox(layoutWidget_2);
        shutter4->setObjectName(QString::fromUtf8("shutter4"));
        shutter4->setEnabled(false);
        shutter4->setFocusPolicy(Qt::NoFocus);
        shutter4->setTristate(false);

        verticalLayout_2->addWidget(shutter4);

        warning = new QPushButton(onFrame);
        warning->setObjectName(QString::fromUtf8("warning"));
        warning->setGeometry(QRect(129, 20, 81, 71));
        warning->setFocusPolicy(Qt::NoFocus);
        warning->setCheckable(false);
        highVoltage = new QPushButton(onFrame);
        highVoltage->setObjectName(QString::fromUtf8("highVoltage"));
        highVoltage->setGeometry(QRect(129, 122, 81, 71));
        QFont font3;
        font3.setStyleStrategy(QFont::PreferDefault);
        highVoltage->setFont(font3);
        highVoltage->setFocusPolicy(Qt::NoFocus);
        highVoltage->setAutoFillBackground(true);
        highVoltage->setCheckable(true);
        highVoltage->setChecked(false);
        highVoltage->setFlat(false);
        warningText = new QLabel(onFrame);
        warningText->setObjectName(QString::fromUtf8("warningText"));
        warningText->setGeometry(QRect(140, 92, 65, 16));
        warningText->setFont(font1);
        warningText->setAlignment(Qt::AlignCenter);
        frame_3 = new QFrame(xrayGroup);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(18, 23, 143, 124));
        sizePolicy4.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy4);
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Plain);
        frame_3->setLineWidth(0);
        layoutWidget3 = new QWidget(frame_3);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(1, 0, 142, 123));
        gridLayout_5 = new QGridLayout(layoutWidget3);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy4.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy4);

        gridLayout_5->addWidget(label_3, 0, 0, 1, 1);

        setVDisp = new QLineEdit(layoutWidget3);
        setVDisp->setObjectName(QString::fromUtf8("setVDisp"));
        sizePolicy4.setHeightForWidth(setVDisp->sizePolicy().hasHeightForWidth());
        setVDisp->setSizePolicy(sizePolicy4);
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette6.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette6.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        setVDisp->setPalette(palette6);
        setVDisp->setFrame(false);
        setVDisp->setAlignment(Qt::AlignCenter);
        setVDisp->setReadOnly(true);

        gridLayout_5->addWidget(setVDisp, 0, 1, 1, 1);

        setVoltage = new QPushButton(layoutWidget3);
        setVoltage->setObjectName(QString::fromUtf8("setVoltage"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Ignored);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(setVoltage->sizePolicy().hasHeightForWidth());
        setVoltage->setSizePolicy(sizePolicy5);

        gridLayout_5->addWidget(setVoltage, 1, 0, 1, 1);

        setV = new QLineEdit(layoutWidget3);
        setV->setObjectName(QString::fromUtf8("setV"));
        sizePolicy4.setHeightForWidth(setV->sizePolicy().hasHeightForWidth());
        setV->setSizePolicy(sizePolicy4);
        setV->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(setV, 1, 1, 1, 1);

        label_4 = new QLabel(layoutWidget3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy4.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy4);

        gridLayout_5->addWidget(label_4, 2, 0, 1, 1);

        setCDisp = new QLineEdit(layoutWidget3);
        setCDisp->setObjectName(QString::fromUtf8("setCDisp"));
        sizePolicy4.setHeightForWidth(setCDisp->sizePolicy().hasHeightForWidth());
        setCDisp->setSizePolicy(sizePolicy4);
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette7.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette7.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette7.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette7.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        setCDisp->setPalette(palette7);
        setCDisp->setFrame(false);
        setCDisp->setAlignment(Qt::AlignCenter);
        setCDisp->setReadOnly(true);

        gridLayout_5->addWidget(setCDisp, 2, 1, 1, 1);

        setCurrent = new QPushButton(layoutWidget3);
        setCurrent->setObjectName(QString::fromUtf8("setCurrent"));
        sizePolicy5.setHeightForWidth(setCurrent->sizePolicy().hasHeightForWidth());
        setCurrent->setSizePolicy(sizePolicy5);

        gridLayout_5->addWidget(setCurrent, 3, 0, 1, 1);

        setC = new QLineEdit(layoutWidget3);
        setC->setObjectName(QString::fromUtf8("setC"));
        sizePolicy4.setHeightForWidth(setC->sizePolicy().hasHeightForWidth());
        setC->setSizePolicy(sizePolicy4);
        setC->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(setC, 3, 1, 1, 1);

        getErrorCode = new QPushButton(xrayGroup);
        getErrorCode->setObjectName(QString::fromUtf8("getErrorCode"));
        getErrorCode->setGeometry(QRect(10, 211, 168, 27));
        getErrorCode->setFocusPolicy(Qt::NoFocus);
        layoutWidget4 = new QWidget(xrayGroup);
        layoutWidget4->setObjectName(QString::fromUtf8("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(162, 24, 22, 122));
        verticalLayout = new QVBoxLayout(layoutWidget4);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_10 = new QLabel(layoutWidget4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        verticalLayout->addWidget(label_10);

        unit1 = new QLabel(layoutWidget4);
        unit1->setObjectName(QString::fromUtf8("unit1"));

        verticalLayout->addWidget(unit1);

        label_12 = new QLabel(layoutWidget4);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout->addWidget(label_12);

        unit2 = new QLabel(layoutWidget4);
        unit2->setObjectName(QString::fromUtf8("unit2"));

        verticalLayout->addWidget(unit2);

        layoutWidget5 = new QWidget(xrayGroup);
        layoutWidget5->setObjectName(QString::fromUtf8("layoutWidget5"));
        layoutWidget5->setGeometry(QRect(162, 149, 22, 54));
        verticalLayout_3 = new QVBoxLayout(layoutWidget5);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget5);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        verticalLayout_3->addWidget(label_11);

        label_13 = new QLabel(layoutWidget5);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        verticalLayout_3->addWidget(label_13);


        retranslateUi(XRayBoxGuiObject);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(XRayBoxGuiObject);
    } // setupUi

    void retranslateUi(QWidget *XRayBoxGuiObject)
    {
        XRayBoxGuiObject->setWindowTitle(QApplication::translate("XRayBoxGuiObject", "xrayboxgui one", 0, QApplication::UnicodeUTF8));
        the_box->setTitle(QApplication::translate("XRayBoxGuiObject", "X-Ray Motors", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(individual), QApplication::translate("XRayBoxGuiObject", "Individual Slits", 0, QApplication::UnicodeUTF8));
        groupbox->setTitle(QApplication::translate("XRayBoxGuiObject", "Center", 0, QApplication::UnicodeUTF8));
        centerMinus->setText(QApplication::translate("XRayBoxGuiObject", "-", 0, QApplication::UnicodeUTF8));
        centerPlus->setText(QApplication::translate("XRayBoxGuiObject", "+", 0, QApplication::UnicodeUTF8));
        Pos->setText(QApplication::translate("XRayBoxGuiObject", "Center:", 0, QApplication::UnicodeUTF8));
        exactCenter->setText(QApplication::translate("XRayBoxGuiObject", "Exact", 0, QApplication::UnicodeUTF8));
        groupbox_2->setTitle(QApplication::translate("XRayBoxGuiObject", "SlitWidth", 0, QApplication::UnicodeUTF8));
        widthMinus->setText(QApplication::translate("XRayBoxGuiObject", "-", 0, QApplication::UnicodeUTF8));
        widthPlus->setText(QApplication::translate("XRayBoxGuiObject", "+", 0, QApplication::UnicodeUTF8));
        Pos_2->setText(QApplication::translate("XRayBoxGuiObject", "Width:", 0, QApplication::UnicodeUTF8));
        zeroWidth->setText(QApplication::translate("XRayBoxGuiObject", "Zero", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(common), QApplication::translate("XRayBoxGuiObject", "Common Movements", 0, QApplication::UnicodeUTF8));
        updateAll->setText(QApplication::translate("XRayBoxGuiObject", "Update", 0, QApplication::UnicodeUTF8));
        options->setText(QApplication::translate("XRayBoxGuiObject", "Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("XRayBoxGuiObject", "TOP VIEW :", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("XRayBoxGuiObject", "FRONT VIEW :", 0, QApplication::UnicodeUTF8));
        xrayGroup->setTitle(QApplication::translate("XRayBoxGuiObject", "X-Ray Tube", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("XRayBoxGuiObject", "Actual Voltage:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("XRayBoxGuiObject", "Actual Current:", 0, QApplication::UnicodeUTF8));
        warmupBox->setTitle(QApplication::translate("XRayBoxGuiObject", "Warm-up", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("XRayBoxGuiObject", "Time Remaining:", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("XRayBoxGuiObject", "Voltage(kV) vs Wait Time(min)", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("XRayBoxGuiObject", "Current(mA) vs Wait Time(min)", 0, QApplication::UnicodeUTF8));
        shutterBox->setTitle(QApplication::translate("XRayBoxGuiObject", "Shutters", 0, QApplication::UnicodeUTF8));
        shutter1->setText(QApplication::translate("XRayBoxGuiObject", "Shutter 1", 0, QApplication::UnicodeUTF8));
        shutter2->setText(QApplication::translate("XRayBoxGuiObject", "Shutter 2", 0, QApplication::UnicodeUTF8));
        shutter3->setText(QApplication::translate("XRayBoxGuiObject", "Shutter 3", 0, QApplication::UnicodeUTF8));
        shutter4->setText(QApplication::translate("XRayBoxGuiObject", "Shutter 4", 0, QApplication::UnicodeUTF8));
        warning->setText(QString());
        highVoltage->setText(QApplication::translate("XRayBoxGuiObject", "HIGH \n"
"VOLTAGE", 0, QApplication::UnicodeUTF8));
        warningText->setText(QApplication::translate("XRayBoxGuiObject", "CAUTION!", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("XRayBoxGuiObject", "       Voltage:", 0, QApplication::UnicodeUTF8));
        setVoltage->setText(QApplication::translate("XRayBoxGuiObject", "Set Voltage", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("XRayBoxGuiObject", "       Current:", 0, QApplication::UnicodeUTF8));
        setCurrent->setText(QApplication::translate("XRayBoxGuiObject", "Set Current", 0, QApplication::UnicodeUTF8));
        getErrorCode->setText(QApplication::translate("XRayBoxGuiObject", "Get Error Code", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("XRayBoxGuiObject", "kV", 0, QApplication::UnicodeUTF8));
        unit1->setText(QApplication::translate("XRayBoxGuiObject", "kV", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("XRayBoxGuiObject", "mA", 0, QApplication::UnicodeUTF8));
        unit2->setText(QApplication::translate("XRayBoxGuiObject", "mA", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("XRayBoxGuiObject", "kV", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("XRayBoxGuiObject", "mA", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class XRayBoxGuiObject: public Ui_XRayBoxGuiObject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_XRAYBOXGUI_H
