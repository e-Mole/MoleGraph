#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <FormDialogBase.h>
class Channel;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QLineEdit;
class QString;
struct Context;
class ChannelSettings : public FormDialogBase
{
    Q_OBJECT

    void _InitializeShapeCombo();
    void _InitializeAxisCombo();
    Channel *_MoveLastHorizontalToVertical();
    virtual void BeforeAccept();

    Context & m_context;
    Channel *m_channel;
    QLineEdit * m_name;
	QLineEdit * m_units;
	QCheckBox * m_xAxis;
	QCheckBox * m_toRightSide;
    QComboBox * m_shapeComboBox;
    QComboBox * m_axisComboBox;
public:
    ChannelSettings(Channel *channel, QWidget *parent, Context &context);
signals:

private slots:
    void axisChanged(int index);
};

#endif // CHANNELSETTINGS_H
