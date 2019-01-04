#ifndef UARTDEBUG_H
#define UARTDEBUG_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QTimer>
#include <QDateTime>

#include "multiuartsend.h"

namespace Ui {
class UartDebug;
}

class UartDebug : public QMainWindow
{
    Q_OBJECT
public:
    explicit UartDebug(QWidget *parent = nullptr);
    ~UartDebug();
  QSerialPort *m_serial = new QSerialPort; // 串口通信类对象
//    初始化状态栏
    void InitStatusBar(); // 初始化状态栏
    void SetSerState(); // 设置状态栏串口状态
    void SetRecvBytes(); // 设置接收字节数
    void SetSendBytes(); // 设置发送字节数

//    初始化 CombBox 控件
    void InitCommCmb(); // 初始化 CombBox 控件
    void SetPortNumCmb(); // 设置串口号
    QStringList GetEnableCommPortQt(); // 设置计算机可用串口 Qt 方式
    QStringList GetEnableCommPortWin(); // 获取计算机可用串口 Windows API 方式
    QString GetComm(int nIndex, bool bValue = true);
    void SetBaudCmb(); // 设置波特率
    void SetDPaityCmb(); // 设置校验位
    void SetDataBCmb(); // 设置数据位
    void SetStopBCmb(); // 设置停止位
    void SetStreamCmb(); // 设置流控制

    void handleSerialError(QSerialPort::SerialPortError error);
    bool SetSerialPortParam(QSerialPort *serial); // 设置串口参数，失败返回 false，成功返回 true
    char ConvertHexChar(char ch); // 字符转 16 进制
    void StringToHex(QString str, QByteArray &sendData); // 字符串转 16 进制

//    父类重写
    void closeEvent(QCloseEvent *event);

private slots:
    void on_baudRCmb_currentIndexChanged(int index); // 自定义波特率
    void on_OnOffBtn_clicked(); // 打开/断开串口
    void slot_RecvPortData(); // 接收串口数据
    void on_SendBtn_clicked(); // 发送数据
    void on_ClearRecvBtn_clicked(); //清空接收区
    void slot_ResetNumBtn_clicked(); // 复位计数
    void rfSerialPort(); // 定时串口刷新
    void on_ClearSendBtn_clicked(); // 清空发送区
    void timerUpdate(void); // 实时时间更新

    void on_lightOFF_clicked();// ZigBee 关灯按钮

    void on_permitJoinBtn_clicked();// ZigBee 允许入网

    void on_action_New_triggered();// 新建窗口工具栏

    void on_action_Pause_triggered();// 暂停按钮

    void on_action_Play_triggered();// 开始按钮


    void on_multiSendBtn_clicked();// 多项发送

    void receiveMultiData(QByteArray data); // 接收多项发送的槽

private:
    Ui::UartDebug *ui;
//    状态栏控件
    QLabel *m_SerStateLbl; // 串口状态
    QLabel *m_RecvBytesLbl; // 接收字节数
    QLabel *m_SendBytesLbl; // 发送字节数
    QPushButton *m_ResetNumBtn; // 复位计数按钮

//    发送、接收字节数
    int m_nRecvBytes; // 接收字节数
    int m_nSendBytes; // 发送字节数

    bool m_bOpen; // 标识串口状态


    QTimer *rfSerialPortTmr;// 刷新串口计时器
    qint64 m_nReadBuffSize; // 串口缓冲区大小
    QTimer *rtcTimer; // 实时时间
    QTimer *resendTimer; // 自动重发时间

    bool permitRecData;// 允许数据接收
    MultiUartSend *multiUartSend = new MultiUartSend; // 多项发送类实例化
};

#endif // UARTDEBUG_H
