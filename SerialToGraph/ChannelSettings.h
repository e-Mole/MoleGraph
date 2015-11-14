#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <FormDialogBase.h>
class Channel;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QLineEdit;
class QString;

class ChannelSettings : public FormDialogBase
{
    Q_OBJECT

    void _InitializeShapeCombo(QFormLayout *formLaout, unsigned shapeIndex);
    virtual void BeforeAccept();

    Channel *m_channel;
    QLineEdit * m_name;
	QLineEdit * m_units;
	QCheckBox * m_xAxis;
	QCheckBox * m_toRightSide;
    QComboBox * m_shape;
public:
    ChannelSettings(Channel *channel, QWidget *parent);
signals:

private slots:
};

#endif // CHANNELSETTINGS_H
