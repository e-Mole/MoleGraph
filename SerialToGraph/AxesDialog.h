#ifndef REMOVEAXISDIALOG_H
#define REMOVEAXISDIALOG_H

#include <FormDialogBase.h>
#include <QVector>
#include <QMap>

class Axis;
class AxisCopy;
class QPushButton;
class AxesDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();
    void _ReinitAxes();

    QVector<Axis *> &m_axesOriginal;
    QVector<AxisCopy *> m_axesCopy;
    QMap<QPushButton*, AxisCopy*> m_removeButtontoAxis;
    QMap<QPushButton*, AxisCopy*> m_editButtontoAxis;
public:
    AxesDialog(QVector<Axis *> & axes);
    ~AxesDialog();

signals:

private slots:
    bool addButtonPressed();
    bool removeButtonPressed();
    bool editButtonPressed();
};

#endif // REMOVEAXISDIALOG_H
