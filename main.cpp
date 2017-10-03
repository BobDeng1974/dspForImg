#include "display.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    display w;
    w.show();

    return a.exec();
}
