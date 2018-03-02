#include "FileDialog.h"
#include <file/NativeFileDialog.h>
#include <file/OwnFileDialog.h>
#include <QFileDialog>
#include <GlobalSettings.h>
#include <QString>

namespace file
{

#if defined Q_OS_ANDROID
#   define OWN_FILE_DIALOG
#endif

QString FileDialog::getOpenFileName(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    const QString &filter,
    bool acceptChangesByDialogClosing,
    const QString &limitDir
)
{
#if defined(OWN_FILE_DIALOG)
    return OwnFileDialog::ExecuteFileDialog(
        OwnFileDialog::Type_OpenFile, parent, caption, dir, filter, acceptChangesByDialogClosing, limitDir);
#else
    Q_UNUSED(acceptChangesByDialogClosing);
    NativeFileDialog nfd(QFileDialog::AcceptOpen, parent, caption, dir, filter, limitDir);
    return 0 == nfd.exec() ? "" : nfd.selectedFiles()[0];
#endif
}

QString FileDialog::getSaveFileName(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    const QString &filter,
    bool acceptChangesByDialogClosing,
    const QString &limitDir)
{
#if defined(OWN_FILE_DIALOG)
    return OwnFileDialog::ExecuteFileDialog(
        OwnFileDialog::Type_SaveFile, parent, caption, dir, filter, acceptChangesByDialogClosing, limitDir);
#else
    Q_UNUSED(acceptChangesByDialogClosing);
    Q_UNUSED(limitDir);
    NativeFileDialog nfd(QFileDialog::AcceptSave, parent, caption, dir, filter, GlobalSettings::GetInstance().GetLimitDir());
    return 0 == nfd.exec() ? "" : nfd.selectedFiles()[0];
#endif
}

QString FileDialog::getExistingDirectory(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    bool acceptChangesByDialogClosing,
    const QString &limitDir
)
{
#if defined(OWN_FILE_DIALOG)
    return OwnFileDialog::ExecuteFileDialog(
        OwnFileDialog::Type_SelectDir, parent, caption, dir, "", acceptChangesByDialogClosing, limitDir);
#else
    Q_UNUSED(acceptChangesByDialogClosing);
    Q_UNUSED(limitDir);
    return QFileDialog::getExistingDirectory(parent, caption, dir);
#endif
}

} //namespace file
