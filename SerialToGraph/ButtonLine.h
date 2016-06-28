#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <hw/HwSink.h>
#include <QKeySequence>
#include <QMap>
#include <QVector>
#include <QWidget>

class Axis;
class ChannelBase;
class ChannelMenu;
class ConnectivityLabel;
class GlobalSettingsDialog;
class Measurement;
//class QAction;
class QDialog;
class QGridLayout;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
class QShortcut;
struct Context;

class ButtonLine : public QWidget
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
    QString _GetFileNameToSave(const QString &extension, bool values);
    void _SaveFile(const QString &fileName, bool values);
    void _OpenFile(QString const &filePath, bool values);
    void _OpenFile(bool values);
    void _SetConnectivityState(const QString &stateString, hw::HwSink::State state);
    QString _GetRootDir();
    void _FillRecentFileMenu();
    void _SetMenuStyle(QMenu *menu);

    QGridLayout *m_mainLayout;
    QPushButton *m_startButton;
    QPushButton *m_sampleRequestButton;
    //QAction *m_sampleRequestAction;
    QPushButton *m_pauseContinueButton;
    //QAction *m_pauseContinueAction;
    QPushButton *m_stopButton;
    QPushButton *m_connectivityButton;
    QPushButton * m_fileMenuButton;
    QPushButton * m_panelMenuButton;
    QPushButton * m_axisMenuButton;
    QPushButton * m_measurementButton;
    QMenu *m_fileMenu;
    QMenu *m_recentFilesMenu;
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
    QWidget* m_space;
    QMap<QAction*, QString> m_recentFileActions;

public:
    void UpdateRunButtonsState();
    ButtonLine(QWidget *parent, const Context &context, Qt::Orientation orientation);
    void ChangeMeasurement(Measurement *measurement);
    QString GetGraphShortcutText();
    QString GetAllChannelShortcutText();
    QString GetNoChannelShortcutText();
    QString GetChannelShortcutText(ChannelBase *channel);
    void ReplaceButtons(Qt::Orientation orientation);

signals:
    void periodChanged(unsigned period);
    void channelTriggered(ChannelBase *channel, bool checked);
    void axesPressed();
    void allChannelsDisplayedOrHidden();

private slots:
    void newFile();
    void openWithoutValues();
    void openFile();
    void openRecentFile();
    void saveFile();
    void saveAsFile();
    void saveWithoutValuesAsFile();
    void sampleRequest();
    void settings();
    void about();
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
