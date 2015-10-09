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
	QString m_units;
	bool m_toRightSide;
    unsigned m_axisNumber;
    double m_channelMinValue;
    double m_channelMaxValue;
    unsigned m_attachedTo;
    bool m_samples;
    unsigned m_selectedValueIndex;
    unsigned m_shapeIndex;
public:
    Channel(QWidget *parent, int index, QString const &name, QColor const &color, bool samples, unsigned shapeIndex);
	~Channel();

	bool IsSelected();
	QColor &GetColor() { return m_color; }
	unsigned GetIndex() { return m_index; }
	QString GetName();
	QString GetUnits();
	bool ToRightSide()
	{ return m_toRightSide; }

	unsigned GetValueCount()
	{ return m_values.size();}

	double GetValue(unsigned index)
	{ return m_values[index]; }

	void SelectValue(unsigned index);
	void AddValue( double value);
	void ClearValues();

    void SetAxisNumber(unsigned number)
    { m_axisNumber = number; }

    unsigned GetAxisNumber()
    { return m_axisNumber; }

    double GetMinValue()
    { return m_channelMinValue; }

    double GetMaxValue()
    { return m_channelMaxValue; }

    void SetAxisRange(double min, double max);

    void SetAttachedTo(unsigned channelIndex)
    { m_attachedTo = channelIndex; }

    void ResetAttachedTo()
    { m_attachedTo = ~0; }

    bool IsAttached()
    {   return m_attachedTo != ~0; }

    unsigned AttachedTo()
    { return m_attachedTo; }

    bool IsSampleChannel()
    { return m_samples; }

    unsigned GetShapeIndex()
    { return m_shapeIndex; }

signals:
	void stateChanged();
private slots:
    void checkBoxClicked(bool checked);
};

#endif // CHANNEL_H
