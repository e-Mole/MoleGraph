#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>
#include <QVector>

class CentralWidget;
class Measurement;
class QApplication;
class QTabWidget;
class MainWindow : public QMainWindow
{
	Q_OBJECT

    void _InitializeMeasurement();

    QSettings m_settings;
	SerialPort m_serialPort;
    ButtonLine* m_buttonLine;

    QVector<Measurement*> m_measurements;
    Context m_context;
    QTabWidget *m_measurementTabs;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
private slots:
    void measurementNameChanged();
};

#endif // MAINWINDOW_H
