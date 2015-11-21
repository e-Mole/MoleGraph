#ifndef REMOVEAXISDIALOG_H
#define REMOVEAXISDIALOG_H

#include <FormDialogBase.h>
#include <QVector>
#include <QMap>

class Axis;
class AxisCopy;
class QPushButton;
struct Context;
class AxesDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();
    void _ReinitAxes();

    QVector<AxisCopy *> m_axesCopy;
    QMap<QPushButton*, AxisCopy*> m_removeButtontoAxis;
    QMap<QPushButton*, AxisCopy*> m_editButtontoAxis;
    Context &m_context;
public:
    AxesDialog(Context &context);
    ~AxesDialog();

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // REMOVEAXISDIALOG_H
