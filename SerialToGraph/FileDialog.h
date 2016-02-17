#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <bases/PlatformDialog.h>

class QFileSystemModel;
class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
class QString;
class QWidget;
class FileDialog : public bases::PlatformDialog
{
    Q_OBJECT

    QString _GetFilePath();

    static QString _ExecuteFileDialog(
            QWidget *parent,
            const QString &caption,
            const QString &dir,
            const QString &filter,
            bool open);

    QLineEdit *m_fileName;
    QFileSystemModel *m_model;
    QString m_dir;
    QLabel *m_extension;
    QPushButton *m_actionButton;
    QListView *m_view;
public:
    FileDialog(QWidget *parent, bool open, const QString &caption, const QString &dir, const QString &extension);
    ~FileDialog();

    static QString getOpenFileName(
        QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter
    );

    static QString getSaveFileName(
        QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter);

private slots:
    void viewClicked(const QModelIndex &index);
    void createFolder();
    void goUp();
    void fileNameChanged(QString const &fileName);
    void fileSelected();
};

#endif // FILEDIALOG_H
