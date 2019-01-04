#include "uartdebug.h"
#include "ui_uartdebug.h"

#include <QSettings>
#include <qt_windows.h>
#include <QMessageBox>
#include <QDebug>

UartDebug::UartDebug(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UartDebug)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("串口调试助手 QT5 V1.0"));

    m_nRecvBytes = 0;
    m_nSendBytes = 0;
    m_bOpen = false;
    permitRecData = true;
    m_nReadBuffSize = 64;
//    m_serial = new QSerialPort;
    rfSerialPortTmr = new QTimer;
    resendTimer = new QTimer;
    rtcTimer = new QTimer;
    InitStatusBar();
    InitCommCmb();
    ui->RecvDataEdt->setReadOnly(true);
    // 自动重发定时器
    connect(resendTimer, SIGNAL(timeout()), this, SLOT(on_SendBtn_clicked()));
    // 实时时间
//    connect(rtcTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
//    rtcTimer->start(1000);
    // 自动刷新串口定时器
    connect(rfSerialPortTmr,SIGNAL(timeout()),this,SLOT(rfSerialPort()));
    rfSerialPortTmr->start(1000);
    ui->OnOffBtn->setIcon(QIcon(":/myImages/images/run.png"));
    // 捕捉接收信号定时器
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(slot_RecvPortData()));
    // 接收多项发送的信号处理
    connect(multiUartSend, SIGNAL(sendMultiData(QByteArray)), SLOT(receiveMultiData(QByteArray)));
}

void UartDebug::timerUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss.zzz dddd");
    qDebug()<< str;
}

// 刷新串口处理函数
void UartDebug::rfSerialPort()
{
    // 刷新串口，动态监测串口
    SetPortNumCmb();
    // 监测串口是否被拔出
    handleSerialError(m_serial->error());
}
// 检测到 USB 设备被溢出，关闭串口，并弹出提示
void UartDebug::handleSerialError(QSerialPort::SerialPortError error)
{
//    qDebug() << error;
    if(error == QSerialPort::ResourceError || error == QSerialPort::PermissionError)
    {
        QMessageBox::critical(this,tr("Error"),"串口连接中断，请检查是否正确连接");
        if(m_serial->isOpen())
        {
            m_serial->clearError();
            m_serial->close();
            ui->OnOffBtn->setText("打开");
            ui->OnOffBtn->setIcon(QIcon(":/myImages/images/run.png"));
            m_bOpen = false;
            ui->PortNumCmb->setEnabled(true);
            ui->DataBCmb->setEnabled(true);
            ui->DPaityCmb->setEnabled(true);
            ui->StopBCmb->setEnabled(true);
            ui->FlowCtrlCmb->setEnabled(true);
            ui->baudRCmb->setEnabled(true);
            SetSerState();
        }
    }
}
UartDebug::~UartDebug()
{
    delete ui;
    delete m_SerStateLbl;
    delete m_RecvBytesLbl;
    delete m_SendBytesLbl;
    delete m_ResetNumBtn;
}

//初始化状态栏

void UartDebug::InitStatusBar()
{
    m_SerStateLbl = new QLabel();
    m_RecvBytesLbl = new QLabel();
    m_SendBytesLbl = new QLabel();
    m_ResetNumBtn = new QPushButton();
    connect(m_ResetNumBtn, SIGNAL(clicked()), this, SLOT(slot_ResetNumBtn_clicked()));

    m_SerStateLbl->setMinimumSize(180,20);
    m_RecvBytesLbl->setMinimumSize(180,20);
    m_SendBytesLbl->setMinimumSize(180,20);
    ui->statusbar->addWidget(m_SerStateLbl);
    ui->statusbar->addWidget(m_RecvBytesLbl);
    ui->statusbar->addWidget(m_SendBytesLbl);
    ui->statusbar->addWidget(m_ResetNumBtn);

    SetSerState();
    SetRecvBytes();
    SetSendBytes();
    m_ResetNumBtn->setText("复位计数");
}

void UartDebug::SetSerState()
{
    QString strState;
    if (m_bOpen)
        strState = tr("打开");
    else {
        strState = tr("关闭");
    }

    m_SerStateLbl->setText(tr("串口状态：%1").arg(strState));
}

void UartDebug::SetRecvBytes()
{
    QString strRecvBytes = QString::number(m_nRecvBytes);
    m_RecvBytesLbl->setText(tr("接收字节数：%1").arg(strRecvBytes));
}

