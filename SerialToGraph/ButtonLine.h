#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <QWidget>
class QLineEdit;
class QPushButton;
class QLabel;
class ButtonLine : public QWidget
{
    Q_OBJECT

    QLineEdit *m_period;
    QLabel *m_periodUnits;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QLabel *m_connectivityLabel;

	bool m_connected;
	bool m_enabledBChannels;

public:
    explicit ButtonLine(QWidget *parent = 0);


signals:
    void periodTypeChanged(int type);
    void periodChanged(unsigned period);
    void start();
    void stop();
    void exportPng(QString const &fileName);
    void exportCsv(QString const &fileName);

public slots:
    void startButtonPressed();
    void stopButtonPressed();
    void periodLineEditChanged(QString const &text);
    void enableStartButton(bool enabled);
    void changePeriodUnits(int periodType);
    void exportPngSlot();
    void exportCsvSlot();
    void connectivityStateChange(bool connected);
};

#endif // BUTTONLINE_H
