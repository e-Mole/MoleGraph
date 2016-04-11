#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <bases/PlatformDialog.h>
class QUrl;
class AboutDialog : public bases::PlatformDialog
{
    Q_OBJECT
public:
    AboutDialog(QWidget *parent);

signals:

private slots:
    void anchorClicked(QUrl const &url);
};

#endif // ABOUTDIALOG_H
