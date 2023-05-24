#ifndef FAILWINDOW_H
#define FAILWINDOW_H

#include <QWidget>
#include "successwindow.h"

namespace Ui {
class FailWindow;
}

class FailWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FailWindow(QWidget *parent = nullptr);
    ~FailWindow();
    void Set_QSS();
    void Set_image();

private slots:
    void on_button_back_clicked();

private:
    Ui::FailWindow *ui;
};

#endif // FAILWINDOW_H
