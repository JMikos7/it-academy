#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QXmlStreamReader>
#include "dialog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectArduino();
    void onTimeout();
    void disconnectArduino();
    void realTimeStats();
    void notDependentOnTime();
    void xml_file();
    void start();
    void end();
    //void openDialog();
    //void xml_read();

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QTimer *m_timer;
    QLineSeries *m_series;
    QLineSeries *n_series;
    QElapsedTimer *n_timer;
    QChart *chart;
    QChart *m_chart;
    QChartView *m_chartView;
    QChartView *n_chartView;
    QLineSeries *l_series;
    QByteArray data;
    QByteArray datax;
    QByteArray datay;
    QString tempx;
    QString startDateTime;
    QString endDateTime;
    int b;
    //Dialog *SWindow;
    QString xmldata;

    //Dialog Dialog;

};

#endif // MAINWINDOW_H
