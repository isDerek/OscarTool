#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QDebug>
#include <QMessageBox>
#include <QtNetwork>

TCPServer::TCPServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TCPServer)
{
    m_nRecvBytes = 0;
    m_nSendBytes = 0;
    ui->setupUi(this);
    this->setWindowIcon((QIcon(":/myImages/images/run.png")));
    this->setWindowTitle(tr("TCP 服务器 QT5 V1.0"));
    ui->stopListeningBtn->setEnabled(false);
    ui->startListeningBtn->setEnabled(false);
    ui->RecvDataEdt->setReadOnly(true);
    InitStatusBar();
    // 自动重发定时器
    resendTimer = new QTimer;
    connect(resendTimer, SIGNAL(timeout()), this, SLOT(on_SendBtn_clicked()));
}

TCPServer::~TCPServer()
{
    delete ui;
}
void TCPServer::InitStatusBar()
{
    m_RecvBytesLbl = new QLabel();
    m_SendBytesLbl = new QLabel();
    m_ResetNumBtn = new QPushButton();
    connect(m_ResetNumBtn, SIGNAL(clicked()), this, SLOT(slot_ResetNumBtn_clicked()));

    m_RecvBytesLbl->setMinimumSize(180,20);
    m_SendBytesLbl->setMinimumSize(180,20);
    ui->statusbar->addWidget(m_RecvBytesLbl);
    ui->statusbar->addWidget(m_SendBytesLbl);
    ui->statusbar->addWidget(m_ResetNumBtn);

    SetRecvBytes();
    SetSendBytes();
    m_ResetNumBtn->setText("复位计数");
}

void TCPServer::SetRecvBytes()
{
    QString strRecvBytes = QString::number(m_nRecvBytes);
    m_RecvBytesLbl->setText(tr("接收字节数：%1").arg(strRecvBytes));
}

void TCPServer::SetSendBytes()
{
    QString strSendBytes = QString::number(m_nSendBytes);
    m_SendBytesLbl->setText(tr("发送字节数:%1").arg(strSendBytes));
}

// 接收客户端连接
void TCPServer::acceptConnection()
{
    // 构造回复字符串
    QString Msg = "server:I am server, we are already connected.";
    QString clientIP;
    quint16 clientPort;
    char *replyMsg = Msg.toUtf8().data();

    // 我们获取已经建立连接的子套接字
    clientConnectSocket = TCPServerPtr->nextPendingConnection();
    clientIP = clientConnectSocket->peerAddress().toString();
    clientIP = clientIP.mid(7,20);// 截取客户端 IP 地址部分
    clientPort = clientConnectSocket->peerPort();
    ui->clientIPText->setText(clientIP);
    ui->clientPortText->setNum(clientPort);
    connect(clientConnectSocket,SIGNAL(disconnected()),clientConnectSocket,SLOT(deleteLater()));
    clientConnectSocket->write(replyMsg);
    // 为服务端 Socket 建立信号与槽以响应客户端来信
    connect(clientConnectSocket,SIGNAL(readyRead()),this,SLOT(slot_RevClientData()));
}

void TCPServer::slot_RevClientData()
{
    QByteArray bytes;
    bytes = bytes.append(clientConnectSocket->readAll());
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

void TCPServer::receiveLocalPort(int localPort)
{
    // 设置好端口号后，若是二次点击创建 TCP 服务器，则释放前一次的 TCP 服务器，重新分配空间
    delete TCPServerPtr;
    TCPServerPtr = new QTcpServer;

    // 用户设置好端口号后，按钮使能
    ui->stopListeningBtn->setEnabled(false);
    ui->startListeningBtn->setEnabled(true);
//    qDebug()<<localPort;
    recLocalPort = localPort; // 保存 localPort 到类的私有变量
    // 初始化网络参数显示
    ui->socketStatusText->setText("监听未开启");
    ui->clientIPText->clear();
    ui->clientPortText->clear();

    // 查询本地 IP 地址
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    ui->localIP->setText(info.addresses().last().toString());

    ui->localPort->setNum(localPort);
    creatTCPServerDialog->hide();
}

void TCPServer::receiveRejectSignal()
{
    creatTCPServerDialog->hide();
}

void TCPServer::on_creatTCPServerBtn_clicked()
{
    connect(creatTCPServerDialog, SIGNAL(sendLocalPort(int)), SLOT(receiveLocalPort(int)));
    connect(creatTCPServerDialog, SIGNAL(sendRejectSignal()), SLOT(receiveRejectSignal()));
    creatTCPServerDialog->show();
}

void TCPServer::on_startListeningBtn_clicked()
{
    TCPServerPtr = new QTcpServer;
    // 如果 TCP 服务器异常，则关闭窗口
    if(!TCPServerPtr->listen(QHostAddress::Any,recLocalPort))
    {
        qDebug()<< TCPServerPtr->errorString();
        close();
    }
    connect(TCPServerPtr,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    ui->stopListeningBtn->setEnabled(true);
    ui->startListeningBtn->setEnabled(false);
    ui->socketStatusText->setText("监听已开启");
}

void TCPServer::on_stopListeningBtn_clicked()
{
    // 关闭 TCP 服务器监听，释放 TPCServer 指针，是因为重新开启监听后，原本的监听 SOCKET 端口的客户端建立连接的 SOCKET 还在
    if(TCPServerPtr->isListening())
    {
        TCPServerPtr->close();
        delete TCPServerPtr;
        ui->stopListeningBtn->setEnabled(false);
        ui->startListeningBtn->setEnabled(true);
        ui->socketStatusText->setText("监听未开启");
        ui->clientIPText->clear();
        ui->clientPortText->clear();
    }

}
// 复位计数
void TCPServer::slot_ResetNumBtn_clicked()
{
    m_nSendBytes = 0;
    m_nRecvBytes = 0;
    SetSendBytes();
    SetRecvBytes();
}
// 清空接收区
void TCPServer::on_ClearRecvBtn_clicked()
{
    ui->RecvDataEdt->setText(tr(""));
}
// 清空发送区
void TCPServer::on_ClearSendBtn_clicked()
{
    ui->SendDataEdt->setText(tr(""));
}
char TCPServer::ConvertHexChar(char ch)
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

void TCPServer::StringToHex(QString str, QByteArray &sendData)
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
void TCPServer::on_SendBtn_clicked()
{
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
       qDebug()<<SendBytes;
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
        clientConnectSocket->write(SendBytes);
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

// 窗口关闭，释放服务器指针
void TCPServer::closeEvent(QCloseEvent *event)
{
    delete TCPServerPtr;
    event->accept();
}

void TCPServer::on_action_New_triggered()
{
    TCPServer *tcpserver = new TCPServer;
    tcpserver->show();
}

void TCPServer::on_action_Play_triggered()
{
    permitRecData = true;
    ui->action_Play->setEnabled(false);
    ui->action_Pause->setEnabled(true);
}

void TCPServer::on_action_Pause_triggered()
{
    permitRecData = false;
    ui->action_Pause->setEnabled(false);
    ui->action_Play->setEnabled(true);
}
