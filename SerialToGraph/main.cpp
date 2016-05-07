#include "MainWindow.h"
#include <QApplication>
#include <QString>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(TARGET);
    a.setApplicationVersion("1");
#ifdef __linux__
    a.setWindowIcon(QIcon(":MoleGraph.png"));
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption openOption(QStringList() << "o" << "open-file",
                QCoreApplication::translate("main", "Open file."), QCoreApplication::translate("main", "directory"));
    parser.addOption(openOption);

    QCommandLineOption withoutValuesOption(QStringList() << "w" << "without-values",
                QCoreApplication::translate("main", "Modifier for opening file without values (just measurement template)."));
    parser.addOption(withoutValuesOption);

    parser.process(a);

    MainWindow w(a, parser.value(openOption), parser.isSet(withoutValuesOption));
    w.show();

	return a.exec();
}
