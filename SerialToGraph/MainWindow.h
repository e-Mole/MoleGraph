#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
#include <hw/HwSink.h>
#include <QMainWindow>
#include <QString>
#include <QVector>

class CentralWidget;
class ChannelMenu;
class ChannelSettings;
class ChannelWidget;
class Console;
class GraphicsContainer;
class GraphicsContainerManager;
class Measurement;
class MeasurementMenu;
class PortListDialog;
class QApplication;
class QDataStream;
class QGridLayout;
class QTabWidget;
namespace hw { class SensorManager; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

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
    QGridLayout *m_mainLayout;
    QPushButton *m_menuButton;
    QWidget *m_centralWidget;
    bool m_storedValues;
    GraphicsContainerManager *m_graphicsContainerManager;
    MeasurementMenu *m_measurementMenu;
    ChannelMenu *m_channelMenu;
    hw::SensorManager *m_sensorManager;
    bool m_ghostCreating;

    void _SetCurrentFileName(QString const &fileName);
    void _WriteUnsupportedFileVersion();
    void keyReleaseEvent(QKeyEvent * event);
    void closeEvent(QCloseEvent *event);
    bool _RealyExit();
    QString _MessageIfUnsaved();
    QString _GetRootDir();
    void _SaveFile(const QString &fileName, bool values);
    void _OpenFile(bool values);
    void _OpenFile(QString const &filePath, bool values);
    bool _CouldBeOpen();
    QString _GetFileNameToSave(const QString &extension, bool values);
    void _ExportCSV(std::vector<GraphicsContainer *> &graphicsContainers);
    QString _DisplayMeasurementRemoveMessage(Measurement *m, bool isInProgress, bool alreadyMeasured, bool haveGhosts);
    void _SerializeGhsotColections(QDataStream &out);
    bool _DeSerializeGhsotColections(QDataStream &in);
    void _ShowCoruptedFileMessage(const QString &fileName);
    unsigned _GetGhostCount();
    void _DisconnectChannelSettings(ChannelSettings *settings);

public:
    MainWindow(QApplication const &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent = 0);
    ~MainWindow();

    Measurement *CreateNewMeasurement(bool initializeAxesAndChannels);
    Measurement *CloneCurrentMeasurement();
    void ConfirmMeasurement(Measurement *m);
    void SwichCurrentMeasurement(Measurement *m);
    void RemoveAllMeasurements();
    void RemoveMeasurement(Measurement *m, bool confirmed);
    Measurement * GetCurrnetMeasurement();
    void DeserializeMeasurements(QString const &fileName, bool values);
    void SerializeMeasurements(const QString &fileName, bool values);
    QString &GetCurrentFileNameWithPath();
    void RefreshHwConnection();
    void ShowConsole(bool show);
    void TerminateBluetooth();
    bool GetSavedValues() { return m_savedValues; }
    void ReplaceWidgets(Qt::Orientation menuOrientation, bool showMenu);
    void ShowMenuButton(bool show);

private slots:
    void measurementNameChanged();
    void currentMeasurementChanged(int index);
    void measurementColorChanged();
    void menuButtonClicked();
    void measurementMenuButtonPressed();
    void updateWindowTitle();
    void openNewFile();
    void openFileValues();
    void openFileNoValues();
    void openRecentFile(QString const fileName);
    void saveFile();
    void saveAsFile();
    void saveWithoutValuesAsFile();
    void exportPng();
    void exportCsv();
    void exportAllCsv();
    void axisMenuButtonPressed();
    void settings();
    void openSerialPort();
    void removeMeasurement(Measurement *m);
    void addGhostChannel();
    void showPanelMenu(Measurement *m);
    void editChannel(GraphicsContainer* gc, ChannelWidget *channelWidget);
    void channelEditingAccepted();
    void channelEditingRejected();
};

#endif // MAINWINDOW_H
