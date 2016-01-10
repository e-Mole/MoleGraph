#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <FormDialogColor.h>
#include <QColor>

class Axis;
class Channel;
class ChannelWithTime;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QLineEdit;
class QString;
struct Context;
class ChannelSettings : public FormDialogColor
{
    Q_OBJECT

    void _InitializeShapeCombo();
    void _InitializeAxisCombo();
    bool _MoveLastHorizontalToVertical();
    virtual bool BeforeAccept();
    void _InitializeTimeFeatures();

    Context const & m_context;
    Channel *m_channel;
    QLineEdit * m_name;
	QLineEdit * m_units;
    QComboBox * m_shapeComboBox;
    QComboBox * m_axisComboBox;
    QComboBox * m_style;
    QComboBox * m_timeUnits;
    QComboBox * m_format;
public:
    ChannelSettings(Channel *channel, Context const &context);
signals:

private slots:
    void axisChanged(int index);
    void styleChanged(int index);
};

#endif // CHANNELSETTINGS_H
