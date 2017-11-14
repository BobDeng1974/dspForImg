#ifndef DISPLAY_H
#define DISPLAY_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <imageformat.h>
#include "videoreader.h"
#include "imgprocesser.h"
#include <QVariantMap>

namespace Ui {
class display;
}

class display : public QMainWindow
{
    Q_OBJECT

public:
    explicit display(QWidget *parent = 0);
    ~display();

private slots:
    void on_open_clicked();

    void on_close_clicked();

    void on_start_clicked();

    void on_videoSlider_valueChanged(int value);

    void getNewFrame();
    void on_refresh_clicked();

private:
    Ui::display *ui;
    videoReader *vReader;
    imgProcesser *iPro;
    QVariantMap settings;

    QString platformPrefix;
};

#endif // DISPLAY_H
