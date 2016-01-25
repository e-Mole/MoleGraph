#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QKeySequence>
#include <QMap>
#include <QVector>
#include <QToolBar>

class Axis;
class Channel;
class ChannelMenu;
class ConnectivityLabel;
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
    void _ClearShortcuts();
    void _CreateShortcuts();
    void _ActivateChannel(Channel *channel, bool checked);
    QString _GetFileNameToSave(const QString &extension);
    void _SaveFile(const QString &fileName);

    QPushButton *m_startButton;
    QPushButton *m_sampleRequestButton;
    QPushButton *m_stopButton;
    ConnectivityLabel *m_connectivityLabel;
    QPushButton * m_fileMenuButton;
    QPushButton * m_panelMenuButton;
    QPushButton * m_axisMenuButton;
    QPushButton * m_measurementButton;
    QMenu *m_fileMenu;
    ChannelMenu *m_channelMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QMap<Channel *, QAction*> m_channelActions;
    QAction *m_allAction;
    QAction *m_noneAction;
    QAction *m_afterLastChannelSeparator;
    Context const &m_context;
    Measurement *m_measurement;

    QShortcut *m_graphShortcut;
    QMap<QShortcut*, Channel*> m_shortcutChannels;
    QShortcut *m_allChannelsShortcut;
    QShortcut *m_noChannelsShortcut;

public:
    void UpdateRunButtonsState();
    ButtonLine(QWidget *parent, const Context &context);
    void ChangeMeasurement(Measurement *measurement);
    QString GetGraphShortcutText();
    QString GetAllChannelShortcutText();
    QString GetNoChannelShortcutText();
    QString GetChannelShortcutText(Channel *channel);

signals:
    void periodChanged(unsigned period);
    void channelTriggered(Channel *channel, bool checked);
    void axesPressed();
    void allChannelsDisplayedOrHidden();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void sampleRequest();
public slots:
    void exportPng();
    void exportCsv();
    void exportAllCsv();
    void connectivityStateChange(bool connected);
    void fileMenuButtonPressed();
    void panelMenuButtonPressed();
    void axisMenuButtonPressed();
    void measurementMenuButtonPressed();
    void measurementStateChanged();
    void channelActivated();
    void start();
    void stop();
};

#endif // BUTTONLINE_H
