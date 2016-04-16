#ifndef NATIVEFILEDIALOG_H
#define NATIVEFILEDIALOG_H

#include <QFileDialog>

class QString;
class NativeFileDialog : public QFileDialog
{
    Q_OBJECT

    QString m_limit;
public:
    NativeFileDialog(
        AcceptMode mode,
        QWidget *parent,
        const QString &caption,
        const QString &directory,
        const QString &filter,
        const QString &limit);

signals:

private slots:
    void limitDirectory(QString const &dirName);
};

#endif // NATIVEFILEDIALOG_H
