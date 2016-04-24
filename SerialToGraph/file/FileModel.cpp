#include "FileModel.h"
#include <QObject>
#include <QFileSystemModel>
namespace file
{
FileModel::FileModel(QObject *parent):
    QSortFilterProxyModel(parent),
    m_model(new QFileSystemModel(this))
{
    setDynamicSortFilter(true);
    setSourceModel(m_model);

    m_model->setNameFilterDisables(false);
}

void FileModel::SetRootPath(QString const &path)
{
    m_model->setRootPath(path);
}

void FileModel::SetNameFilters(QStringList const &filters)
{
    m_model->setNameFilters(filters);
}

QModelIndex FileModel::Index(const QString &path) const
{
    return mapFromSource(m_model->index(path));
}

QModelIndex FileModel::Mkdir(const QModelIndex &parent, const QString &name)
{
    return mapFromSource(m_model->mkdir(parent, name));
}

QString FileModel::FilePath(const QModelIndex &index) const
{
    return m_model->filePath(mapToSource(index));
}

bool FileModel::IsDir(const QModelIndex &index) const
{
    return m_model->isDir(mapToSource(index));
}

QString FileModel::FileName(const QModelIndex &index) const
{
    return m_model->fileName(mapToSource(index));
}

bool FileModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QFileInfo leftFileInfo  = m_model->fileInfo(left);
    QFileInfo rightFileInfo = m_model->fileInfo(right);

    if (!leftFileInfo.isDir() && rightFileInfo.isDir())
        return false;

    else if (leftFileInfo.isDir() && !rightFileInfo.isDir())
        return true;

    return QSortFilterProxyModel::lessThan(left, right);
}

} //namespace file
