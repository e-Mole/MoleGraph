#include "Context.h"
#include <MainWindow.h>
#include <Measurement.h>


Context::Context(QVector<Measurement *> &measurements,
        MainWindow &mainWindow):
    m_measurements(measurements),
    m_mainWindow(mainWindow)
{
}
