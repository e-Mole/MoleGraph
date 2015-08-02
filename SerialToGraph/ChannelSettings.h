#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QDialog>

class QLineEdit;
class QString;
class QCheckBox;

class ChannelSettings : public QDialog
{
    Q_OBJECT

    QLineEdit * m_name;
	QCheckBox * m_selected;
	QCheckBox * m_xAxis;
public:
	ChannelSettings(QString const &title, bool selected, bool xAxe, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QString GetName();
	bool GetSelected();
signals:

public slots:
};

#endif // CHANNELSETTINGS_H
