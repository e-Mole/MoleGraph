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
class Measurement;
class QApplication;
class QTabWidget;
class MainWindow : public QMainWindow
{
	Q_OBJECT

    void _InitializeMeasurement();
    void _AddYChannel(Qt::GlobalColor color, Axis *axis);

    QSettings m_settings;
	SerialPort m_serialPort;
    ButtonLine* m_buttonLine;

    QVector<Axis *> m_axes;
    QVector<Channel *> m_channels;
    QVector<Measurement*> m_measurements;
    Context m_context;
    QTabWidget *m_measurmentTabs;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
};

#endif // MAINWINDOW_H
