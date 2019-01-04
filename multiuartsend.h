#ifndef MULTIUARTSEND_H
#define MULTIUARTSEND_H

#include <QWidget>

namespace Ui {
class MultiUartSend;
}

class MultiUartSend : public QWidget
{
    Q_OBJECT

public:
    explicit MultiUartSend(QWidget *parent = nullptr);
    ~MultiUartSend();
    char ConvertHexChar(char ch); // 字符转 16 进制
    void StringToHex(QString str, QByteArray &sendData); // 字符串转 16 进制
    void Delay_Msec(int msec);// 延时函数
signals:
    void sendMultiData(QByteArray);
public slots:
    void on_multiSendBtn_1_clicked();

private slots:
    void on_multiSendBtn_2_clicked();

    void on_multiSendBtn_3_clicked();

    void on_multiSendBtn_4_clicked();

    void on_multiSendBtn_5_clicked();

    void on_multiSendBtn_6_clicked();

    void on_multiSendBtn_7_clicked();

    void on_multiSendBtn_8_clicked();

    void on_listLoopSendBtn_clicked();

private:
    Ui::MultiUartSend *ui;
};

#endif // MULTIUARTSEND_H
