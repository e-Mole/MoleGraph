#ifndef AXISCHOOSEDIALOG_H
#define AXISCHOOSEDIALOG_H

#include <QDialog>
#include <QMap>
class Axis;
class ChannelWidget;
class GraphicsContainer;
class QRadioButton;
class QString;
class QWidget;
class AxisChooseDialog : public QDialog
{
    Q_OBJECT

    GraphicsContainer *m_graphicsContainer;
    ChannelWidget *m_originalHChannelWidget;
    ChannelWidget *m_newHChannelWidget;
    bool m_isOriginalChannelRealTime;
    QRadioButton *m_newAxis;
    QMap<QRadioButton *, Axis*> m_axes;


public:
    AxisChooseDialog(QWidget *parent, GraphicsContainer *graphicsContainer, ChannelWidget *originalHChannelWidget, ChannelWidget *newHChannelWidget);

signals:

public slots:
    void newAxisSelected();
    void axisSelected();

};

#endif // AXISCHOOSEDIALOG_H
