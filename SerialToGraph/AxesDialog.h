#ifndef REMOVEAXISDIALOG_H
#define REMOVEAXISDIALOG_H

#include <FormDialogBase.h>
#include <QVector>
#include <QMap>

class Axis;
class QPushButton;
class AxesDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();
    void _ReinitAxes();

    QVector<Axis *> &m_axesOriginal;
    QVector<Axis *> m_axesCopy;
    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
public:
    explicit AxesDialog(QVector<Axis *> & axes);

signals:

private slots:
    bool addButtonPressed();
    bool removeButtonPressed();
    bool editButtonPressed();
};

#endif // REMOVEAXISDIALOG_H
