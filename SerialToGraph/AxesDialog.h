#ifndef REMOVEAXISDIALOG_H
#define REMOVEAXISDIALOG_H

#include <QDialog>
#include <QMap>

class Axis;
class MyCustomPlot;
class QFormLayout;
class QPushButton;
struct Context;
class AxesDialog : public QDialog
{
    Q_OBJECT

    void _ReinitAxisGrid();

    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
    Context const &m_context;
    MyCustomPlot &m_plot;
    QFormLayout *m_formLayout;
public:
    AxesDialog(Context const &context);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // REMOVEAXISDIALOG_H
