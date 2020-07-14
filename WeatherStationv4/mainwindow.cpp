#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include <QSerialPort>
#include <QObject>
#include <QChartView>
#include <QTimer>
#include <stdlib.h>

struct value
{
    float temp;
    float hum;
} actual, min, max, mean, ptpa, variance, stddev, count, sum;


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->temperature->display("00.00");
    ui->humidity->display("00.00");
    arduino = new QSerialPort();

    for (auto serialPort : QSerialPortInfo::availablePorts()) {
        ui->portsComboBox->addItem(serialPort.portName());
    }

    ui->portsComboBox->model()->sort(0);

    //TIMER INITIALIZATION
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::realTimeStats);
    n_timer = new QElapsedTimer(); //czas ktory uplynal
    //INITIALIZATION OF A CONNECT ARDUINO FUNCTION WHEN THE BUTTON IS CLICKED
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectArduino);
    connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectArduino);
    connect(ui->test, &QPushButton::clicked, this, &MainWindow::notDependentOnTime);
    //CHARTS OF TEMP AND HUM

    m_series = new QLineSeries();
    n_series = new QLineSeries();
    l_series = new QLineSeries();


    max.temp = 0;
    min.temp = 50;
    max.hum = 0;
    min.hum = 95;
    sum.temp = 0;



    QChart* chart = new QChart();
    chart->legend()->hide();
    chart->createDefaultAxes();
    chart->addSeries(m_series);
    m_chartView = new QChartView(chart);
    ui->plot1->addWidget(m_chartView);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->chart()->createDefaultAxes();
    m_chartView->chart()->axes(Qt::Horizontal).back();
    m_chartView->chart()->axes(Qt::Vertical).back();
    m_chartView->chart()->axisY()->setRange(0, 60);

    QChart* m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->createDefaultAxes();
    m_chart->addSeries(n_series);
    n_chartView = new QChartView(m_chart);
    ui->plot2->addWidget(n_chartView);
    n_chartView->setRenderHint(QPainter::Antialiasing);
    n_chartView->chart()->createDefaultAxes();
    n_chartView->chart()->axes(Qt::Horizontal).back();
    n_chartView->chart()->axes(Qt::Vertical).back();
    n_chartView->chart()->axisY()->setRange(0, 100);
}



void MainWindow::connectArduino()
{
    if (!arduino->isOpen()) {
        arduino->setPortName(ui->portsComboBox->currentText());
        if (!arduino->open(QSerialPort::ReadWrite))

        {
            return;
        }

        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->write("temp\n");
        datax = arduino->readLine();
        arduino->write("hum\n");
        datay = arduino->readLine();


        m_timer->start(2000);
        n_timer->start();


    }
    else {
        arduino->close();
        m_timer->stop();
    }
}
void MainWindow::onTimeout()
{
    if (arduino->isWritable()) {

        int a = n_timer->elapsed();
        b = a / 1000;

        //TEMP
        arduino->write("temp\n");
        data = arduino->readLine();
        tempx = QString::fromStdString(data.toStdString());
        QString temp1 = tempx.trimmed();
        float temp2 = temp1.toFloat();
        actual.temp = temp2;
        //qDebug() << "Temperatura: " << temp1;
        ui->temperature->display(temp1);
        m_series->append(b, temp2);
        m_chartView->chart()->axisX()->setRange(0, b);
        count.temp = m_series->count();

        //HUM
        arduino->write("hum\n");
        QByteArray data2 = arduino->readLine();
        QString humx = QString::fromStdString(data2.toStdString());
        QString hum1 = humx.trimmed();
        float hum2 = hum1.toFloat();
        actual.hum = hum2;
        //qDebug() << "Wilgotnosc: " << hum1;
        ui->humidity->display(hum1);
        n_series->append(b, hum2);
        n_series->remove(b,0);
        n_chartView->chart()->axisX()->setRange(0, b);
        count.hum = n_series->count();

        //ui->maxLcd->display(max_temp);
        ui->maxLcdT->display(max.temp);
        ui->minLcdT->display(min.temp);
        ui->maxLcdH->display(max.hum);
        ui->minLcdH->display(min.hum);
        ui->meanTemp->display(mean.temp);
        ui->meanHum->display(mean.hum);
        //qDebug() << count.temp;
        //qDebug() << count.hum;

    }
}
void MainWindow::disconnectArduino()
{
    m_timer->stop();
    arduino->close();
    ui->temperature->display("00.00");
    ui->humidity->display("00.00");
}
void MainWindow::realTimeStats()
{

    //For the max&min temperature
    if(actual.temp > max.temp)
    {max.temp = actual.temp;}
    if(actual.temp < min.temp)
    {min.temp = actual.temp;}


    //For the max&min humidity
    if(actual.hum > max.hum)
    {max.hum = actual.hum;}
    if(actual.hum < min.hum)
    {min.hum = actual.hum;}

    //Create sum of temp and hum values and then find mean
    sum.temp += actual.temp;
    sum.hum += actual.hum;
    mean.temp = sum.temp/count.temp;
    mean.hum = sum.hum/count.hum;


}
void MainWindow::notDependentOnTime()
{
    qDebug()<<sum.temp;
    qDebug()<<sum.hum;
    qDebug()<<mean.temp;
    qDebug()<<mean.hum;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_timer;
    delete n_timer;
    delete chart;
    delete m_series;
    delete n_series;
    delete m_chart;
    delete m_chartView;
    delete n_chartView;
}
