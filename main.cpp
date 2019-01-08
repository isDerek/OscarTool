#include "oscartool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OscarTool w;
    w.setWindowIcon(QIcon(":/myImages/images/run.png"));
    w.show();

    return a.exec();
}
