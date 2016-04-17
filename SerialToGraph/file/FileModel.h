#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QSortFilterProxyModel>

class QFileSystemModel;
class QObject;

namespace file
{
class FileModel : public QSortFilterProxyModel
{
    QFileSystemModel *m_model;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
    FileModel(QObject *parent);
    void SetRootPath(QString const &path);
    void SetNameFilters(QStringList const &filters);
    QModelIndex Index(const QString &path) const;
    QModelIndex Mkdir(const QModelIndex &parent, const QString &name);
    QString FilePath(const QModelIndex &index) const;
    bool IsDir(const QModelIndex &index) const;
    QString FileName(const QModelIndex &aindex) const;
};

} //namespace file

#endif // FILEMODEL_H
