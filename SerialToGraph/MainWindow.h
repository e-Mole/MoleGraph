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
class QDataStream;
class QTabWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

    void _SetCurrentFileName(QString const &fileName);

    QSettings m_settings;
    SerialPort m_serialPort;
    ButtonLine* m_buttonLine;
    QVector<Measurement*> m_measurements;
    Context m_context;
    QTabWidget *m_measurementTabs;
    Measurement *m_currentMeasurement;
    QString m_currentFileName;

public:
    MainWindow(QApplication const &application, QString fileNameToOpen, QWidget *parent = 0);
    ~MainWindow();

    Measurement *CreateNewMeasurement(bool initializeAxesandChannels);
    Measurement *CloneCurrentMeasurement();
    void ConfirmMeasurement(Measurement *m);
    void SwichCurrentMeasurement(Measurement *m);
    void RemoveAllmeasurements();
    void RemoveMeasurement(Measurement *m, bool confirmed);
    Measurement * GetCurrnetMeasurement();
    bool OpenSerialPort();
    void DeserializeMeasurements(QString const &fileName);
    void SerializeMeasurements(const QString &fileName);
    QString &GetCurrentFileName();

    bool m_close;
private slots:
    void measurementNameChanged();
    void currentMeasurementChanged(int index);
};

#endif // MAINWINDOW_H
