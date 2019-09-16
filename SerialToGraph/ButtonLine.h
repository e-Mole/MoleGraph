#ifndef BUTTONLINE_H
#define BUTTONLINE_H

#include <hw/HwSink.h>
#include <QKeySequence>
#include <QMap>
#include <QVector>
#include <QWidget>

class Axis;
class ConnectivityLabel;
class Measurement;
class PlotContextMenu;
class QDialog;
class QGridLayout;
class QKeySequence;
class Label;
class LineEdit;
class QPoint;
class PushButton;
class QShortcut;

namespace bases { class MenuBase;}
namespace hw { class HwSink; }

class ButtonLine : public QWidget
{
    Q_OBJECT

    void _InitializeMenu();
    QPoint _GetGlobalMenuPosition(PushButton *button);
    void _SetConnectivityState(const QString &stateString, hw::HwSink::State state);
    QString _GetRootDir();
    void _FillRecentFileMenu();
    QShortcut * _CreateShortcut(QKeySequence const &sequence, const QObject *receiver, const char *slot);
    QKeySequence _GetKey(QShortcut *shortcut);

    QGridLayout *m_mainLayout;
    PushButton *m_startButton;
    PushButton *m_sampleRequestButton;
    PushButton *m_pauseContinueButton;
    PushButton *m_stopButton;
    PushButton *m_connectivityButton;
    PushButton * m_fileMenuButton;
    PushButton * m_panelMenuButton;
    PushButton * m_axisMenuButton;
    PushButton * m_measurementButton;
    PushButton * m_viewButton;
    bases::MenuBase *m_fileMenu;
    PlotContextMenu *m_viewMenu;
    bases::MenuBase *m_recentFilesMenu;
	bool m_connected;
	bool m_enabledBChannels;

    QAction *m_graphAction;
    QAction *m_allAction;
    QAction *m_noneAction;
    QAction *m_afterLastChannelSeparator;
    hw::HwSink &m_hwSink;
    Measurement *m_measurement;
    QWidget* m_space;
    QMap<QObject*, QString> m_recentFileActions;
public:
    ButtonLine(QWidget *parent, hw::HwSink &hwSink, Qt::Orientation orientation);
    void ChangeMeasurement(Measurement *measurement);
    void ReplaceButtons(Qt::Orientation orientation);

signals:
    void periodChanged(unsigned period);
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
