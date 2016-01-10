#ifndef AXISCHOOSEDIALOG_H
#define AXISCHOOSEDIALOG_H

#include <QDialog>
#include <QMap>
class Axis;
class Channel;
class QRadioButton;
class QString;
struct Context;
class AxisChooseDialog : public QDialog
{
    Q_OBJECT

    bool _IsEmptyAxis(Axis *axis);

    Context const& m_context;
    QRadioButton *m_newAxis;
    QMap<QRadioButton *, Axis*> m_axes;
    Channel *m_originalHChannel;
    Channel *m_newHChannel;
    bool m_isOriginalChannelRealTime;
public:
    AxisChooseDialog(
        Context const &context, Channel *originalHChannel, Channel *newHChannel);

signals:

public slots:
    void newAxisSelected();
    void axisSelected();

};

#endif // AXISCHOOSEDIALOG_H
