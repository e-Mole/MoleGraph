#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <QDialog>
#include <QMap>

class Measurement;
class QFormLayout;
class QPushButton;
class MeasurementMenu : public QDialog
{
    Q_OBJECT

    void _AddRowWithEditAndRemove(Measurement *measurement);
    void _ReinitGrid();

    QFormLayout *m_formLayout;
    QVector<Measurement*> &m_measurements;
    QMap<QPushButton*, Measurement*> m_removeButtonToIten;
    QMap<QPushButton*, Measurement*> m_editButtonToItem;
public:
    MeasurementMenu(QVector<Measurement *> &measurements);
    static QString GetNextMeasurementName(QVector<Measurement *> &measurements);
signals:

private slots:
    void addButtonPressed();
    void editButtonPressed();
    void removeButtonPressed();
};

#endif // MEASUREMENTMENU_H
