#ifndef PORTLISTDIALOG_H
#define PORTLISTDIALOG_H

#include <QDialog>
#include <QList>

class QCloseEvent;
class QSettings;
class PortListWidget;

namespace hw { class HwSink; class PortInfo; }
class PortListDialog : public QDialog
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);

    PortListWidget *m_mainWidget;
    bool m_close;
public:
    PortListDialog(hw::HwSink &hwSink, QList<hw::PortInfo> const& portInfos, QSettings &settings);
    bool CloseApp()
    { return m_close; }
signals:

public slots:
    void closeClicked(bool checked);
    void refreshClicked(bool checked);
};

#endif // PORTLISTDIALOG_H
