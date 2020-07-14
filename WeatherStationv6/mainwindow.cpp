#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include <QSerialPort>
#include <QObject>
#include <QChartView>
#include <QTimer>
#include <stdlib.h>
#include <QtXml>

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
    connect(m_timer, &QTimer::timeout, this, &MainWindow::notDependentOnTime);
    n_timer = new QElapsedTimer(); //czas ktory uplynal
    //INITIALIZATION OF A CONNECT ARDUINO FUNCTION WHEN THE BUTTON IS CLICKED
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectArduino);
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::start);
    //connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::notDependentOnTime);
    connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectArduino);
    connect(ui->test, &QPushButton::clicked, this, &MainWindow::xml_file);
    connect(ui->test, &QPushButton::clicked, this, &MainWindow::end);
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

        ui->maxLcdT->display(max.temp);
        ui->minLcdT->display(min.temp);
        ui->maxLcdH->display(max.hum);
        ui->minLcdH->display(min.hum);
        ui->meanTemp->display(mean.temp);
        ui->meanHum->display(mean.hum);
        ui->ptpaT->display(ptpa.temp);
        ui->ptpaH->display(ptpa.hum);
        ui->varT->display(variance.temp);
        ui->varH->display(variance.hum);
        ui->stT->display(stddev.temp);
        ui->stH->display(stddev.hum);


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

    //Create sum of temp and hum values and then find mean, peak to peak
    sum.temp += actual.temp;
    sum.hum += actual.hum;
    mean.temp = sum.temp/count.temp;
    mean.hum = sum.hum/count.hum;
    ptpa.temp = max.temp - min.temp;
    ptpa.hum = max.hum - min.hum;

    //Variance, Standard Deviation
    variance.temp = ((actual.temp - mean.temp)*(actual.temp - mean.temp))/(count.temp);
    variance.hum = ((actual.hum - mean.hum)*(actual.hum - mean.hum))/(count.hum);
    stddev.temp = sqrt(variance.temp);
    stddev.hum = sqrt(variance.hum);
}
struct values
{
    QString temp;
    QString hum;
} mi, ma, mea, pt, va, st;

void MainWindow::xml_file()
{


    QDomProcessingInstruction createProcessingInstruction;
    QDomDocument doc("XML");
    QDomElement root = doc.createElement("Measurements");
    doc.appendChild(root);

    QDomElement temp = doc.createElement("Temperature");
    root.appendChild(temp);
    QDomElement startT = doc.createElement(startDateTime);
    temp.appendChild(startT);
    QDomElement endT = doc.createElement(endDateTime);
    temp.appendChild(endT);



    QDomElement tag1 = doc.createElement("Min");
    temp.appendChild(tag1);
    QDomText t1 = doc.createTextNode(mi.temp);
    tag1.appendChild(t1);

    QDomElement tag2 = doc.createElement("Max");
    temp.appendChild(tag2);
    QDomText t2 = doc.createTextNode(ma.temp);
    tag2.appendChild(t2);

    QDomElement tag3 = doc.createElement("Mean");
    temp.appendChild(tag3);
    QDomText t3 = doc.createTextNode(mea.temp);
    tag3.appendChild(t3);

    QDomElement tag4 = doc.createElement("Ptp");
    temp.appendChild(tag4);
    QDomText t4 = doc.createTextNode(pt.temp);
    tag4.appendChild(t4);

    QDomElement tag5 = doc.createElement("Var");
    temp.appendChild(tag5);
    QDomText t5 = doc.createTextNode(va.temp);
    tag5.appendChild(t5);

    QDomElement tag6 = doc.createElement("StD");
    temp.appendChild(tag6);
    QDomText t6 = doc.createTextNode(st.temp);
    tag6.appendChild(t6);


    QDomElement hum = doc.createElement("Humidity");
    root.appendChild(hum);
    QDomElement startH = doc.createElement(startDateTime);
    hum.appendChild(startH);
    QDomElement endH = doc.createElement(endDateTime);
    hum.appendChild(endH);


    QDomElement tag7 = doc.createElement("Min");
    hum.appendChild(tag7);
    QDomText t7 = doc.createTextNode(mi.hum);
    tag7.appendChild(t7);

    QDomElement tag8 = doc.createElement("Max");
    hum.appendChild(tag8);
    QDomText t8 = doc.createTextNode(ma.hum);
    tag8.appendChild(t8);

    QDomElement tag9 = doc.createElement("Mean");
    hum.appendChild(tag9);
    QDomText t9 = doc.createTextNode(mea.hum);
    tag9.appendChild(t9);

    QDomElement tag10 = doc.createElement("Ptp");
    hum.appendChild(tag10);
    QDomText t10 = doc.createTextNode(pt.hum);
    tag10.appendChild(t10);

    QDomElement tag11 = doc.createElement("Var");
    hum.appendChild(tag11);
    QDomText t11 = doc.createTextNode(va.hum);
    tag11.appendChild(t11);

    QDomElement tag12 = doc.createElement("StD");
    hum.appendChild(tag12);
    QDomText t12 = doc.createTextNode(st.hum);
    tag12.appendChild(t12);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "X:\Pulpit",
                               tr("XML files (*.xml)"));
    QFile file( fileName );
    if(file.open(QIODevice::ReadWrite))
    {
    QTextStream stream( &file );
    stream << doc.toString();
    }
    file.close();
}
void MainWindow::notDependentOnTime()
{

    std::string str1 = std::to_string(max.temp);
    ma.temp = QString::fromStdString(str1);
    std::string str2 = std::to_string(min.temp);
    mi.temp = QString::fromStdString(str2);
    std::string str3 = std::to_string(mean.temp);
    mea.temp = QString::fromStdString(str3);
    std::string str4 = std::to_string(ptpa.temp);
    pt.temp = QString::fromStdString(str4);
    std::string str5 = std::to_string(variance.temp);
    va.temp = QString::fromStdString(str5);
    std::string str6 = std::to_string(stddev.temp);
    st.temp = QString::fromStdString(str6);

    std::string str7 = std::to_string(max.hum);
    ma.hum = QString::fromStdString(str7);
    std::string str8 = std::to_string(min.hum);
    mi.hum = QString::fromStdString(str8);
    std::string str9 = std::to_string(mean.hum);
    mea.hum = QString::fromStdString(str9);
    std::string str10 = std::to_string(ptpa.hum);
    pt.hum = QString::fromStdString(str10);
    std::string str11 = std::to_string(variance.hum);
    va.hum = QString::fromStdString(str11);
    std::string str12 = std::to_string(stddev.hum);
    st.hum = QString::fromStdString(str12);
}
void MainWindow::start()
{
    startDateTime = QDateTime::currentDateTime().toString();

}
void MainWindow::end()
{
    endDateTime = QDateTime::currentDateTime().toString();
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
