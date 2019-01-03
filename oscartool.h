#ifndef OSCARTOOL_H
#define OSCARTOOL_H

#include <QMainWindow>
#include "uartdebug.h"
#include "socketdebug.h"

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
    UartDebug *uartdebug = new UartDebug;
    SocketDebug *socketdebug = new SocketDebug;
private slots:
    void on_uartDebug_clicked();
    void on_socketDebug_clicked();
};

#endif // OSCARTOOL_H
