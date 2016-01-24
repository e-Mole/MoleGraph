#include "MainWindow.h"
#include <QApplication>
#include <AxisQml.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(a);
    if (w.m_close)
        return 0;

    w.show();

	return a.exec();
}
