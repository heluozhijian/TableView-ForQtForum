#include "TableView.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TableView w;
    w.show();

    return a.exec();
}
