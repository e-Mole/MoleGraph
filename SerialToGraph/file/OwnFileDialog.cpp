#include "OwnFileDialog.h"
#include <bases/FormDialogBase.h>
#include <file/FileModel.h>
#include <MyMessageBox.h>
#include <QCommonStyle>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
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

namespace file
{

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

} //namespace

OwnFileDialog::OwnFileDialog(
    QWidget *parent,
    Type type,
    QString const & caption,
    QString const &dir,
    QString const &filter,
    QString const &limit
):
    bases::PlatformDialog(parent, caption),
    m_fileName(new QLineEdit(this)),
    m_model(new FileModel(this)),
    m_dir(dir),
    m_extension(new QLabel(filter.mid(filter.lastIndexOf(".")), this)),
    m_actionButton(new QPushButton(_GetActionButtonText(type), this)),
    m_view(new QListView(this)),
    m_type(type),
    m_upButton(NULL),
    m_limit(limit)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QCommonStyle style;
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);
    m_upButton =
            new QPushButton(style.standardIcon(QStyle::SP_FileDialogToParent), tr("Go Up"), this);

    _CheckUpButton();
    buttonLayout->addWidget(m_upButton);
    connect(m_upButton, SIGNAL(released()), this, SLOT(goUp()));

    QPushButton *newFolderButton =
            new QPushButton(style.standardIcon(QStyle::SP_FileDialogNewFolder), tr("Add Directory"), this);
    newFolderButton->setDisabled(type == Type_OpenFile);
    buttonLayout->addWidget(newFolderButton);
    connect(newFolderButton, SIGNAL(released()), this, SLOT(createFolder()));

    m_model->SetRootPath(m_dir);
    m_model->SetNameFilters(QStringList(filter));

    m_view->setModel(m_model);
    m_view->setRootIndex(m_model->Index(m_dir));
    m_view->setResizeMode(QListView::Adjust);
    m_view->setIconSize(QSize(32,32));
    m_view->setSpacing(5);
    m_view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);

#if defined(Q_OS_ANDROID)
    //because of bug on Lenovo. there is not seeable selected text
    m_view->setStyleSheet(
        "QListView::item:selected { background: LightBlue; color: black; }"
    );
#endif
    QScroller::grabGesture(m_view->viewport(), QScroller::LeftMouseButtonGesture);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(viewClicked(QModelIndex)));

    layout->addWidget(m_view);

    QHBoxLayout *fileOpenLayout = new QHBoxLayout();
    layout->addLayout(fileOpenLayout);

    m_fileName->setDisabled(type == Type_OpenFile);
    m_fileName->setHidden(type == Type_SelectDir);
    connect(m_fileName, SIGNAL(textChanged(QString)), this, SLOT(fileNameChanged(QString)));
    fileOpenLayout->addWidget(m_fileName);

    m_extension->setHidden(type == Type_SelectDir);
    fileOpenLayout->addWidget(m_extension);

    m_actionButton->setEnabled(m_type == Type_SelectDir);
    fileOpenLayout->addWidget(m_actionButton);
    connect(m_actionButton, SIGNAL(clicked()), this, SLOT(fileSelected()));
}

void OwnFileDialog::_CheckUpButton()
{
    m_upButton->setDisabled(m_limit == m_dir);
}
QString OwnFileDialog::_GetActionButtonText(Type type)
{
    switch (type)
    {
    case Type_OpenFile:
        return tr("Open");
    case Type_SaveFile:
        return tr("Save");
    case Type_SelectDir:
        return tr("Select");
    }
    return "";
}

void OwnFileDialog::fileSelected()
{
    if (m_type == Type_OpenFile || m_type == Type_SelectDir)
    {
        accept();
        return;
    }

    QFileInfo file(_GetFilePath());
    if (file.exists())
    {
        if (file.isDir())
        {

            MyMessageBox::critical(this, tr("Directory alredy contains subdirectory with the same name."));
            return;
        }

        if (MyMessageBox::StandardButton::No ==
            MyMessageBox::question(
                this,
                tr("Directory already contains file with the same name. Rewrite it?"),
                tr("Rewrite")
            )
        )
            return;
    }
    accept();
}
void OwnFileDialog::fileNameChanged(QString const &fileName)
{
    m_actionButton->setEnabled(fileName.size() != 0);
}

void OwnFileDialog::createFolder()
{
    AddDirDialog dialog(this);
    if (QDialog::Accepted == dialog.exec() && dialog.GetDirName().size() > 0)
        m_model->Mkdir(m_view->rootIndex(), dialog.GetDirName());
}

void OwnFileDialog::goUp()
{
    _ChangeDir(m_model->parent(m_view->rootIndex()));
}

void OwnFileDialog::_ChangeDir(const QModelIndex &index)
{
    m_view->setRootIndex(index);
    m_model-> sort(0);
    m_fileName->setText("");
    m_dir = m_model->FilePath(index);
    _CheckUpButton();
}
void OwnFileDialog::viewClicked(const QModelIndex &index)
{
    if (m_model->IsDir(index))
        _ChangeDir(index);
    else
    {
        QString fileName = m_model->FileName(index);
        m_fileName->setText(fileName.left(fileName.lastIndexOf(".")));
    }
}

OwnFileDialog::~OwnFileDialog()
{
}

QString OwnFileDialog::_GetFilePath()
{
    if (m_type == Type_SelectDir)
        return m_dir;
    else
        return m_dir + "/" + m_fileName->text() + m_extension->text();
}

QString OwnFileDialog::ExecuteFileDialog(
        Type type,
        QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter,
        const QString &limit)
{
    QString directory = dir;
#if defined(Q_OS_ANDROID)
    if (dir == "./")
    {
        QAndroidJniObject storageDirectory =
            QAndroidJniObject::callStaticObjectMethod(
                "android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;"
            );
        directory = storageDirectory.toString();
    }
#endif

    OwnFileDialog * dialog = new OwnFileDialog(
        parent, type, caption, directory, filter, limit);
    if (QDialog::Accepted == dialog->exec())
        return dialog->_GetFilePath();
    else
        return "";
}
} //namespace file
