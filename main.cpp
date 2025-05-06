#include "Surveillance.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Surveillance w;
    w.show();
    return a.exec();
}
