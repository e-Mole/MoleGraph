#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QWidget>
#include <QVector>
class QLineEdit;
class QPushButton;
class QLabel;
class QMenu;
class QAction;
class QKeySequence;
class ButtonLine : public QWidget
{
    Q_OBJECT

    QAction * _AddAction(QMenu *menu, QString title, const QKeySequence &keySequence, bool checkable);
    void _InitializeMenu();

    QLineEdit *m_period;
    QLabel *m_periodUnits;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QLabel *m_connectivityLabel;
    QPushButton * m_menuButton;
    QMenu *m_mainMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QVector<QAction*> m_channels;
    QAction *m_allAction;
    QAction *m_noneAction;

public:
    explicit ButtonLine(QWidget *parent = 0);


signals:
    void periodTypeChanged(int type);
    void periodChanged(unsigned period);
    void start();
    void stop();
    void exportPng(QString const &fileName);
    void exportCsv(QString const &fileName);
    void graphTriggered(bool checked);
    void channelTriggered(unsigned index, bool checked);
public slots:
    void startButtonPressed();
    void stopButtonPressed();
    void periodLineEditChanged(QString const &text);
    void enableStartButton(bool enabled);
    void changePeriodUnits(int periodType);
    void exportPngSlot();
    void exportCsvSlot();
    void connectivityStateChange(bool connected);
    void menuButtonPressed();
    void actionStateChanged();
};

#endif // BUTTONLINE_H
