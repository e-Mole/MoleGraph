#include "FileDialog.h"
#include <bases/FormDialogBase.h>
#include <QCommonStyle>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QModelIndex>
#include <QPushButton>
#include <QScroller>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#if defined(Q_OS_ANDROID)
#   include <QtAndroidExtras/QAndroidJniObject>
#endif


namespace{

class AddDirDialog : public bases::FormDialogBase
{
    QLineEdit *m_dirName;

    bool BeforeAccept(){ return true; }

public:
    AddDirDialog(QWidget *parent) :
        bases::FormDialogBase(parent, tr("Add Directory")),
        m_dirName(new QLineEdit(this))
    {
        m_formLayout->addRow(tr("Directory Name"), m_dirName);

    }
    QString GetDirName() {return m_dirName->text();}
signals:

public slots:
};

}
FileDialog::FileDialog(
    QWidget *parent, bool open, QString const & caption, QString const &dir, QString const &filter):
    bases::PlatformDialog(parent, caption),
    m_fileName(new QLineEdit(this)),
    m_model(new QFileSystemModel(this)),
    m_dir(dir),
    m_extension(new QLabel(filter.mid(filter.lastIndexOf(".")), this)),
    m_actionButton(new QPushButton(open ? tr("Open") : tr("Save"), this)),
    m_view(new QListView(this)),
    m_open(open)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QCommonStyle style;
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    layout->addLayout(buttonLayout);
    QPushButton *upButton =
            new QPushButton(style.standardIcon(QStyle::SP_FileDialogToParent), tr("Go Up"), this);
    buttonLayout->addWidget(upButton);
    connect(upButton, SIGNAL(released()), this, SLOT(goUp()));

    QPushButton *newFolderButton =
            new QPushButton(style.standardIcon(QStyle::SP_FileDialogNewFolder), tr("Add Directory"), this);
    newFolderButton->setDisabled(open);
    buttonLayout->addWidget(newFolderButton);
    connect(newFolderButton, SIGNAL(released()), this, SLOT(createFolder()));

    m_model->setRootPath(m_dir);
    m_model->setNameFilterDisables(false);
    m_model->setNameFilters(QStringList(filter));

    m_view->setModel(m_model);
    m_view->setRootIndex(m_model->index(m_dir));
    m_view->setResizeMode(QListView::Adjust);
    m_view->setIconSize(QSize(32,32));
    m_view->setSpacing(5);
    m_view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    QScroller::grabGesture(m_view->viewport(), QScroller::LeftMouseButtonGesture);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(viewClicked(QModelIndex)));

    layout->addWidget(m_view);

    QHBoxLayout *fileOpenLayout = new QHBoxLayout(this);
    layout->addLayout(fileOpenLayout);

    m_fileName->setDisabled(open);
    connect(m_fileName, SIGNAL(textChanged(QString)), this, SLOT(fileNameChanged(QString)));
    fileOpenLayout->addWidget(m_fileName);
    fileOpenLayout->addWidget(m_extension);

    m_actionButton->setDisabled(true);
    fileOpenLayout->addWidget(m_actionButton);
    connect(m_actionButton, SIGNAL(clicked()), this, SLOT(fileSelected()));
}

void FileDialog::fileSelected()
{
    if (!m_open)
    {
        accept();
        return;
    }

    QFileInfo file(_GetFilePath());
    if (file.exists())
    {
        if (file.isDir())
        {

            QMessageBox::critical(this, "", tr("Directory alredy contains subdirectory with the same name."));
            return;
        }

        QMessageBox question(
           QMessageBox::Question,
            "",
            tr("Directory already contains file with the same name. Rewrite it?"),
            QMessageBox::Yes | QMessageBox::No,
            this
        );

        question.setDefaultButton(QMessageBox::No);
        question.setEscapeButton(QMessageBox::No);
        question.setButtonText(QMessageBox::Yes, tr("Rewrite"));
        question.setButtonText(QMessageBox::No, tr("Cancel"));


        if (QMessageBox::No == question.exec())
            return;
    }
    accept();
}
void FileDialog::fileNameChanged(QString const &fileName)
{
    m_actionButton->setEnabled(fileName.size() != 0);
}

void FileDialog::createFolder()
{
    AddDirDialog dialog(this);
    if (QDialog::Accepted == dialog.exec() && dialog.GetDirName().size() > 0)
        m_model->mkdir(m_view->rootIndex(), dialog.GetDirName());
}

void FileDialog::goUp()
{
    m_view->setRootIndex(m_model->parent(m_view->rootIndex()));
}

void FileDialog::viewClicked(const QModelIndex &index)
{
    if (m_model->isDir(index))
    {
        m_view->setRootIndex(index);
        m_fileName->setText("");
        m_dir = m_model->filePath(index);
    }
    else
    {
        QString fileName = m_model->fileName(index);
        m_fileName->setText(fileName.left(fileName.lastIndexOf(".")));
    }
}

FileDialog::~FileDialog()
{
}

QString FileDialog::_GetFilePath()
{
    return m_dir + "/" + m_fileName->text() + m_extension->text();
}

QString FileDialog::_ExecuteFileDialog(
        QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter,
        bool open)
{

    QString directory = dir;
#if defined(Q_OS_ANDROID)
    QAndroidJniObject storageDirectory =
        QAndroidJniObject::callStaticObjectMethod(
            "android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;"
        );
    directory = storageDirectory.toString();
#endif

    FileDialog * dialog = new FileDialog(parent, open, caption, directory, filter);
    if (QDialog::Accepted == dialog->exec())
        return dialog->_GetFilePath();
    else
        return "";
}

QString FileDialog::getOpenFileName(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    const QString &filter
)
{
#if defined(Q_OS_ANDROID)
    return _ExecuteFileDialog(parent, caption, dir, filter, true);
#else
    return QFileDialog::getOpenFileName(parent, caption, dir, filter);
#endif
}

QString FileDialog::getSaveFileName(
    QWidget *parent,
    const QString &caption,
    const QString &dir,
    const QString &filter)
{
#if defined(Q_OS_ANDROID)
    return _ExecuteFileDialog(parent, caption, dir, filter, false);
#else
    return QFileDialog::getSaveFileName(parent, caption, dir, filter);
#endif
}

