#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>

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
    QByteArray *data;
    QByteArray *data2;
};
#endif // MAINWINDOW_H
