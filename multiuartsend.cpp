#include "multiuartsend.h"
#include "ui_multiuartsend.h"
#include "uartdebug.h"


MultiUartSend::MultiUartSend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiUartSend)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("串口多项发送"));
}

MultiUartSend::~MultiUartSend()
{
    delete ui;
}
char MultiUartSend::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
    {
        return ch-0x30;
    }
    else if(ch >= 'A' && ch <= 'F')
    {
        return ch-'A'+10;
    }
    else if(ch >= 'a' && ch <= 'f')
    {
        return ch-'a'+10;
    }
    else return ch-ch;
}

void MultiUartSend::StringToHex(QString str, QByteArray &sendData)
{
    char hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    sendData.resize(len/2);
    char lstr,hstr;
    for (int i=0; i<len;) {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
//            qDebug()<< tr("null");
            i++;
            continue;
        }
        i++;
        if(i>=len)
        {
//            qDebug()<< tr("over");
            break;
        }
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
        {
//            qDebug()<< tr("error");
            break;
        }
        else
        {
            hexdata =  hexdata *16 + lowhexdata;
        }
        i++;
//        qDebug("转化为16进制数：%d",hexdata);
        sendData[hexdatalen] = hexdata;
        hexdatalen++;
    }
    sendData.resize(hexdatalen);
//    qDebug()<<sendData.length();
}
void MultiUartSend::on_multiSendBtn_1_clicked()
{
    QString SendText = ui->multiText_1->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_1->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_2_clicked()
{
    QString SendText = ui->multiText_2->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_2->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_3_clicked()
{
    QString SendText = ui->multiText_3->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_3->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_4_clicked()
{
    QString SendText = ui->multiText_4->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_4->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_5_clicked()
{
    QString SendText = ui->multiText_5->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_5->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_6_clicked()
{
    QString SendText = ui->multiText_6->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_6->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_7_clicked()
{
    QString SendText = ui->multiText_7->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_7->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}

void MultiUartSend::on_multiSendBtn_8_clicked()
{
    QString SendText = ui->multiText_8->text();
    QByteArray SendTextArray;
    SendTextArray = SendText.toLatin1();
    if(ui->multiHexBtn_8->checkState())
    {
        StringToHex(SendText,SendTextArray);
    }
    emit sendMultiData(SendTextArray);
}
void MultiUartSend::Delay_Msec(int msec)
{
    QTime _Time = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime()<_Time)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MultiUartSend::on_listLoopSendBtn_clicked()
{
    for(int i =0; i<ui->listLoopCounts->value(); i++)
    {
        if(ui->multiText_1->text() != "")
        {
            on_multiSendBtn_1_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_2->text() != "")
        {
            on_multiSendBtn_2_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_3->text() != "")
        {
            on_multiSendBtn_3_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_4->text() != "")
        {
            on_multiSendBtn_4_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_5->text() != "")
        {
            on_multiSendBtn_5_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_6->text() != "")
        {
            on_multiSendBtn_6_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_7->text() != "")
        {
            on_multiSendBtn_7_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
        if(ui->multiText_8->text() != "")
        {
            on_multiSendBtn_8_clicked();
            Delay_Msec(ui->listLoopTime->value());
        }
    }


}
