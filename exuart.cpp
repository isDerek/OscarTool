#include "exuart.h"
#include "ui_exuart.h"

ExUart::ExUart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExUart)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
}

ExUart::~ExUart()
{
    delete ui;
}
