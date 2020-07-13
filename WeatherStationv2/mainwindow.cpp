#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include <QSerialPort>
#include <QObject>
#include <QChartView>
#include <QTimer>

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
    n_timer = new QElapsedTimer(); //czas ktory uplynal
    //INITIALIZATION OF A CONNECT ARDUINO FUNCTION WHEN THE BUTTON IS CLICKED
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectArduino);
    connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectArduino);
    //CHARTS OF TEMP AND HUM

    m_series = new QLineSeries();
    n_series = new QLineSeries();
    l_series = new QLineSeries();
    n_timer->start();

    //data = new QByteArray();
    //data2 = new QByteArray();

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
    m_chartView->chart()->axisY()->setRange(0, 100);

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

void MainWindow::disconnectArduino()
{
    m_timer->stop();
    arduino->close();
    ui->temperature->display("00.00");
    ui->humidity->display("00.00");

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

        m_timer->start(2000);
    }
    else {
        arduino->close();
        m_timer->stop();
    }
}
void MainWindow::onTimeout()
{

    if (arduino->isWritable()) {

        arduino->write("temp\n");
        QByteArray data = arduino->readLine();
        QString tempx = QString::fromStdString(data.toStdString());
        QString temp1 = tempx.trimmed();
        float temp2 = temp1.toFloat();
        //qDebug() << "Temperatura: " << temp1;
        ui->temperature->display(temp1);
        int a = n_timer->elapsed();
        int b = a / 1000;
        m_series->append(b, temp2);
        m_series->remove(b,0);
        m_chartView->chart()->axisX()->setRange(5, b);

        arduino->write("hum\n");
        QByteArray data2 = arduino->readLine();
        QString humx = QString::fromStdString(data2.toStdString());
        QString hum1 = humx.trimmed();
        float hum2 = hum1.toFloat();
        //qDebug() << "Wilgotnosc: " << hum1;
        ui->humidity->display(hum1);
        n_series->append(b, hum2);
        n_series->remove(b,0);
        n_chartView->chart()->axisX()->setRange(5, b);

        arduino->write("maxTemp\n");
        QByteArray maxTemp = arduino->readLine();
        QString maxTemp1 = QString::fromStdString(maxTemp.toStdString());
        QString maxTemp2 = maxTemp1.trimmed();
        float max = maxTemp2.toFloat();
        qDebug() << max;
        arduino->write("minTemp\n");
        QByteArray minTemp = arduino->readLine();
        QString minTemp1 = QString::fromStdString(minTemp.toStdString());
        QString minTemp2 = minTemp1.trimmed();
        float min = minTemp2.toFloat();
        qDebug() << min;
        ui->maxLcd->display(max);
        ui->minLcd->display(min);
    }
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
