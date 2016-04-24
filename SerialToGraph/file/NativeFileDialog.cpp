#include "NativeFileDialog.h"

namespace file
{

NativeFileDialog::NativeFileDialog(
        AcceptMode mode,
        QWidget *parent,
        const QString &caption,
        const QString &directory,
        const QString &filter,
        const QString &limit):
        QFileDialog(parent, caption, directory, filter),
        m_limit(limit)
{
    setAcceptMode(mode);
    connect (this, SIGNAL(directoryEntered(QString)), this, SLOT(limitDirectory(QString)));
}

void NativeFileDialog::limitDirectory(QString const &dirName)
{
    if (!dirName.contains(m_limit))
        setDirectory(m_limit);
}
} //namespace file
