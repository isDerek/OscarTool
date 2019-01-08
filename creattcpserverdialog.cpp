#include "creattcpserverdialog.h"
#include "ui_creattcpserverdialog.h"

CreatTCPServerDialog::CreatTCPServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatTCPServerDialog)
{
    ui->setupUi(this);
}

CreatTCPServerDialog::~CreatTCPServerDialog()
{
    delete ui;
}

void CreatTCPServerDialog::on_listenPortBox_accepted()
{
    int localPort = ui->listenPortText->text().toInt();
    emit sendLocalPort(localPort);
}

void CreatTCPServerDialog::on_listenPortBox_rejected()
{
    emit sendRejectSignal();
}
