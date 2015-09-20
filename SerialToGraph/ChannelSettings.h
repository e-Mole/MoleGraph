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
	QLineEdit * m_units;
	QCheckBox * m_selected;
	QCheckBox * m_xAxis;
	QCheckBox * m_toRightSide;
public:
	ChannelSettings(QString const &title, const QString &units, bool selected, bool xAxe, bool toRightSide, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QString GetName();
	bool GetSelected();
	QString GetUnits();
signals:

public slots:
};

#endif // CHANNELSETTINGS_H
