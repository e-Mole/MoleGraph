#ifndef OWNFILEDIALOG_H
#define OWNFILEDIALOG_H

#include <bases/PlatformDialog.h>
class GlobalSettings;
class Label;
class LineEdit;
class QListView;
class PushButton;
class QString;
class QWidget;

namespace file
{

class FileModel;
class OwnFileDialog : public bases::PlatformDialog
{
    Q_OBJECT
public:
    enum Type
    {
        Type_OpenFile,
        Type_SaveFile,
        Type_SelectDir
    };
private:
    QString _GetFilePath();
    QString _GetActionButtonText(Type type);
    void _CheckUpButton();
    void _ChangeDir(const QModelIndex &index);

    LineEdit *m_fileName;
    FileModel *m_model;
    QString m_dir;
    Label *m_extension;
    PushButton *m_actionButton;
    QListView *m_view;
    Type m_type;
    PushButton *m_upButton;
    bool m_acceptChangesByDialogClosing;
    QString const &m_limitDir;

public:
    OwnFileDialog(QWidget *parent,
        Type type,
        const QString &caption,
        const QString &dir,
        const QString &extension,
        bool acceptChangesByDialogClosing,
        const QString &limitDir
    );
    ~OwnFileDialog();

    static QString ExecuteFileDialog(Type type,
            QWidget *parent,
            const QString &caption,
            const QString &dir,
            const QString &filter,
            bool acceptChangesByDialogClosing, const QString &limitDir);

private slots:
    void viewClicked(const QModelIndex &index);
    void createFolder();
    void goUp();
    void fileNameChanged(QString const &fileName);
    void fileSelected();
};

} //namespace file

#endif // OWNFILEDIALOG_H