void UartDebug::SetSendBytes()
{
    QString strSendBytes = QString::number(m_nSendBytes);
    m_SendBytesLbl->setText(tr("发送字节数:%1").arg(strSendBytes));
}

//初始化 CombBox
void UartDebug::InitCommCmb()
{
    SetPortNumCmb(); // 串口号
    SetBaudCmb(); // 波特率
    SetDPaityCmb(); // 校验位
    SetDataBCmb(); // 数据位
    SetStopBCmb(); // 停止位
    SetStreamCmb(); // 流控制
}

// 设置串口号
void UartDebug::SetPortNumCmb()
{
    QStringList commPortList = GetEnableCommPortQt();
    QStringList portItems;
    int portItemsCounts;
    // 如果串口正在使用，然后串口定时刷新，当前串口字段依旧保留
    if(m_serial->isOpen())
    {
      QString serialPort = m_serial->portName();
      commPortList.insert(0,serialPort);
    }
//    qDebug() << commPortList;
    if(! commPortList.isEmpty())
    {
        portItemsCounts = ui->PortNumCmb->count();
        for(int i = 0; i<portItemsCounts;i++)
        {
            portItems.append(ui->PortNumCmb->itemText(i));
        }
        if(portItems == commPortList)
        {
            return;
        }
        else
        {
          ui->PortNumCmb->clear();
          ui->PortNumCmb->addItems(commPortList);
        }
    }
}

// 获取计算机可用串口 QSerialPort QSerialPortInfo 类
QStringList UartDebug::GetEnableCommPortQt()
{
    QStringList CommPortList;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            CommPortList.append(serial.portName());
            serial.close();
        }
    }

    return CommPortList;
}

// 设置波特率

void UartDebug::SetBaudCmb()
{
    QStringList baudRList;
    baudRList.append(tr("110"));
    baudRList.append(tr("300"));
    baudRList.append(tr("4800"));
    baudRList.append(tr("9600"));
    baudRList.append(tr("115200"));
    baudRList.append(tr("自定义"));

    ui->baudRCmb->addItems(baudRList);
    ui->baudRCmb->setCurrentIndex(4);
}

// 设置校验位
void UartDebug::SetDPaityCmb()
{
    QStringList DPaityList;
    DPaityList.append(tr("NONE"));
    DPaityList.append(tr("ODD"));
    DPaityList.append(tr("EVEN"));
    DPaityList.append(tr("MARK"));
    DPaityList.append(tr("SPACE"));

    ui->DPaityCmb->addItems(DPaityList);
}

// 设置数据位
void UartDebug::SetDataBCmb()
{
    for(int i = 5; i<= 8; i++)
    {
        QString strDataB = QString::number(i);
        ui->DataBCmb->addItem(strDataB);
    }
    ui->DataBCmb->setCurrentIndex(3);
}

// 设置停止位
void UartDebug::SetStopBCmb()
{
    ui->StopBCmb->addItem(tr("1"));
    ui->StopBCmb->addItem(tr("1.5"));
    ui->StopBCmb->addItem(tr("2"));
}

// 流控制
void UartDebug::SetStreamCmb()
{
    ui->FlowCtrlCmb->addItem(tr("NO"));
    ui->FlowCtrlCmb->addItem(tr("RTS/CTS"));
    ui->FlowCtrlCmb->addItem(tr("XON/XOFF"));
}

//槽函数
// 波特率自定义
void UartDebug::on_baudRCmb_currentIndexChanged(int index)
{
    int nCount = ui->baudRCmb->count();;
    // 要做修改，用最新的写法
    if(index == nCount - 1)
    {
        ui->baudRCmb->setEditable(TRUE);
        ui->baudRCmb->setItemText(index, tr(""));
    }
}

// 复位计数
void UartDebug::slot_ResetNumBtn_clicked()
{
    m_nSendBytes = 0;
    m_nRecvBytes = 0;
    SetSendBytes();
    SetRecvBytes();
}
// 清空接收区
void UartDebug::on_ClearRecvBtn_clicked()
{
    ui->RecvDataEdt->setText(tr(""));
}
// 清空发送区
void UartDebug::on_ClearSendBtn_clicked()
{
    ui->SendDataEdt->setText(tr(""));
}

