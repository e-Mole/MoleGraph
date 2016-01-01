#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QKeySequence>
#include <QMap>
#include <QVector>
#include <QWidget>

class Axis;
class Channel;
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

class ButtonLine : public QWidget
{
    Q_OBJECT

    void _UpdateStartAndStopButtonsState();
    QAction * _InsertAction(
        QMenu *menu,
        QString title,
        const QKeySequence &keySequence,
        bool checkable,
        bool checked,
        QAction *before = NULL);
    void _InitializeMenu();
    QPoint _GetGlobalMenuPosition(QPushButton *button);
    void _OpenMenuDialog(QPushButton *button, QDialog &dialog);
    void _RefreshPanelMenu();
    void _ExportCSV(QVector<Measurement *> const & measurements);

    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_cloneButton;
    ConnectivityLabel *m_connectivityLabel;
    QPushButton * m_fileMenuButton;
    QPushButton * m_panelMenuButton;
    QPushButton * m_axisMenuButton;
    QPushButton * m_measurementButton;
    QMenu *m_fileMenu;
    QMenu *m_panelMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QMap<Channel *, QAction*> m_channelActions;
    QAction *m_allAction;
    QAction *m_noneAction;
    QAction *m_afterLastChannelSeparator;
    Context const &m_context;
    Measurement *m_measurement;
    QMap<QKeySequence, QShortcut*> m_shortcuts;

public:
    ButtonLine(QWidget *parent, const Context &context);
    void AddChannel(Channel *channel, QMenu *panelMenu);
    void ChngeMeasurement(Measurement *measurement);
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
    void start();
    void stop();
public slots:
    void exportPng();
    void exportCsv();
    void exportAllCsv();
    void connectivityStateChange(bool connected);
    void fileMenuButtonPressed();
    void panelMenuButtonPressed();
    void axisMenuButtonPressed();
    void measurementMenuButtonPressed();
    void actionStateChanged();
    void channelSettingChanged();
    void measurementStateChanged();
};

#endif // BUTTONLINE_H
