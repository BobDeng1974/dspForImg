/********************************************************************************
** Form generated from reading UI file 'display.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_display
{
public:
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QVBoxLayout *centralWgt;
    QHBoxLayout *horizontalLayout_3;
    QLabel *src;
    QLabel *dst;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *open;
    QPushButton *close;
    QPushButton *refresh;
    QPushButton *start;
    QHBoxLayout *horizontalLayout_2;
    QSlider *videoSlider;
    QLabel *currentTime;
    QLabel *totalTime;

    void setupUi(QMainWindow *display)
    {
        if (display->objectName().isEmpty())
            display->setObjectName(QString::fromUtf8("display"));
        display->resize(843, 497);
        centralWidget = new QWidget(display);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 363, 361));
        centralWgt = new QVBoxLayout(layoutWidget);
        centralWgt->setSpacing(6);
        centralWgt->setContentsMargins(11, 11, 11, 11);
        centralWgt->setObjectName(QString::fromUtf8("centralWgt"));
        centralWgt->setSizeConstraint(QLayout::SetMaximumSize);
        centralWgt->setContentsMargins(10, 10, 10, 10);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        src = new QLabel(layoutWidget);
        src->setObjectName(QString::fromUtf8("src"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(src->sizePolicy().hasHeightForWidth());
        src->setSizePolicy(sizePolicy);
        src->setMinimumSize(QSize(1, 1));
        src->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(src);

        dst = new QLabel(layoutWidget);
        dst->setObjectName(QString::fromUtf8("dst"));
        sizePolicy.setHeightForWidth(dst->sizePolicy().hasHeightForWidth());
        dst->setSizePolicy(sizePolicy);
        dst->setMinimumSize(QSize(1, 1));
        dst->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(dst);


        centralWgt->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Preferred);

        centralWgt->addItem(verticalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        open = new QPushButton(layoutWidget);
        open->setObjectName(QString::fromUtf8("open"));

        horizontalLayout->addWidget(open);

        close = new QPushButton(layoutWidget);
        close->setObjectName(QString::fromUtf8("close"));

        horizontalLayout->addWidget(close);

        refresh = new QPushButton(layoutWidget);
        refresh->setObjectName(QString::fromUtf8("refresh"));

        horizontalLayout->addWidget(refresh);

        start = new QPushButton(layoutWidget);
        start->setObjectName(QString::fromUtf8("start"));

        horizontalLayout->addWidget(start);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        videoSlider = new QSlider(layoutWidget);
        videoSlider->setObjectName(QString::fromUtf8("videoSlider"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(videoSlider->sizePolicy().hasHeightForWidth());
        videoSlider->setSizePolicy(sizePolicy1);
        videoSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(videoSlider);

        currentTime = new QLabel(layoutWidget);
        currentTime->setObjectName(QString::fromUtf8("currentTime"));

        horizontalLayout_2->addWidget(currentTime);

        totalTime = new QLabel(layoutWidget);
        totalTime->setObjectName(QString::fromUtf8("totalTime"));

        horizontalLayout_2->addWidget(totalTime);


        verticalLayout->addLayout(horizontalLayout_2);


        centralWgt->addLayout(verticalLayout);

        display->setCentralWidget(centralWidget);

        retranslateUi(display);

        QMetaObject::connectSlotsByName(display);
    } // setupUi

    void retranslateUi(QMainWindow *display)
    {
        display->setWindowTitle(QApplication::translate("display", "display", nullptr));
        src->setText(QString());
        dst->setText(QString());
        open->setText(QApplication::translate("display", "open", nullptr));
        close->setText(QApplication::translate("display", "stop", nullptr));
        refresh->setText(QApplication::translate("display", "resetting", nullptr));
        start->setText(QApplication::translate("display", "start", nullptr));
        currentTime->setText(QApplication::translate("display", "0:00", nullptr));
        totalTime->setText(QApplication::translate("display", "0:00", nullptr));
    } // retranslateUi

};

namespace Ui {
    class display: public Ui_display {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAY_H
