#include "FileDialog.h"
#include <QDir>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>

FileDialog::FileDialog(QWidget *parent, bool open, QString const &extension):
    bases::PlatformDialog(parent, open ? tr("Open File") : tr("Save File"))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QListView *view = new QListView(this);
    layout->addWidget(view);

    QFileSystemModel *fsm = new QFileSystemModel(this);
    fsm->setRootPath(QDir::currentPath());
    fsm->setNameFilterDisables(false);
    fsm->setNameFilters(QStringList("*." + extension));

    view->setModel(fsm);
    view->setRootIndex(fsm->index(QDir::currentPath()));
    view->setViewMode(QListView::IconMode);

    QHBoxLayout *fileOpenLaout = new QHBoxLayout(this);
    layout->addLayout(fileOpenLaout);

    QLineEdit *fileName = new QLineEdit(this);
    fileName->setDisabled(open);
    fileOpenLaout->addWidget(fileName);

    QLabel *label = new QLabel("." + extension, this);
    fileOpenLaout->addWidget(label);

    QPushButton *actionButton = new QPushButton(open ? tr("Open") : tr("Save"), this);
    fileOpenLaout->addWidget(actionButton);
    connect(actionButton, SIGNAL(clicked()), this, SLOT(accept()));
}

FileDialog::~FileDialog()
{

}

