#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>
#include <QCloseEvent>
#include "creattcpserverdialog.h"

namespace Ui {
class TCPServer;
}

class TCPServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TCPServer(QWidget *parent = nullptr);
    ~TCPServer();
    void InitStatusBar();   // 初始化状态栏
    void SetRecvBytes();    // 设置状态栏接收字节
    void SetSendBytes();  // 设置状态栏发送字节

private slots:
    void acceptConnection(); // 获取已建立连接的客户端套接字

    void on_creatTCPServerBtn_clicked(); // 创建 TCP 服务器
    void receiveLocalPort(int localPort); // 接收端口 Dialog 返回的端口号，Dialog 的 OK 事件
    void receiveRejectSignal(); // 端口 DIalog 的 Cancel 事件
    void on_startListeningBtn_clicked(); // 开启监听

    void on_stopListeningBtn_clicked(); //停止监听

    void slot_RevClientData();// 接收客户端传来的数据
    void slot_ResetNumBtn_clicked(); // 复位状态栏计数
    void on_ClearRecvBtn_clicked(); // 清空接收区
    void on_ClearSendBtn_clicked(); // 清空发送区
    void on_SendBtn_clicked(); // 发送按钮
    void StringToHex(QString str, QByteArray &sendData);
    char ConvertHexChar(char ch);
    //    父类重写
    void closeEvent(QCloseEvent *event);
    void on_action_New_triggered();

    void on_action_Play_triggered();

    void on_action_Pause_triggered();

private:
    Ui::TCPServer *ui;
    QTcpServer *TCPServerPtr = new QTcpServer;
    QTcpSocket *clientConnectSocket = new QTcpSocket;
    CreatTCPServerDialog *creatTCPServerDialog = new CreatTCPServerDialog;
    int recLocalPort; // 保存端口 Dialog 的端口号
    bool permitRecData = true;// 允许接收数据
    //    状态栏控件
    QLabel *m_RecvBytesLbl; // 接收字节数
    QLabel *m_SendBytesLbl; // 发送字节数
    QPushButton *m_ResetNumBtn; // 复位计数按钮
    //    发送、接收字节数
    int m_nRecvBytes; // 接收字节数
    int m_nSendBytes; // 发送字节数
    QTimer *resendTimer; // 自动重发时间
};

#endif // TCPSERVER_H
