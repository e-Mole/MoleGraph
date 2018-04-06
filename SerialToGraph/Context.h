#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class MainWindow;
class Measurement;

struct Context
{
public:
    Context(QVector<Measurement *> &measurements,
        MainWindow &mainWindow);

    QVector<Measurement *> &m_measurements;
    MainWindow &m_mainWindow;

    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
