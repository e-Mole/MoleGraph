#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <FormDialogColor.h>
#include <QColor>

class Channel;
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
    void _MoveLastHorizontalToVertical();
    virtual void BeforeAccept();
    void ColorChanged(QColor &color);

    Context const & m_context;
    Channel *m_channel;
    QLineEdit * m_name;
	QLineEdit * m_units;
	QCheckBox * m_xAxis;
	QCheckBox * m_toRightSide;
    QComboBox * m_shapeComboBox;
    QComboBox * m_axisComboBox;
    QColor m_color;


    Channel * m_lastHorrizontalChannel;
public:
    ChannelSettings(Channel *channel, Context const &context);
signals:

private slots:
    void axisChanged(int index);
};

#endif // CHANNELSETTINGS_H
