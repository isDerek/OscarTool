#ifndef EXUART_H
#define EXUART_H

#include <QWidget>

namespace Ui {
class ExUart;
}

class ExUart : public QWidget
{
    Q_OBJECT

public:
    explicit ExUart(QWidget *parent = nullptr);
    ~ExUart();

private:
    Ui::ExUart *ui;
};

#endif // EXUART_H
