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
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QMap<ChannelBase *, QAction*> m_channelActions;
    QAction *m_allAction;
    QAction *m_noneAction;
    QAction *m_afterLastChannelSeparator;
    hw::HwSink &m_hwSink;
    Measurement *m_measurement;
    QWidget* m_space;
    QMap<QAction*, QString> m_recentFileActions;
    ChannelMenu *m_channelMenu;
public:
    ButtonLine(QWidget *parent, hw::HwSink &hwSink, Qt::Orientation orientation);
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
    void settings();
    void panelMenuButtonPressed(Measurement *);

private slots:
    void sampleRequest();
    void about();
    void openRecentFileSlot();
    void panelMenuButtonPressed();
public slots:
    void connectivityStateChanged(QString const & stateText, hw::HwSink::State state);
    void fileMenuButtonPressed();
    void viewMenuButtonPressed();
    void measurementStateChanged();
    void start();
    void pauseContinue();
    void stop();
    void updateRunButtonsState();
};

#endif // BUTTONLINE_H
