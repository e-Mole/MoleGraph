#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>
#include <QVector>

class Axis;
class CentralWidget;
class Channel;
class Graph;
class Measurement;
class QApplication;
class QScrollBar;
class MainWindow : public QMainWindow
{
	Q_OBJECT

    void _InitializeMeasurement();
    void _InitializeAxes(Axis *xAxis, Axis *yAxis);
    void _InitializeChannels(Axis *xAxis, Axis *yAxis);
    void _AddYChannel(Qt::GlobalColor color, Axis *axis);

    QSettings m_settings;
	SerialPort m_serialPort;
    CentralWidget *m_centralWidget;
    ButtonLine* m_buttonLine;

    QVector<Axis *> m_axes;
    QVector<Channel *> m_channels;
    QVector<Measurement*> m_measurements;
    QScrollBar *m_scrollBar;
    Graph* m_graph;
    Context m_context;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
};

#endif // MAINWINDOW_H
