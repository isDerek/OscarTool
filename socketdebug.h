#ifndef SOCKETDEBUG_H
#define SOCKETDEBUG_H

#include <QMainWindow>

namespace Ui {
class SocketDebug;
}

class SocketDebug : public QMainWindow
{
    Q_OBJECT

public:
    explicit SocketDebug(QWidget *parent = nullptr);
    ~SocketDebug();

private:
    Ui::SocketDebug *ui;
};

#endif // SOCKETDEBUG_H
