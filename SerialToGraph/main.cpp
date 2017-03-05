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

    parser.addPositionalArgument("file", "file to open");

    parser.process(a);

    QString fileName = parser.value(openOption);
    const QStringList arguments = parser.positionalArguments();
    if (arguments.size() > 0)
    {
        if (!arguments[0].contains(".exe")) //quick workaround for running from windows shortcut (it looks there is as first parameter used name of the .exe file)
            fileName = arguments[0];
    }
    MainWindow w(a, fileName, parser.isSet(withoutValuesOption));
    w.show();

	return a.exec();
}
