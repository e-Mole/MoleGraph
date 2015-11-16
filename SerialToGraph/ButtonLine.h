#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QWidget>
#include <QMap>
#include <QVector>

class Axis;
class QLineEdit;
class QPushButton;
class QLabel;
class QMenu;
class QAction;
class QKeySequence;
class Channel;
class ButtonLine : public QWidget
{
    Q_OBJECT

    void _EnableStartButton(bool enabled);
    QAction * _InsertAction(QMenu *menu, QString title, const QKeySequence &keySequence, bool checkable, QAction *before = NULL);
    void _InitializeMenu();

    QLineEdit *m_period;
    QLabel *m_periodUnits;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QLabel *m_connectivityLabel;
    QPushButton * m_fileMenuButton;
    QPushButton * m_panelMenuButton;
    QPushButton * m_graphMenuButton;
    QMenu *m_fileMenu;
    QMenu *m_panelMenu;
    QMenu *m_axisMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QMap<Channel *, QAction*> m_channelActions;
    QAction *m_allAction;
    QAction *m_noneAction;
    QAction *m_afterLastChannelSeparator;
    QVector<Axis *> &m_axes;

public:
    ButtonLine(QWidget *parent, QVector<Axis *> &axes);
    void AddChannel(Channel *channel);

signals:
    void periodTypeChanged(int type);
    void periodChanged(unsigned period);
    void start();
    void stop();
    void exportPng(QString const &fileName);
    void exportCsv(QString const &fileName);
    void graphTriggered(bool checked);
    void channelTriggered(Channel *channel, bool checked);
    void axesPressed();

public slots:
    void startButtonPressed();
    void stopButtonPressed();
    void periodLineEditChanged(QString const &text);
    void changePeriodUnits(int periodType);
    void exportPngSlot();
    void exportCsvSlot();
    void connectivityStateChange(bool connected);
    void fileMenuButtonPressed();
    void panelMenuButtonPressed();
    void graphMenuButtonPressed();
    void actionStateChanged();
    void channelSettingChanged();
};

#endif // BUTTONLINE_H
