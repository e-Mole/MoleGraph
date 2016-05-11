#include "AboutDialog.h"
#include <QDesktopServices>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>
#include <GitVersion.h> //will be generated

#define TEXT(...) #__VA_ARGS__

AboutDialog::AboutDialog(QWidget *parent) :
    bases::PlatformDialog(parent, tr("About"))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QTextBrowser *tb = new QTextBrowser(this);
    tb->setOpenLinks(false);
    tb->append(QString("<html><b>%1</b</html>").arg(TARGET));
    tb->append("");
    tb->append(QString(tr("Version: %1")).arg(GIT_VERSION));
    tb->append("Copyright © 2016 <html><a href='http://www.e-mole.cz'>e-Mole</a></html>");
    tb->append(QString(tr("Used libraries: %1, %2")).
        arg(QString("<html><a href='http://www.qt.io'>Qt %1</a></html>").arg(QT_VERSION_STR)).
        arg("<html><a href='http://www.qcustomplot.com'>QCustomPlot</a></html>")
    );
    tb->append("");
    tb->append(tr(TEXT(
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
    )));
    tb->append("");
    tb->append(tr(TEXT(
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
    )));
    tb->adjustSize();
#if not defined(Q_OS_ANDROID)
    tb->setMinimumSize(400, 250);
#endif
    tb->moveCursor (QTextCursor::Start);
    connect (tb, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));
    layout->addWidget(tb);
}

void AboutDialog::anchorClicked(QUrl const &url)
{
    QDesktopServices::openUrl(url);
}
