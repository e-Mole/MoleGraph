#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <bases/FormDialogColor.h>
#include <QColor>

class Axis;
class ChannelBase;
class SampleChannel;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QLineEdit;
class QString;
struct Context;
class ChannelSettings : public bases::FormDialogColor
{
    Q_OBJECT

    void _InitializeShapeCombo();
    void _InitializeAxisCombo();
    bool _MoveLastHorizontalToVertical();
    virtual bool BeforeAccept();
    void _InitializeTimeFeatures();
    void _RefillAxisCombo();
    bool _AxisCheckForRealTimeMode();
    void _InitializePenStyle();

    Context const & m_context;
    ChannelBase *m_channel;
    QLineEdit * m_name;
	QLineEdit * m_units;
    QComboBox * m_shapeComboBox;
    QComboBox * m_axisComboBox;
    QComboBox * m_style;
    QComboBox * m_timeUnits;
    QComboBox * m_format;
    QComboBox * m_penStyle;
public:
    ChannelSettings(ChannelBase *channel, Context const &context);
signals:

private slots:
    void axisChanged(int index);
    void styleChanged(int index);
};

#endif // CHANNELSETTINGS_H
