#ifndef PLATFORMDIALOG_H
#define PLATFORMDIALOG_H

#include <QDialog>
class QWidget;
class QLayout;
class QString;
namespace bases
{
class PlatformDialog : public QDialog
{
    Q_OBJECT

    QWidget *m_viewport;
public:
    explicit PlatformDialog(QWidget *parent, QString const &title);

    void setLayout(QLayout *layout);
signals:

public slots:
    int exec();
};

}//namespace bases

#endif // PLATFORMDIALOG_H
