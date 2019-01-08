#ifndef OSCARTOOL_H
#define OSCARTOOL_H

#include <QMainWindow>
#include "uartdebug.h"
#include "tcpserver.h"
namespace Ui {
class OscarTool;
}

class OscarTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit OscarTool(QWidget *parent = nullptr);
    ~OscarTool();

public:
    Ui::OscarTool *ui;
    UartDebug *uartdebug;
    TCPServer *tcpserver;
private slots:
    void on_uartDebugBtn_clicked();
    void on_TCPServerBtn_clicked();
};

#endif // OSCARTOOL_H
