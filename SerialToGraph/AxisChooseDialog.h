#ifndef AXISCHOOSEDIALOG_H
#define AXISCHOOSEDIALOG_H

#include <QDialog>
#include <QMap>
class Axis;
class Channel;
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
    Channel *m_originalHChannel;
    Channel *m_newHChannel;
    bool m_isOriginalChannelRealTime;
public:
    AxisChooseDialog(
        QWidget *parent, Context const &context, Channel *originalHChannel, Channel *newHChannel);

signals:

public slots:
    void newAxisSelected();
    void axisSelected();

};

#endif // AXISCHOOSEDIALOG_H
