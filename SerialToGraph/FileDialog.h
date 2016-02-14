#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <bases/PlatformDialog.h>

class QString;

class FileDialog : public bases::PlatformDialog
{
public:
    FileDialog(QWidget *parent, bool open, const QString &extension);
    ~FileDialog();
};

#endif // FILEDIALOG_H
