#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <hw/HwSink.h>
#include <QKeySequence>
#include <QMap>
#include <QVector>
#include <QToolBar>

class Axis;
class ChannelBase;
class ChannelMenu;
class ConnectivityLabel;
class GlobalSettingsDialog;
class Measurement;
class QAction;
class QDialog;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
class QShortcut;
struct Context;

class ButtonLine : public QToolBar
{
    Q_OBJECT

    void _InitializeMenu();
    QPoint _GetGlobalMenuPosition(QPushButton *button);
    void _OpenMenuDialog(QPushButton *button, QDialog &dialog);
    void _RefreshPanelMenu();
    void _ExportCSV(QVector<Measurement *> const & measurements);
    void _ClearPanelShortcuts();
    void _CreatePanelShortcuts();
    void _ActivateChannel(ChannelBase *channel, bool checked);
    QString _GetFileNameToSave(const QString &extension);
    void _SaveFile(const QString &fileName, bool values);
    void _OpenFile(bool values);
    void _SetConnectivityState(const QString &stateString, hw::HwSink::State state);

    QPushButton *m_startButton;
    QPushButton *m_sampleRequestButton;
    QAction *m_sampleRequestAction;
    QPushButton *m_pauseContinueButton;
    QAction *m_pauseContinueAction;
    QPushButton *m_stopButton;
    QPushButton *m_connectivityButton;
    QPushButton * m_fileMenuButton;
    QPushButton * m_panelMenuButton;
    QPushButton * m_axisMenuButton;
    QPushButton * m_measurementButton;
    QMenu *m_fileMenu;
    ChannelMenu *m_channelMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QMap<ChannelBase *, QAction*> m_channelActions;
    QAction *m_allAction;
    QAction *m_noneAction;
    QAction *m_afterLastChannelSeparator;
    Context const &m_context;
    Measurement *m_measurement;

    QShortcut *m_graphShortcut;
    QMap<QShortcut*, ChannelBase*> m_shortcutChannels;
    QShortcut *m_allChannelsShortcut;
    QShortcut *m_noChannelsShortcut;
    bool m_storedValues;
    GlobalSettingsDialog *m_settingsDialog;

public:
    void UpdateRunButtonsState();
    ButtonLine(QWidget *parent, const Context &context);
    void ChangeMeasurement(Measurement *measurement);
    QString GetGraphShortcutText();
    QString GetAllChannelShortcutText();
    QString GetNoChannelShortcutText();
    QString GetChannelShortcutText(ChannelBase *channel);

signals:
    void periodChanged(unsigned period);
    void channelTriggered(ChannelBase *channel, bool checked);
    void axesPressed();
    void allChannelsDisplayedOrHidden();

private slots:
    void newFile();
    void openWithoutValues();
    void openFile();
    void saveFile();
    void saveAsFile();
    void saveWithoutValuesAsFile();
    void sampleRequest();
    void settings();
public slots:
    void exportPng();
    void exportCsv();
    void exportAllCsv();
    void connectivityStateChanged(QString const & stateText, hw::HwSink::State state);
    void fileMenuButtonPressed();
    void panelMenuButtonPressed();
    void axisMenuButtonPressed();
    void measurementMenuButtonPressed();
    void measurementStateChanged();
    void channelActivated();
    void start();
    void pauseContinue();
    void stop();
};

#endif // BUTTONLINE_H
