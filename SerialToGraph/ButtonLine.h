#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QWidget>
#include <QMap>
#include <QVector>

class Axis;
class Channel;
class Measurement;
class QAction;
class QDialog;
class QKeySequence;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
struct Context;

class ButtonLine : public QWidget
{
    Q_OBJECT

    void _UpdateStartAndStopButtonsState();
    QAction * _InsertAction(QMenu *menu, QString title, const QKeySequence &keySequence, bool checkable, QAction *before = NULL);
    void _InitializeMenu();
    QPoint _GetGlobalMenuPosition(QPushButton *button);
    void _OpenMenuDialog(QPushButton *button, QDialog &dialog);

    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QLabel *m_connectivityLabel;
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

public:
    ButtonLine(QWidget *parent, const Context &context);
    void AddChannel(Channel *channel);
    void ChngeMeasurement(Measurement *measurement);
signals:
    void periodChanged(unsigned period);
    void start();
    void stop();
    void graphTriggered(bool checked);
    void channelTriggered(Channel *channel, bool checked);
    void axesPressed();
    void allChannelsDisplayedOrHidden();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
public slots:
    void exportPng();
    void exportCsv();
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