// 串口通信
// 打开/关闭串口
void UartDebug::on_OnOffBtn_clicked()
{
    if(m_serial->isOpen())
    {
        m_serial->close();
        ui->OnOffBtn->setText("打开");
        ui->OnOffBtn->setIcon(QIcon(":/myImages/images/run.png"));
        m_bOpen = false;
        ui->PortNumCmb->setEnabled(true);
        ui->DataBCmb->setEnabled(true);
        ui->DPaityCmb->setEnabled(true);
        ui->StopBCmb->setEnabled(true);
        ui->FlowCtrlCmb->setEnabled(true);
        ui->baudRCmb->setEnabled(true);
        SetSerState();
        resendTimer->stop(); //  断开串口，重发定时器要停止
    }
    else
    {
        if( !SetSerialPortParam(m_serial))
        {
            QMessageBox::critical(this, tr("Error"), tr("串口错误"), QMessageBox::Ok);
            return;
        }

        if(!m_serial->open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this, tr("Error"), tr("串口不存在或者被其它程序占用"),QMessageBox::Ok);
            ui->RecvDataEdt->append(tr("\n [Error] Can't Open COM Port!"));
            return;
        }

        m_serial->setReadBufferSize(m_nReadBuffSize);

        ui->OnOffBtn->setText("断开");
        ui->OnOffBtn->setIcon(QIcon(":/myImages/images/close.png"));
        m_bOpen = true;
        ui->PortNumCmb->setEnabled(false);
        ui->DataBCmb->setEnabled(false);
        ui->DPaityCmb->setEnabled(false);
        ui->StopBCmb->setEnabled(false);
        ui->FlowCtrlCmb->setEnabled(false);
        ui->baudRCmb->setEnabled(false);
        SetSerState();
    }
}
// 设置串口参数，失败返回 false, 成功返回 true
bool UartDebug::SetSerialPortParam(QSerialPort *serial)
{
    // 设置串口号
    QString strPortNum = ui->PortNumCmb->currentText(); // 当前的串口
    if(strPortNum == tr(""))
    {
        return false;
    }
    serial->setPortName(strPortNum);

    // 设置波特率
    qint32 nBaudRate = ui->baudRCmb->currentText().toInt();
    serial->setBaudRate(nBaudRate);

    // 设置奇偶校验
    int nParityType = ui->DPaityCmb->currentIndex();
    switch (nParityType) {
    case 0:
        serial->setParity(QSerialPort::NoParity);
        break;
    case 1:
        serial->setParity(QSerialPort::OddParity);
        break;
    case 2:
        serial->setParity(QSerialPort::EvenParity);
        break;
    case 3:
        serial->setParity(QSerialPort::MarkParity);
        break;
    case 4:
        serial->setParity(QSerialPort::SpaceParity);
        break;
    case 5:
        serial->setParity(QSerialPort::UnknownParity);
        break;
    default:
        break;
    }

    // 设置数据位
    int nDataBits = ui->DataBCmb->currentIndex();
    switch (nDataBits) {
    case 0:
        serial->setDataBits(QSerialPort::Data5);
        break;
    case 1:
        serial->setDataBits(QSerialPort::Data6);
        break;
    case 2:
        serial->setDataBits(QSerialPort::Data7);
        break;
    case 3:
        serial->setDataBits(QSerialPort::Data8);
        break;
    default:
        serial->setDataBits(QSerialPort::UnknownDataBits);
        break;
    }

    // 设置停止位
    int nStopBits = ui->StopBCmb->currentIndex();
    switch (nStopBits) {
    case 0:
        serial->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        serial->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        serial->setStopBits(QSerialPort::TwoStop);
        break;
    default:
        serial->setStopBits(QSerialPort::UnknownStopBits);
        break;
    }
    // 流控制
    int nFlowCtrl = ui->FlowCtrlCmb->currentIndex();
    switch (nFlowCtrl) {
    case 0:
        serial->setFlowControl(QSerialPort::NoFlowControl);
        break;
    case 1:
        serial->setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        serial->setFlowControl(QSerialPort::SoftwareControl);
        break;
    case 3:
        serial->setFlowControl(QSerialPort::UnknownFlowControl);
    }
    return true;
}
// 槽函数，接收串口数据
void UartDebug::slot_RecvPortData()
{
    QByteArray bytes;
    bytes = bytes.append(m_serial->readAll());
    // 接受到的数据转为 16 进制
    if(ui->HexReceiveBtn->checkState())
    {
       bytes = bytes.toHex().toUpper().simplified();
       int len = bytes.count()/2;
       for(int i = 1; i<len+1; i++)
       {
           bytes.insert(3*i-1," ");
       }
    }
    qDebug() << bytes;
    if(!bytes.isEmpty()&& permitRecData)
    {
        QString strRecv = QString::fromLocal8Bit(bytes);
        ui->RecvDataEdt->insertPlainText(strRecv);
        ui->RecvDataEdt->moveCursor(QTextCursor::End);//保证 Text 数据不会自动换行
        m_nRecvBytes += bytes.count();
        SetRecvBytes();
    }
}
// 发送数据，写串口
void UartDebug::on_SendBtn_clicked()
{
    // 串口未打开
    if(!m_bOpen)
    {
        m_SerStateLbl->setText(tr("串口状态：串口未打开，发送失败"));
        return;
    }
    QByteArray SendBytes = ui->SendDataEdt->toPlainText().toLocal8Bit();
    // 如果发送窗口为空，则不发送
    if(SendBytes == "")
    {
        return;
    }
    // 发送按钮，16进制接收显示状态，在接收做处理
    if(ui->HexSendBtn->checkState())
    {
       QByteArray HexBytes = ui->SendDataEdt->toPlainText().toLocal8Bit();
       StringToHex(HexBytes,SendBytes);
    }
    // 发送按钮，自动重发
    if(ui->autoResendBtn->checkState())
    {
        int resendCounts;
        resendCounts = ui->resendTimer->value();
        resendTimer->start(resendCounts);
    }
    // 自动重发没选中，要停止定时器
    else
    {
        resendTimer->stop();
    }
    // 显示新行选中，字段末尾添加换行
    if(ui->SendNewLineBtn->checkState())
    {
        SendBytes.append("\n\r");
    }
    if(!SendBytes.isEmpty())
    {
//        qDebug("发送字节："+SendBytes);
        m_serial->write(SendBytes);
        m_nSendBytes += SendBytes.count();
        SetSendBytes();
    }
    // 发送按钮，显示发送,必须置于函数最后端，保证逻辑正确
    if(ui->viewSendBtn->checkState())
    {
        // 发送按钮，16进制接收显示状态
        if(ui->HexReceiveBtn->checkState())
        {
           SendBytes = SendBytes.toHex().toUpper().simplified();
           int len = SendBytes.count()/2;
           for(int i = 1; i<len+1; i++)
           {
               SendBytes.insert(3*i-1," ");
           }
        }
        SendBytes.insert(0,"SendData:");
        SendBytes.append("\n\r");
        QString strRecv = QString::fromLocal8Bit(SendBytes);
        ui->RecvDataEdt->insertPlainText(strRecv);
        ui->RecvDataEdt->moveCursor(QTextCursor::End);//保证 Text 数据不会自动换行
        if(ui->HexReceiveBtn->checkState())
        {
            m_nRecvBytes += (SendBytes.count()-11)/3;//去掉添加的多余字符,16 进制 3 个字符为一组
        }
        else
        {
            m_nRecvBytes += SendBytes.count()-11;//去掉添加的多余字符
        }

        SetRecvBytes();
    }

}

