#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "successwindow.h"
#include "failwindow.h"

#include "../utils/CoordinateUtil.h"
#include "../utils/TypeUtil.h"
#include "../utils/RandomUtil.h"
#include "../grpc/greeterClient.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Set_QSS();
    void Set_image();
    void Set_pk();
    void Set_coordinate();

private slots:
    void on_button_submit_clicked();
    void on_button_get_clicked();

private:
    Ui::MainWindow *ui;
    QString qpk;
    QString qlat;
    QString qlon;
signals:
    void sendData(QString,QString);
};
#endif // MAINWINDOW_H
