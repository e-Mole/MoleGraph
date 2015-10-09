#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QDialog>

class QLineEdit;
class QString;
class QCheckBox;
class QGridLayout;
class QComboBox;
class ChannelSettings : public QDialog
{
    Q_OBJECT

    void _InitializeShapeCombo(QGridLayout *gridLaout, unsigned row, unsigned shapeIndex);

    QLineEdit * m_name;
	QLineEdit * m_units;
	QCheckBox * m_selected;
	QCheckBox * m_xAxis;
	QCheckBox * m_toRightSide;
    QComboBox * m_shape;
public:
    ChannelSettings(QString const &title, const QString &units, bool selected, bool samples, bool toRightSide, unsigned shape, QWidget *parent, Qt::WindowFlags f = 0);
    QString GetName();
	bool GetSelected();
	QString GetUnits();
    bool IsSetToRightSide();
    unsigned GetShapeIndex();
signals:

public slots:
    void selectedCheckboxClicked(bool checked);
};

#endif // CHANNELSETTINGS_H
