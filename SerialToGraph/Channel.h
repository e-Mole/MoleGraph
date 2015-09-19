#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QGroupBox>
#include <QColor>

class QLabel;
class QCheckBox;
class QBoxLaout;
class QString;

class Channel : public QGroupBox
{
	Q_OBJECT

	void _DisplayNAValue();
	void mousePressEvent(QMouseEvent * event);

	unsigned m_index;
	QVector<double> m_values;
	QLabel *m_selectedValue;
	QCheckBox *m_enabled;
	QColor m_color;

public:
	Channel(QWidget *parent, int index, QString const &name, QColor const &color, bool samples);
	~Channel();

	bool IsSelected();
	QColor &GetColor() { return m_color; }
	unsigned GetIndex() { return m_index; }
	QString GetName();

	unsigned GetValueCount()
	{ return m_values.size();}

	double GetValue(unsigned index)
	{ return m_values[index]; }

	void SelectValue(unsigned index);
	void AddValue( double value);
	void ClearValues();

	void Enable(bool enable);
signals:
	void enableChanged();
private slots:
	void checkBoxStateChanged(int state);
};

#endif // CHANNEL_H
