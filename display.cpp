#include "display.h"
#include "ui_display.h"
#include <QFile>
#include <QJsonDocument>

display::display(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::display)
{
    ui->setupUi(this);
    centralWidget()->setLayout(ui->centralWgt);
    vReader = new videoReader;
    iPro = new imgProcesser;

    connect(vReader,SIGNAL(newArrived()),this,SLOT(getNewFrame()));
    on_refresh_clicked();
}

display::~display()
{
    delete ui;
}

void display::on_open_clicked()
{
    QVariant path = settings["path"];
    if(vReader->open(path.toString())){
        vReader->timer->start();
        ui->start->setText("pause");
        if(vReader->totalFrame>0){
            int min;
            int sec;
            min = vReader->totalFrame/vReader->fps/60;
            sec = vReader->totalFrame/vReader->fps%60;
            ui->totalTime->setText(QString::number(min)+":"+QString::number(sec));
        }
    }
}

void display::on_close_clicked()
{
    vReader->stop();
}

void display::on_start_clicked()
{
    if(!vReader->timer->isActive()){
        vReader->timer->start();
        ui->start->setText("pause");
    }else{
        vReader->timer->stop();
        ui->start->setText("start");
    }
}

void display::on_videoSlider_valueChanged(int value)
{
    vReader->currentFrame = int(value/99.0*vReader->totalFrame);
}

void display::getNewFrame()
{
    cv::Mat src, dst;
    int w = ui->src->width();
    int h = ui->src->height();
    int w2 = ui->dst->width();
    int h2 = ui->src->height();

    if(vReader->totalFrame>0)// not webcam
    {
        int min;
        int sec;
        min = vReader->currentFrame/vReader->fps/60;
        sec = vReader->currentFrame/vReader->fps%60;
        ui->currentTime->setText(QString::number(min)+":"+QString::number(sec));
        ui->videoSlider->setValue(vReader->currentFrame*99/vReader->totalFrame);
    }


    src = vReader->rawFrame.clone();
    cv::resize(src, src, cv::Size(src.size().width,src.size().height), 0, 0, CV_INTER_LINEAR);
    ui->src->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(src)).scaled(w,h,Qt::KeepAspectRatio));

    dst = iPro->process(src);
    displayMessage(iPro->message);
    cv::resize(dst, dst, cv::Size(dst.size().width,dst.size().height), 0, 0, CV_INTER_LINEAR);
    ui->dst->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(dst)).scaled(w2,h2,Qt::KeepAspectRatio));
}

void display::on_refresh_clicked()
{
    QString val;
    QFile file;
    // for ubuntu
    //file.setFileName("/home/meiqua/dspForImg/settings.json");

    file.setFileName("D:\\dspForImg\\settings.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    settings = QJsonDocument::fromJson(val.toUtf8()).toVariant().toMap();
    iPro->settings = settings;
}

void display::displayMessage(QString src)
{
    ui->message->setText(src);
}
