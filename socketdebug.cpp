#include "socketdebug.h"
#include "ui_socketdebug.h"

SocketDebug::SocketDebug(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SocketDebug)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Socket 调试助手 QT5 V1.0"));
}

SocketDebug::~SocketDebug()
{
    delete ui;
}