// 函数重写

// 串口关闭时间，如果窗口关闭前串口未关闭，则关闭串口
void UartDebug::closeEvent(QCloseEvent *event)
{
    if(m_serial->isOpen())
    {
        m_serial->close();
    }
    event->accept();
}
char UartDebug::ConvertHexChar(char ch)
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

void UartDebug::StringToHex(QString str, QByteArray &sendData)
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

void UartDebug::on_lightOFF_clicked()
{
    // 串口未打开
    if(!m_bOpen)
    {
        m_SerStateLbl->setText(tr("串口状态：串口未打开，发送失败"));
        return;
    }
    QByteArray lightOFFBytes = "0102109202100216930214FFFC02110211021003";// 关灯
    QByteArray SendBytes;
    StringToHex(lightOFFBytes,SendBytes);
    if(!SendBytes.isEmpty())
    {
        m_serial->write(SendBytes);
        m_nSendBytes += SendBytes.count();
        SetSendBytes();
    }
    // 发送按钮，显示发送,必须置于函数最后端，保证逻辑正确
    if(ui->viewSendBtn->checkState())
    {
        // 发送按钮，16进制接收显示状态
        if(ui->HexReceiveBtn->checkState())
        {
           SendBytes = SendBytes.toHex().toUpper().simplified();
           int len = SendBytes.count()/2;
           for(int i = 1; i<len+1; i++)
           {
               SendBytes.insert(3*i-1," ");
           }
        }
        SendBytes.insert(0,"SendData:");
        SendBytes.append("\n\r");
        QString strRecv = QString::fromLocal8Bit(SendBytes);
        ui->RecvDataEdt->insertPlainText(strRecv);
        ui->RecvDataEdt->moveCursor(QTextCursor::End);//保证 Text 数据不会自动换行
        m_nRecvBytes += (SendBytes.count()-11)/3; //去掉添加的多余字符,16 进制 3 个字符为一组
        SetRecvBytes();
    }
}


