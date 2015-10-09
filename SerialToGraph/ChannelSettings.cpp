#include "ChannelSettings.h"
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

ChannelSettings::ChannelSettings
(
	const QString &title,
	const QString &units,
	bool selected,
    bool samples,
	bool toRightSide,
    unsigned shape,
	QWidget * parent,
	Qt::WindowFlags f
) :
	QDialog(parent, f),
    m_name(NULL),
    m_shape(NULL)
{
    setWindowTitle(tr("Channel settings"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QGridLayout *gridLaout = new QGridLayout(this);
	mainLayout->addLayout(gridLaout);

    unsigned row = 0;

    if (!samples)
    {
        QLabel *selectedLabel = new QLabel(tr("Selected"), this);
        gridLaout->addWidget(selectedLabel, row, 0);
        m_selected = new QCheckBox(this);
        m_selected->setChecked(selected);
        connect(m_selected, SIGNAL(clicked(bool)), this , SLOT(selectedCheckboxClicked(bool)));
        gridLaout->addWidget(m_selected, row++, 1);
    }

	QLabel *nameLabel = new QLabel(tr("Title"), this);
	gridLaout->addWidget(nameLabel, row,0);
    m_name = new QLineEdit(title, this);
	gridLaout->addWidget(m_name, row++, 1);

    if (!samples)
    {
        QLabel *unitsLabel = new QLabel(tr("Units"), this);
        gridLaout->addWidget(unitsLabel, row,0);
        m_units = new QLineEdit(units, this);
        gridLaout->addWidget(m_units, row++, 1);

        QLabel *toRigtSideLabel = new QLabel(tr("To right side"), this);
        gridLaout->addWidget(toRigtSideLabel, row, 0);
        m_toRightSide = new QCheckBox(this);
        m_toRightSide->setChecked(toRightSide);
        gridLaout->addWidget(m_toRightSide, row++, 1);

        _InitializeShapeCombo(gridLaout, row++, shape);
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);

    QPushButton *store = new QPushButton(tr("Store"), this);
    buttonLayout->addWidget(store);
    connect(store, SIGNAL(clicked(bool)), this, SLOT(accept()));

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

    if (!samples)
    {
        //set intitial state
        selectedCheckboxClicked(selected);
    }
}

void ChannelSettings::_InitializeShapeCombo(QGridLayout *gridLaout, unsigned row, unsigned shapeIndex)
{
    QLabel *shapeLabel = new QLabel(tr("Shape"), this);
    gridLaout->addWidget(shapeLabel, row,0);

    m_shape = new QComboBox(this);

    m_shape->addItem(tr("Cross"));
    m_shape->addItem(tr("Plus"));
    m_shape->addItem(tr("Circle"));
    m_shape->addItem(tr("Disc"));
    m_shape->addItem(tr("Square"));
    m_shape->addItem(tr("Diamond"));
    m_shape->addItem(tr("Star"));
    m_shape->addItem(tr("Triangle"));
    m_shape->addItem(tr("Inverted Triangle"));
    m_shape->addItem(tr("Cross and Square"));
    m_shape->addItem(tr("Plus and Square"));
    m_shape->addItem(tr("Cross and Circle"));
    m_shape->addItem(tr("Plus and Circle"));
    m_shape->addItem(tr("Peace"));

    m_shape->setCurrentIndex(shapeIndex);

    gridLaout->addWidget(m_shape, row, 1);

}
void ChannelSettings::selectedCheckboxClicked(bool checked)
{
    m_name->setEnabled(checked);
    m_units->setEnabled(checked);
    m_toRightSide->setEnabled(checked);
    m_shape->setEnabled(checked);
}

QString ChannelSettings::GetName()
{
    return m_name->text();
}

bool ChannelSettings::GetSelected()
{
	return m_selected-> isChecked();
}

QString ChannelSettings::GetUnits()
{
	return m_units->text();
}

bool ChannelSettings::IsSetToRightSide()
{
    return m_toRightSide->isChecked();
}

unsigned ChannelSettings::GetShapeIndex()
{
    return m_shape->currentIndex();
}
