#include "MergeBinTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MergeBinTool w;
    w.show();
    return a.exec();
}