void UartDebug::on_permitJoinBtn_clicked()
{
    // 串口未打开
    if(!m_bOpen)
    {
        m_SerStateLbl->setText(tr("串口状态：串口未打开，发送失败"));
        return;
    }
    QByteArray lightOFFBytes = "01021049021002147EFFFC30021003";// 关灯
    QByteArray SendBytes;
    StringToHex(lightOFFBytes,SendBytes);
    if(!SendBytes.isEmpty())
    {
        m_serial->write(SendBytes);
        m_nSendBytes += SendBytes.count();
        SetSendBytes();
    }
    // 发送按钮，显示发送,必须置于函数最后端，保证逻辑正确
    if(ui->viewSendBtn->checkState())
    {
        // 发送按钮，16进制接收显示状态
        if(ui->HexReceiveBtn->checkState())
        {
           SendBytes = SendBytes.toHex().toUpper().simplified();
           int len = SendBytes.count()/2;
           for(int i = 1; i<len+1; i++)
           {
               SendBytes.insert(3*i-1," ");
           }
        }
        SendBytes.insert(0,"SendData:");
        SendBytes.append("\n\r");
        QString strRecv = QString::fromLocal8Bit(SendBytes);
        ui->RecvDataEdt->insertPlainText(strRecv);
        ui->RecvDataEdt->moveCursor(QTextCursor::End);//保证 Text 数据不会自动换行
        m_nRecvBytes += (SendBytes.count()-11)/3;//去掉添加的多余字符,16 进制 3 个字符为一组
        SetRecvBytes();
    }
}
// 新建窗口
void UartDebug::on_action_New_triggered()
{
    UartDebug *uartdebug = new UartDebug;
    uartdebug->move(100,100); // 新建的窗口不和原来窗口重合
    uartdebug->show();
}

void UartDebug::on_action_Pause_triggered()
{
    permitRecData = false;
    ui->action_Pause->setEnabled(false);
    ui->action_Play->setEnabled(true);
}

void UartDebug::on_action_Play_triggered()
{
    permitRecData = true;
    ui->action_Play->setEnabled(false);
    ui->action_Pause->setEnabled(true);
}

void UartDebug::on_multiSendBtn_clicked()
{
    multiUartSend->show();
}

void UartDebug::receiveMultiData(QByteArray data)
{
    // 串口未打开
    if(!m_bOpen)
    {
        m_SerStateLbl->setText(tr("串口状态：串口未打开，发送失败"));
        return;
    }
    QByteArray SendBytes = data;
    // 如果发送窗口为空，则不发送
    if(SendBytes == "")
    {
        return;
    }
    if(!SendBytes.isEmpty())
    {
//        qDebug("发送字节："+SendBytes);
        m_serial->write(SendBytes);
        m_nSendBytes += SendBytes.count();
        SetSendBytes();
    }
    // 发送按钮，显示发送,必须置于函数最后端，保证逻辑正确
    if(ui->viewSendBtn->checkState())
    {
        // 发送按钮，16进制接收显示状态
        if(ui->HexReceiveBtn->checkState())
        {
           SendBytes = SendBytes.toHex().toUpper().simplified();
           int len = SendBytes.count()/2;
           for(int i = 1; i<len+1; i++)
           {
               SendBytes.insert(3*i-1," ");
           }
        }
        SendBytes.insert(0,"SendData:");
        SendBytes.append("\n\r");
        QString strRecv = QString::fromLocal8Bit(SendBytes);
        ui->RecvDataEdt->insertPlainText(strRecv);
        ui->RecvDataEdt->moveCursor(QTextCursor::End);//保证 Text 数据不会自动换行
        if(ui->HexReceiveBtn->checkState())
        {
            m_nRecvBytes += (SendBytes.count()-11)/3;//去掉添加的多余字符,16 进制 3 个字符为一组
        }
        else
        {
            m_nRecvBytes += SendBytes.count()-11;//去掉添加的多余字符
        }

        SetRecvBytes();
    }
}
