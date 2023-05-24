#include "failwindow.h"
#include "ui_failwindow.h"
#include "QFile"

FailWindow::FailWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FailWindow)
{
    ui->setupUi(this);
    Set_QSS();
    Set_image();
}

FailWindow::~FailWindow()
{
    delete ui;
}

// SET QSS
void  FailWindow::Set_QSS()
{
    QFile file(":/QSS/myQss.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = tr(file.readAll());
    this->setStyleSheet(styleSheet);
    file.close();
}

void FailWindow::on_button_back_clicked()
{
    this->close();
    MainWindow *mw = new MainWindow();
    mw->show();
}

void FailWindow::Set_image()
{
    QPixmap *pix = new QPixmap(":/Image/blackMap.png");
    QSize sz = ui->label_image->size();
    ui->label_image->setPixmap(pix->scaled(sz));
}
