#ifndef AXISCHOOSEDIALOG_H
#define AXISCHOOSEDIALOG_H

#include <QDialog>
#include <QMap>
class Axis;
class ChannelBase;
class QRadioButton;
class QString;
class QWidget;
struct Context;
class AxisChooseDialog : public QDialog
{
    Q_OBJECT

    Context const& m_context;
    QRadioButton *m_newAxis;
    QMap<QRadioButton *, Axis*> m_axes;
    ChannelBase *m_originalHChannel;
    ChannelBase *m_newHChannel;
    bool m_isOriginalChannelRealTime;
public:
    AxisChooseDialog(QWidget *parent, Context const &context, ChannelBase *originalHChannel, ChannelBase *newHChannel);

signals:

public slots:
    void newAxisSelected();
    void axisSelected();

};

#endif // AXISCHOOSEDIALOG_H
