#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>
#include <QVector>

class Axis;
class CentralWidget;
class Channel;
class Graph;
class QApplication;
class QScrollBar;
class MainWindow : public QMainWindow
{
	Q_OBJECT

    void _InitializeChannels(Axis *xAxis, Axis *yAxis);
    void _AddYChannel(Qt::GlobalColor color, Axis *axis);
    void _AddChannel(Channel *channel);

    QSettings m_settings;
	SerialPort m_serialPort;
    CentralWidget *m_centralWidget;
    ButtonLine* m_buttonLine;

    QVector<Axis *> m_axes;
    QVector<Channel *> m_channels;
    QScrollBar *m_scrollBar;
    Graph* m_graph;
    Context m_context;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
    void buttonLineLocationChanged(Qt::DockWidgetArea area);
    void dockVisibilityChanged(bool visible);
    void addChannelDisplay(Channel* channel);
    void openAxesDialog();
};

#endif // MAINWINDOW_H
