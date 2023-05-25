#include "successwindow.h"
#include "ui_successwindow.h"
#include <QFile>

SuccessWindow::SuccessWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SuccessWindow)
{
    ui->setupUi(this);
    Set_QSS();
    Set_image();
}

SuccessWindow::~SuccessWindow()
{
    delete ui;
}

// SET QSS
void SuccessWindow::Set_QSS()
{
    QFile file(":/QSS/myQss.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = tr(file.readAll());
    this->setStyleSheet(styleSheet);
    file.close();
}

void SuccessWindow::on_button_back_clicked()
{
    this->close();
    MainWindow *mw = new MainWindow();
    mw->show();
}

void SuccessWindow::Set_image()
{
    QPixmap *pix = new QPixmap(":/Image/token.png");
    QSize sz = ui->label_token->size();
    ui->label_token->setPixmap(pix->scaled(sz));
}

void SuccessWindow::getData(QString time, QString rand)
{
    ui->label_time2->setText(time);
    ui->label_rand2->setText(rand);
}
