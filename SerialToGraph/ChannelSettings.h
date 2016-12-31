#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <bases/FormDialogColor.h>
#include <QColor>

namespace bases { class ComboBox;}
class Axis;
class ChannelBase;
class SampleChannel;
class QCheckBox;
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
    int _GetCurrentPos();

    Context const & m_context;
    ChannelBase *m_channel;
    QLineEdit *m_currentValueControl;
    QLineEdit * m_name;
	QLineEdit * m_units;
    bases::ComboBox * m_shapeComboBox;
    bases::ComboBox * m_axisComboBox;
    bases::ComboBox * m_style;
    bases::ComboBox * m_timeUnits;
    bases::ComboBox * m_format;
    bases::ComboBox * m_penStyle;
    bool m_currentValueChanged;
    double m_currentValue;
public:
    ChannelSettings(ChannelBase *channel, Context const &context);
signals:

private slots:
    void axisChanged(int index);
    void styleChanged(int index);
    void currentValueChanged(QString const &content);
    void setOriginalValue(bool checked);
    void setNaValue(bool);
};

#endif // CHANNELSETTINGS_H
