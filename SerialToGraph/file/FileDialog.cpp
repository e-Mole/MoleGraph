#include "FileDialog.h"
#include <file/NativeFileDialog.h>
#include <file/OwnFileDialog.h>
#include <QFileDialog>
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
    const QString &limit
)
{
#if defined(OWN_FILE_DIALOG)
    return OwnFileDialog::ExecuteFileDialog(
        OwnFileDialog::Type_OpenFile, parent, caption, dir, filter, limit);
#else

    NativeFileDialog nfd(QFileDialog::AcceptOpen, parent, caption, dir, filter, limit);
    return 0 == nfd.exec() ? "" : nfd.selectedFiles()[0];
#endif
}

QString FileDialog::getSaveFileName(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    const QString &filter,
    const QString &limit)
{
#if defined(OWN_FILE_DIALOG)
    return OwnFileDialog::ExecuteFileDialog(
        OwnFileDialog::Type_SaveFile, parent, caption, dir, filter, limit);
#else
    NativeFileDialog nfd(QFileDialog::AcceptSave, parent, caption, dir, filter, limit);
    return 0 == nfd.exec() ? "" : nfd.selectedFiles()[0];
#endif
}

QString FileDialog::getExistingDirectory(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    const QString &limit
)
{
#if defined(OWN_FILE_DIALOG)
    return OwnFileDialog::ExecuteFileDialog(
        OwnFileDialog::Type_SelectDir, parent, caption, dir, "", limit);
#else
    Q_UNUSED(limit); //TODO: limit is not implemeted for common file dialog
    return QFileDialog::getExistingDirectory(parent, caption, dir);
#endif
}

} //namespace file
