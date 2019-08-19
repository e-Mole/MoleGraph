#ifndef AXISCHOOSEDIALOG_H
#define AXISCHOOSEDIALOG_H

#include <QDialog>
#include <QMap>
class Axis;
class ChannelProxyBase;
class GraphicsContainer;
class RadioButton;
class QString;
class QWidget;
class AxisChooseDialog : public QDialog
{
    Q_OBJECT

    GraphicsContainer *m_graphicsContainer;
    ChannelProxyBase *m_originalHorizontalChannelProxy;
    ChannelProxyBase *m_newHorizontalChannelProxy;
    bool m_isOriginalChannelRealTime;
    RadioButton *m_newAxis;
    QMap<RadioButton *, Axis*> m_axes;


public:
    AxisChooseDialog(QWidget *parent, GraphicsContainer *graphicsContainer, ChannelProxyBase *originalHorizontalChannelProxy, ChannelProxyBase *newHorizontalChannelProxy);

signals:

public slots:
    void newAxisSelected();
    void axisSelected();

};

#endif // AXISCHOOSEDIALOG_H
