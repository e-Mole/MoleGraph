#ifndef OWNFILEDIALOG_H
#define OWNFILEDIALOG_H

#include <bases/PlatformDialog.h>
class GlobalSettings;
class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
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

    QLineEdit *m_fileName;
    FileModel *m_model;
    QString m_dir;
    QLabel *m_extension;
    QPushButton *m_actionButton;
    QListView *m_view;
    Type m_type;
    QPushButton *m_upButton;
    GlobalSettings const &m_settings;

public:
    OwnFileDialog(
        QWidget *parent,
        Type type,
        const QString &caption,
        const QString &dir,
        const QString &extension,
        const GlobalSettings &settings
    );
    ~OwnFileDialog();

    static QString ExecuteFileDialog(Type type,
            QWidget *parent,
            const QString &caption,
            const QString &dir,
            const QString &filter,
            GlobalSettings const &settings);

private slots:
    void viewClicked(const QModelIndex &index);
    void createFolder();
    void goUp();
    void fileNameChanged(QString const &fileName);
    void fileSelected();
};

} //namespace file

#endif // OWNFILEDIALOG_H
