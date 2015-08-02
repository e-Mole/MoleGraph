#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QGroupBox.h>

class QLabel;
class QCheckBox;
class QBoxLaout;
class QColor;
class QString;

class Channel : public QGroupBox
{
	Q_OBJECT
public:
	Channel(QWidget *parent, QString const &name, QColor const &valueColor, bool samples);
	~Channel();

	void mousePressEvent(QMouseEvent * event);

	QVector<double> m_values;
	QLabel *m_selectedValue;
	QCheckBox *m_enabled;

signals:
	void enableChanged();
private slots:
	void checkBoxStateChanged(int state);
};

#endif // CHANNEL_H
