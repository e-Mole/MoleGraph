#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
#include <hw/HwSink.h>
#include <QMainWindow>
#include <GlobalSettings.h>
#include <QString>
#include <QVector>

class CentralWidget;
class Console;
class Measurement;
class PortListDialog;
class QApplication;
class QDataStream;
class QGridLayout;
class QTabWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

    void _SetCurrentFileName(QString const &fileName);
    void _WriteUnsupportedFileVersion();
    void keyReleaseEvent(QKeyEvent * event);
    void closeEvent(QCloseEvent *event);
    void _UpdateWindowTitle();
    bool _RealyExit();
    QString _MessageIfUnsaved();

    GlobalSettings m_settings;
    hw::HwSink m_hwSink;
    ButtonLine* m_buttonLine;
    QVector<Measurement*> m_measurements;
    Context m_context;
    QTabWidget *m_measurementTabs;
    Measurement *m_currentMeasurement;
    QString m_currentFileName;
    QString m_currentFileNameWithPath;
    QString m_langBcp47;
    PortListDialog *m_portListDialog;
    Console *m_console;
    bool m_savedValues;
    bool m_savedState;
    QGridLayout *m_mainLayout;
    QPushButton *m_menuButton;
    QWidget *m_centralWidget;
public:
    MainWindow(QApplication const &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent = 0);
    ~MainWindow();

    Measurement *CreateNewMeasurement(bool initializeAxesandChannels);
    Measurement *CloneCurrentMeasurement();
    void ConfirmMeasurement(Measurement *m);
    void SwichCurrentMeasurement(Measurement *m);
    void RemoveAllMeasurements();
    void RemoveMeasurement(Measurement *m, bool confirmed);
    Measurement * GetCurrnetMeasurement();
    void DeserializeMeasurements(QString const &fileName, bool values);
    void SerializeMeasurements(const QString &fileName, bool values);
    QString &GetCurrentFileNameWithPath();
    void OpenNew();
    void RefreshHwConnection();
    void ShowConsole(bool show);
    void TerminateBluetooth();
    void SetSavedState(bool savedState);
    void SetSavedValues(bool savedValues);
    bool GetSavedValues() { return m_savedValues; }
    void ReplaceWidgets(Qt::Orientation menuOrientation, bool showMenu);
    bool CouldBeOpen();
    void ShowMenuButton(bool show);
private slots:
    void measurementNameChanged();
    void currentMeasurementChanged(int index);
    void measurementColorChanged();
    void menuButtonClicked();
public slots:
    void openSerialPort();
};

#endif // MAINWINDOW_H
