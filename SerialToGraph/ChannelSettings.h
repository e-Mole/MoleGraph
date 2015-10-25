#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QDialog>
class Channel;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLineEdit;
class QString;

class ChannelSettings : public QDialog
{
    Q_OBJECT

    void _InitializeShapeCombo(QGridLayout *gridLaout, unsigned row, unsigned shapeIndex);

    Channel *m_channel;
    QLineEdit * m_name;
	QLineEdit * m_units;
	QCheckBox * m_xAxis;
	QCheckBox * m_toRightSide;
    QComboBox * m_shape;
public:
    ChannelSettings(Channel *channel);
signals:

public slots:
    void storeAndAccept();
};

#endif // CHANNELSETTINGS_H
