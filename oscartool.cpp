#include "oscartool.h"
#include "ui_oscartool.h"
OscarTool::OscarTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OscarTool)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("奥斯卡工具 QT5 V1.0"));
}

OscarTool::~OscarTool()
{
    delete ui;
}

void OscarTool::on_uartDebug_clicked()
{
//    this->hide();
    uartdebug->show();
}

void OscarTool::on_socketDebug_clicked()
{
    socketdebug->show();
}
