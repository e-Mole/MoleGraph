#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QWidget>
#include <QMap>
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
    QPushButton * m_menuButton;
    QMenu *m_mainMenu;
    QMenu *m_viewMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QMap<Channel *, QAction*> m_channelActions;
    QAction *m_allAction;
    QAction *m_noneAction;

public:
    explicit ButtonLine(QWidget *parent = 0);
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
public slots:
    void startButtonPressed();
    void stopButtonPressed();
    void periodLineEditChanged(QString const &text);
    void changePeriodUnits(int periodType);
    void exportPngSlot();
    void exportCsvSlot();
    void connectivityStateChange(bool connected);
    void menuButtonPressed();
    void actionStateChanged();
    void channelSettingChanged();
};

#endif // BUTTONLINE_H
