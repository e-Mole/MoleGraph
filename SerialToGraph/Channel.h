#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QObject>
#include <QColor>

class Axis;
class QString;
struct Context;
class Channel : public QObject
{
    friend class ChannelSettings;
	Q_OBJECT

    Context const & m_context;
    QString m_title;
	unsigned m_index;
	QVector<double> m_values;
    bool m_visible;
    QColor m_color;
	QString m_units;
    double m_channelMinValue;
    double m_channelMaxValue;
    Axis *m_axis;
    unsigned m_selectedValueIndex;
    unsigned m_shapeIndex;

public:
    Channel(QWidget *parent, Context const & context, int index, QString const &name, QColor const &color, Axis * axis, unsigned shapeIndex);
	~Channel();

    bool IsVisible();
	QColor &GetColor() { return m_color; }
	unsigned GetIndex() { return m_index; }
	QString GetName();
	QString GetUnits();

	unsigned GetValueCount()
	{ return m_values.size();}

	double GetValue(unsigned index)
	{ return m_values[index]; }

	void SelectValue(unsigned index);
	void AddValue( double value);
	void ClearValues();

    Axis * GetAxis()
    { return m_axis; }

    double GetMinValue()
    { return m_channelMinValue; }

    double GetMaxValue()
    { return m_channelMaxValue; }

    void SetAxisRange(double min, double max);

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
