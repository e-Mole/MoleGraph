#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QObject>
#include <QColor>

class QString;

class Channel : public QObject
{
    friend class ChannelSettings;
	Q_OBJECT

    QString m_title;
	unsigned m_index;
	QVector<double> m_values;
    bool m_visible;
    QColor m_color;
	QString m_units;
	bool m_toRightSide;
    unsigned m_axisNumber;
    double m_channelMinValue;
    double m_channelMaxValue;
    signed m_attachedTo;
    bool m_samples;
    unsigned m_selectedValueIndex;
    unsigned m_shapeIndex;
public:
    Channel(QWidget *parent, int index, QString const &name, QColor const &color, bool samples, unsigned shapeIndex);
	~Channel();

    bool IsVisible();
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
    void selectedValueChanged(double value);
    void valuesCleared();
public slots:
    void channelSelectionChanged(bool selected, bool signal);
};

#endif // CHANNEL_H
