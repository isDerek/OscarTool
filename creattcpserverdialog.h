#ifndef CREATTCPSERVERDIALOG_H
#define CREATTCPSERVERDIALOG_H

#include <QDialog>

namespace Ui {
class CreatTCPServerDialog;
}

class CreatTCPServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreatTCPServerDialog(QWidget *parent = nullptr);
    ~CreatTCPServerDialog();
signals:
    void sendLocalPort(int port);
    void sendRejectSignal();
private slots:
    void on_listenPortBox_accepted();

    void on_listenPortBox_rejected();

private:
    Ui::CreatTCPServerDialog *ui;

};

#endif // CREATTCPSERVERDIALOG_H
