#ifndef FILEDIALOG_H
#define FILEDIALOG_H

class GlobalSettings;
class QString;
class QWidget;

namespace file
{

class FileDialog
{
public:
    static QString getOpenFileName(QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter,
        bool acceptChangesByDialogClosing,
        const QString &limitDir);

    static QString getSaveFileName(
        QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter,
        const GlobalSettings &settings
    );

    static QString getExistingDirectory(QWidget *parent,
        const QString &caption,
        const QString &dir,
        const GlobalSettings &settings
    );
};

} //namespace file

#endif // FILEDIALOG_H
