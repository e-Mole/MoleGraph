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
class PlotContextMenu;
class QDialog;
class QGridLayout;
class QKeySequence;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
class QShortcut;
struct Context;

namespace hw { class HwSink; }

class ButtonLine : public QWidget
{
    Q_OBJECT

    void _InitializeMenu();
    QPoint _GetGlobalMenuPosition(QPushButton *button);
    void _ActivateChannel(ChannelBase *channel, bool checked);
    void _SetConnectivityState(const QString &stateString, hw::HwSink::State state);
    QString _GetRootDir();
    void _FillRecentFileMenu();
    void _SetMenuStyle(QMenu *menu);
    QShortcut * _CreateShortcut(QKeySequence const &sequence, const QObject *receiver, const char *slot);
    QKeySequence _GetKey(QShortcut *shortcut);
    void RefreshPanelMenu();

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
    QPushButton * m_viewButton;
    QMenu *m_fileMenu;
    PlotContextMenu *m_viewMenu;
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
    hw::HwSink &m_hwSink;
    Measurement *m_measurement;

    GlobalSettingsDialog *m_settingsDialog;
    QWidget* m_space;
    QMap<QAction*, QString> m_recentFileActions;

public:
    ButtonLine(QWidget *parent, const Context &context, hw::HwSink &hwSink, Qt::Orientation orientation);
    void ChangeMeasurement(Measurement *measurement);
    void ReplaceButtons(Qt::Orientation orientation);

signals:
    void periodChanged(unsigned period);
    void channelTriggered(ChannelBase *channel, bool checked);
    void axesPressed();
    void allChannelsDisplayedOrHidden();
    void measurementMenuButtonPressed();
    void openFileValues();
    void openFileNoValues();
    void openNewFile();
    void openRecentFile(QString const fileName);
    void connectivityButtonReleased();
    void saveFile();
    void saveAsFile();
    void saveWithoutValuesAsFile();
    void exportPng();
    void exportCsv();
    void exportAllCsv();
    void axisMenuButtonPressed();

private slots:
    void sampleRequest();
    void settings();
    void about();
    void openRecentFileSlot();
    void updateRunButtonsState();
public slots:
    void connectivityStateChanged(QString const & stateText, hw::HwSink::State state);
    void fileMenuButtonPressed();
    void panelMenuButtonPressed();
    void viewMenuButtonPressed();
    void measurementStateChanged();
    void start();
    void pauseContinue();
    void stop();
};

#endif // BUTTONLINE_H
