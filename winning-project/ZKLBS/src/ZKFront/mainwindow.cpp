#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Set_QSS();
    Set_image();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// SET QSS
void MainWindow::Set_QSS()
{
    QFile file(":/QSS/myQss.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = tr(file.readAll());
    this->setStyleSheet(styleSheet);
    file.close();
}

//SET IMAGE
void MainWindow::Set_image()
{
    QPixmap *pix = new QPixmap(":/Image/blackMap.png");
    QSize sz = ui->label_image->size();
    ui->label_image->setPixmap(pix->scaled(sz));
}

void MainWindow::Set_pk()
{
    RandomUtil randomUtil;
    std::string pk(randomUtil.generateRandomIntString(5));
    qpk = QString::fromStdString(pk);
}

void MainWindow::Set_coordinate()
{
    CoordinateUtil coordinateUtil;
    std::pair<long int, long int> coordinate = coordinateUtil.getCoordinate();
    qlat = QString::number(coordinate.first,10);
    qlon = QString::number(coordinate.second,10);
}

void MainWindow::on_button_submit_clicked()
{
    std::string pk = qpk.toStdString();
    std::string lat = qlat.toStdString();
    std::string lon = qlon.toStdString();
    GreeterClient greeterClient;
    // greeterClient.RunClient();
    std::vector<std::string> result = greeterClient.RunClient2(pk,lat,lon);

    std::cout << "Result: " << std::endl;
    for(int i = 0; i < result.size(); i++)
    {
        std::cout << result[i] << std::endl;
    }
    bool if_verify = true;
    if(result[0] == "RPC SUCCESS")
    {
        if_verify = true;
    }
    else
    {
        if_verify = false;
    }
    this->close();
    if(if_verify)
    {
        SuccessWindow *sw = new SuccessWindow();

        QString rand = QString::fromStdString(result[1]);
        QString time = QString::fromStdString(result[2]);
    
        connect(this,SIGNAL(sendData(QString,QString)),sw,SLOT(getData(QString,QString)));
        emit sendData(time, rand);

        sw->show();
    }
    else
    {
        FailWindow *fw = new FailWindow();
        fw->show();
    }
}

void MainWindow::on_button_get_clicked()
{
    Set_pk();
    Set_coordinate();
    ui->label_pk2->setText(qpk);
    ui->label_lat->setText(qlat);
    ui->label_lon->setText(qlon);
}
