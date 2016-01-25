#include "MainWindow.h"
#include <QApplication>
#include <QString>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString fileNameToOpen;
    if (argc > 1)
        fileNameToOpen = argv[1];
    MainWindow w(a, fileNameToOpen);
    if (w.m_close)
        return 0;

    w.show();

	return a.exec();
}
